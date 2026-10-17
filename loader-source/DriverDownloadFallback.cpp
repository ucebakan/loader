// App-local WinINet extension for one fixed driver-package URL only.
// No EXE/memory patching, driver execution, TLS changes, or authentication logic.
#include <Windows.h>
#include <wininet.h>
#include "FileIdentity.h"
#include <chrono>
#include <cstring>
#include <memory>
#include <mutex>
#include <unordered_map>
#include "WinInetExports.h"

namespace {
constexpr size_t MaxPackage = 16 * 1024 * 1024;
constexpr wchar_t PackageUrl[] = L"https://pub-fdaae9442b754a829739a8850fcc81a5.r2.dev/ks_driver";
constexpr wchar_t LocalHash[] = L"24DD330F14BE62CDED3D1837124014FBFF2ABDF6D209BD4893C931064B50604D";
constexpr size_t LocalSize = 182063;

HMODULE SystemWinInet() {
    static HMODULE module = [] {
        wchar_t path[MAX_PATH]{};
        if (!GetSystemDirectoryW(path, MAX_PATH)) return HMODULE{};
        auto full = std::filesystem::path(path) / L"wininet.dll";
        return LoadLibraryExW(full.c_str(), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    }();
    return module;
}

template<class Function> Function Original(const char* name) {
    auto module = SystemWinInet();
    auto address = module ? GetProcAddress(module, name) : nullptr;
    if (!address) RaiseFailFastException(nullptr, nullptr, 0);
    return reinterpret_cast<Function>(address);
}

std::filesystem::path OwnDirectory() {
    HMODULE module{};
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&OwnDirectory), &module);
    std::vector<wchar_t> path(32768);
    const auto count = GetModuleFileNameW(module, path.data(), static_cast<DWORD>(path.size()));
    if (!count || count >= path.size()) throw std::runtime_error("Cannot locate extension directory");
    return std::filesystem::path(std::wstring(path.data(), count)).parent_path();
}

void Log(const char* event) noexcept {
    try { std::ofstream(OwnDirectory() / L"driver-download.log", std::ios::app) << event << '\n'; }
    catch (...) { /* A log error must not change download behavior. */ }
}

std::wstring TargetUrl() {
#ifdef FALLBACK_TESTING
    wchar_t value[2048]{};
    const auto length = GetEnvironmentVariableW(L"LOADER_TEST_PACKAGE_URL", value, 2048);
    if (length && length < 2048) return value;
#endif
    return PackageUrl;
}

bool Target(LPCWSTR url) { return url && std::wstring(url) == TargetUrl(); }
bool Target(LPCSTR url) {
    if (!url) return false;
    auto target = TargetUrl();
    std::string ascii;
    for (wchar_t ch : target) { if (ch > 127) return false; ascii.push_back(static_cast<char>(ch)); }
    return std::string(url) == ascii;
}

// Reject empty, truncated, and non-ZIP HTTP bodies before handing them to the
// existing extractor. This is structural validation, not a new trust policy.
bool CompleteZip(const std::vector<unsigned char>& bytes) {
    if (bytes.size() < 22 || bytes.size() > MaxPackage) return false;
    if (memcmp(bytes.data(), "PK\x03\x04", 4) != 0) return false;
    const auto u16 = [&](size_t p) { return unsigned(bytes[p]) | (unsigned(bytes[p + 1]) << 8); };
    const auto u32 = [&](size_t p) { return size_t(u16(p)) | (size_t(u16(p + 2)) << 16); };
    const size_t first = bytes.size() > 65557 ? bytes.size() - 65557 : 0;
    for (size_t p = bytes.size() - 22;; --p) {
        if (!memcmp(bytes.data() + p, "PK\x05\x06", 4) && p + 22 + u16(p + 20) == bytes.size()) {
            if (u16(p + 4) || u16(p + 6) || !u16(p + 10) || u16(p + 8) != u16(p + 10)) return false;
            const size_t offset = u32(p + 16), size = u32(p + 12);
            return size >= 46 && offset <= p && size == p - offset &&
                !memcmp(bytes.data() + offset, "PK\x01\x02", 4);
        }
        if (p == first) break;
    }
    return false;
}

struct BufferedDownload {
    std::vector<unsigned char> bytes;
    size_t position{};
    std::mutex mutex;
};
std::mutex downloadsMutex;
std::unordered_map<HINTERNET, std::shared_ptr<BufferedDownload>> downloads;

using Read = BOOL(WINAPI*)(HINTERNET, LPVOID, DWORD, LPDWORD);
using Close = BOOL(WINAPI*)(HINTERNET);
using OpenA = HINTERNET(WINAPI*)(LPCSTR, DWORD, LPCSTR, LPCSTR, DWORD);
using Query = BOOL(WINAPI*)(HINTERNET, DWORD, LPVOID, LPDWORD, LPDWORD);
using SetOption = BOOL(WINAPI*)(HINTERNET, DWORD, LPVOID, DWORD);

