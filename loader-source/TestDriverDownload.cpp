// Harmless client fixture: downloads bytes only. Never extracts or runs tools.
#include <Windows.h>
#include <wininet.h>
#include <iostream>
#include "FileIdentity.h"
#include "WinInetExports.h"

int wmain(int argc, wchar_t** argv) {
    if (argc != 4) return 2;
    const auto library = std::filesystem::absolute(argv[1]);
    auto proxy = LoadLibraryExW(library.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
    if (!proxy) { std::cerr << "LOAD_ERROR " << GetLastError() << '\n'; return 3; }
    unsigned exports{};
    for (const auto& entry : ForwardExports) {
        if (!GetProcAddress(proxy, entry.name ? entry.name : MAKEINTRESOURCEA(entry.ordinal))) return 4;
        ++exports;
    }
    using Open = HINTERNET(WINAPI*)(LPCWSTR, DWORD, LPCWSTR, LPCWSTR, DWORD);
    using UrlA = HINTERNET(WINAPI*)(HINTERNET, LPCSTR, LPCSTR, DWORD, DWORD, DWORD_PTR);
    using UrlW = HINTERNET(WINAPI*)(HINTERNET, LPCWSTR, LPCWSTR, DWORD, DWORD, DWORD_PTR);
    using Read = BOOL(WINAPI*)(HINTERNET, LPVOID, DWORD, LPDWORD);
    using Close = BOOL(WINAPI*)(HINTERNET);
    using Set = BOOL(WINAPI*)(HINTERNET, DWORD, LPVOID, DWORD);
    const auto open = reinterpret_cast<Open>(GetProcAddress(proxy, "InternetOpenW"));
    const auto urlA = reinterpret_cast<UrlA>(GetProcAddress(proxy, "InternetOpenUrlA"));
    const auto urlW = reinterpret_cast<UrlW>(GetProcAddress(proxy, "InternetOpenUrlW"));
    const auto read = reinterpret_cast<Read>(GetProcAddress(proxy, "InternetReadFile"));
    const auto close = reinterpret_cast<Close>(GetProcAddress(proxy, "InternetCloseHandle"));
    const auto set = reinterpret_cast<Set>(GetProcAddress(proxy, "InternetSetOptionW"));
    wchar_t proxyAddress[256]{};
    const auto hasProxy = GetEnvironmentVariableW(L"LOADER_FIXTURE_PROXY", proxyAddress, 256);
    auto session = open(L"Offline520 fixture", hasProxy ? INTERNET_OPEN_TYPE_PROXY : INTERNET_OPEN_TYPE_DIRECT,
        hasProxy ? proxyAddress : nullptr, nullptr, 0);
    if (!session) return 5;
    DWORD timeout = 2000;
    set(session, INTERNET_OPTION_CONNECT_TIMEOUT, &timeout, sizeof(timeout));
    set(session, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeout, sizeof(timeout));
    std::wstring url = argv[2];
    HINTERNET request{};
    const DWORD flags = INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE;
    if (std::wstring(argv[3]) == L"A") {
        std::string ascii; for (wchar_t ch : url) ascii.push_back(static_cast<char>(ch));
        request = urlA(session, ascii.c_str(), nullptr, 0, flags, 0);
    } else request = urlW(session, url.c_str(), nullptr, 0, flags, 0);
    if (!request) { std::cout << "OPEN_FAILED " << GetLastError() << '\n'; close(session); return 10; }
    std::vector<unsigned char> bytes;
    std::array<unsigned char, 997> buffer{};
    for (;;) {
        DWORD count{};
        if (!read(request, buffer.data(), static_cast<DWORD>(buffer.size()), &count)) { close(request); close(session); return 11; }
        if (!count) break;
        bytes.insert(bytes.end(), buffer.begin(), buffer.begin() + count);
        if (bytes.size() > 17000000) return 12;
    }
    close(request); close(session);
    // Hash the received stream in memory; no executable or driver is written.
    BCRYPT_ALG_HANDLE algorithm{}; BCRYPT_HASH_HANDLE hash{};
    if (BCryptOpenAlgorithmProvider(&algorithm, BCRYPT_SHA256_ALGORITHM, nullptr, 0) < 0) return 13;
    std::array<unsigned char, 32> digest{};
    if (BCryptCreateHash(algorithm, &hash, nullptr, 0, nullptr, 0, 0) < 0 ||
        BCryptHashData(hash, bytes.data(), static_cast<ULONG>(bytes.size()), 0) < 0 ||
        BCryptFinishHash(hash, digest.data(), static_cast<ULONG>(digest.size()), 0) < 0) return 14;
    BCryptDestroyHash(hash); BCryptCloseAlgorithmProvider(algorithm, 0);
    std::cout << "OK exports=" << exports << " bytes=" << bytes.size() << " sha256=";
    const char* hex = "0123456789ABCDEF";
    for (auto byte : digest) std::cout << hex[byte >> 4] << hex[byte & 15];
    std::cout << '\n';
    return 0;
}