bool ReadOnline(HINTERNET request, std::vector<unsigned char>& bytes) {
    if (!request) return false;
    DWORD status{}, size = sizeof(status);
    auto query = Original<Query>("HttpQueryInfoW");
    if (!query(request, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status, &size, nullptr) || status < 200 || status >= 300) return false;
    DWORD timeout = 10000;
    Original<SetOption>("InternetSetOptionW")(request, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
    const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(30);
    std::array<unsigned char, 32768> buffer{};
    for (;;) {
        if (std::chrono::steady_clock::now() > deadline) return false;
        DWORD received{};
        if (!Original<Read>("InternetReadFile")(request, buffer.data(), static_cast<DWORD>(buffer.size()), &received)) return false;
        if (!received) break;
        if (received > MaxPackage - bytes.size()) return false;
        bytes.insert(bytes.end(), buffer.begin(), buffer.begin() + received);
    }
    DWORD expected{}; size = sizeof(expected);
    if (query(request, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &expected, &size, nullptr) && expected != bytes.size()) return false;
    return CompleteZip(bytes);
}

HINTERNET SelectPackage(HINTERNET request) noexcept {
    try {
        auto content = std::make_shared<BufferedDownload>();
        if (ReadOnline(request, content->bytes)) {
            Log("online-package-selected");
        } else {
            if (request) { Original<Close>("InternetCloseHandle")(request); request = nullptr; }
            const auto local = OwnDirectory() / L"Offline520" / L"ks_driver.zip";
            if (std::filesystem::file_size(local) != LocalSize || loader_names::Sha256(local) != LocalHash)
                throw std::runtime_error("Local package identity mismatch");
            std::ifstream file(local, std::ios::binary);
            content->bytes.resize(LocalSize);
            file.read(reinterpret_cast<char*>(content->bytes.data()), LocalSize);
            if (!file || !CompleteZip(content->bytes)) throw std::runtime_error("Invalid local package");
            request = Original<OpenA>("InternetOpenA")("Loader offline package", INTERNET_OPEN_TYPE_DIRECT, nullptr, nullptr, 0);
            if (!request) throw std::runtime_error("Cannot create offline stream handle");
            Log("local-5.20-package-selected-after-download-failure");
        }
        { std::lock_guard<std::mutex> lock(downloadsMutex); downloads.emplace(request, std::move(content)); }
        SetLastError(ERROR_SUCCESS);
        return request;
    } catch (...) {
        if (request) Original<Close>("InternetCloseHandle")(request);
        Log("package-download-failed-local-package-missing-or-invalid");
        SetLastError(ERROR_FILE_INVALID);
        return nullptr;
    }
}
}

extern "C" FARPROC ResolveForward(unsigned index) {
    const auto savedError = GetLastError();
    const auto& entry = ForwardExports[index];
    auto module = SystemWinInet();
    auto address = module ? GetProcAddress(module, entry.name ? entry.name : MAKEINTRESOURCEA(entry.ordinal)) : nullptr;
    if (!address) RaiseFailFastException(nullptr, nullptr, 0);
    SetLastError(savedError);
    return address;
}

extern "C" HINTERNET WINAPI HookInternetOpenUrlA(HINTERNET session, LPCSTR url, LPCSTR headers, DWORD length, DWORD flags, DWORD_PTR context) {
    using Function = HINTERNET(WINAPI*)(HINTERNET, LPCSTR, LPCSTR, DWORD, DWORD, DWORD_PTR);
    auto request = Original<Function>("InternetOpenUrlA")(session, url, headers, length, flags, context);
    return Target(url) ? SelectPackage(request) : request;
}

extern "C" HINTERNET WINAPI HookInternetOpenUrlW(HINTERNET session, LPCWSTR url, LPCWSTR headers, DWORD length, DWORD flags, DWORD_PTR context) {
    using Function = HINTERNET(WINAPI*)(HINTERNET, LPCWSTR, LPCWSTR, DWORD, DWORD, DWORD_PTR);
    auto request = Original<Function>("InternetOpenUrlW")(session, url, headers, length, flags, context);
    return Target(url) ? SelectPackage(request) : request;
}

extern "C" BOOL WINAPI HookInternetReadFile(HINTERNET request, LPVOID buffer, DWORD capacity, LPDWORD received) {
    std::shared_ptr<BufferedDownload> content;
    { std::lock_guard<std::mutex> lock(downloadsMutex); auto it = downloads.find(request); if (it != downloads.end()) content = it->second; }
    if (!content) return Original<Read>("InternetReadFile")(request, buffer, capacity, received);
    if (!received || (!buffer && capacity)) { SetLastError(ERROR_INVALID_PARAMETER); return FALSE; }
    std::lock_guard<std::mutex> lock(content->mutex);
    *received = static_cast<DWORD>((std::min)(size_t(capacity), content->bytes.size() - content->position));
    if (*received) memcpy(buffer, content->bytes.data() + content->position, *received);
    content->position += *received;
    return TRUE;
}

extern "C" BOOL WINAPI HookInternetCloseHandle(HINTERNET request) {
    { std::lock_guard<std::mutex> lock(downloadsMutex); downloads.erase(request); }
    return Original<Close>("InternetCloseHandle")(request);
}
