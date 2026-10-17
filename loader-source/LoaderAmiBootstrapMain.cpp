#include <Windows.h>
#include <shellapi.h>
#include "FileIdentity.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <thread>
#include <vector>

namespace
{
using namespace std::chrono_literals;

std::filesystem::path ModuleDirectory()
{
    std::vector<wchar_t> buffer(32768);
    const DWORD length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    return std::filesystem::path(std::wstring(buffer.data(), length)).parent_path();
}

#ifndef LOADER_BACKEND_520_ONLY
bool FilesEqual(const std::filesystem::path& left, const std::filesystem::path& right)
{
    std::error_code error;
    const auto leftSize = std::filesystem::file_size(left, error);
    if (error)
    {
        return false;
    }
    const auto rightSize = std::filesystem::file_size(right, error);
    if (error || leftSize != rightSize)
    {
        return false;
    }

    std::ifstream leftStream(left, std::ios::binary);
    std::ifstream rightStream(right, std::ios::binary);
    std::array<char, 65536> leftBuffer{};
    std::array<char, 65536> rightBuffer{};
    while (leftStream && rightStream)
    {
        leftStream.read(leftBuffer.data(), static_cast<std::streamsize>(leftBuffer.size()));
        rightStream.read(rightBuffer.data(), static_cast<std::streamsize>(rightBuffer.size()));
        const auto count = leftStream.gcount();
        if (count != rightStream.gcount() ||
            !std::equal(leftBuffer.begin(), leftBuffer.begin() + count, rightBuffer.begin()))
        {
            return false;
        }
    }
    return true;
}

bool PackageLooksStable(const std::filesystem::path& tools)
{
    const std::array<std::filesystem::path, 3> files{{
        tools / L"AMIDEWINx64.EXE",
        tools / L"amifldrv64.sys",
        tools / L"sdeven.sys",
    }};
    std::array<std::uintmax_t, 3> sizes{};
    std::error_code error;
    for (std::size_t index = 0; index < files.size(); ++index)
    {
        if (!std::filesystem::is_regular_file(files[index], error))
        {
            return false;
        }
        sizes[index] = std::filesystem::file_size(files[index], error);
        if (error || sizes[index] == 0)
        {
            return false;
        }
    }

    std::this_thread::sleep_for(250ms);
    for (std::size_t index = 0; index < files.size(); ++index)
    {
        if (std::filesystem::file_size(files[index], error) != sizes[index] || error)
        {
            return false;
        }

        const HANDLE file = CreateFileW(
            files[index].c_str(),
            GENERIC_READ,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);
        if (file == INVALID_HANDLE_VALUE)
        {
            return false;
        }
        CloseHandle(file);
    }
    return true;
}

bool CopyReplacing(const std::filesystem::path& source, const std::filesystem::path& destination)
{
    std::error_code error;
    std::filesystem::copy_file(
        source, destination, std::filesystem::copy_options::overwrite_existing, error);
    return !error;
}

bool DeployCompatibilityLayer(const std::filesystem::path& moduleDirectory)
{
    const std::filesystem::path tools = L"C:\\ks1\\AMITools";
    const std::filesystem::path originalExecutable = tools / L"AMIDEWINx64.EXE";
    const std::filesystem::path proxySource = moduleDirectory / L"AmiBackendProxy.exe";
    if (!std::filesystem::is_regular_file(proxySource))
    {
        return false;
    }

    if (FilesEqual(originalExecutable, proxySource))
    {
        return true;
    }

    if (!PackageLooksStable(tools))
    {
        return false;
    }

    const std::filesystem::path oldTools = L"C:\\ks1\\AMITools520";
    const std::filesystem::path newTools = L"C:\\ks1\\AMITools527";
    const std::filesystem::path assetTools = moduleDirectory / L"AMITools527";
    std::error_code error;
    std::filesystem::create_directories(oldTools, error);
    if (error)
    {
        return false;
    }
    std::filesystem::create_directories(newTools, error);
    if (error)
    {
        return false;
    }

    if (!CopyReplacing(originalExecutable, oldTools / L"AMIDEWINx64.EXE") ||
        !CopyReplacing(tools / L"amifldrv64.sys", oldTools / L"amifldrv64.sys") ||
        !CopyReplacing(assetTools / L"AMIDEWINx64.EXE", newTools / L"AMIDEWINx64.EXE") ||
        !CopyReplacing(assetTools / L"amifldrv64.sys", newTools / L"amifldrv64.sys") ||
        !CopyReplacing(assetTools / L"amigendrv64.sys", newTools / L"amigendrv64.sys"))
    {
        return false;
    }

    const std::filesystem::path stagedProxy = tools / L"AMIDEWINx64.compat.tmp";
    if (!CopyReplacing(proxySource, stagedProxy))
    {
        return false;
    }
    if (!MoveFileExW(
            stagedProxy.c_str(),
            originalExecutable.c_str(),
            MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
    {
        std::filesystem::remove(stagedProxy, error);
        return false;
    }

    // Keep the two AMIDEWIN versions physically separate. sdeven.sys remains in
    // AMITools because it belongs to the loader's unchanged kernel-driver flow.
    std::filesystem::remove(tools / L"amifldrv64.sys", error);
    std::filesystem::remove(L"C:\\ks1\\AMIToolsBackend\\selected.txt", error);
    return true;
}
#endif

std::wstring QuoteArgument(const std::wstring& argument)
{
    std::wstring result = L"\"";
    std::size_t backslashes = 0;
    for (const wchar_t ch : argument)
    {
        if (ch == L'\\')
        {
            ++backslashes;
        }
        else if (ch == L'\"')
        {
            result.append(backslashes * 2 + 1, L'\\');
            result.push_back(L'\"');
            backslashes = 0;
        }
        else
        {
            result.append(backslashes, L'\\');
            backslashes = 0;
            result.push_back(ch);
        }
    }
    result.append(backslashes * 2, L'\\');
    result.push_back(L'\"');
    return result;
}

std::wstring ForwardedArguments()
{
    const wchar_t* commandLine = GetCommandLineW();
    int argumentCount = 0;
    wchar_t** arguments = CommandLineToArgvW(commandLine, &argumentCount);
    if (arguments == nullptr)
    {
        return {};
    }

    std::wstring result;
    for (int index = 1; index < argumentCount; ++index)
    {
        result.push_back(L' ');
        result += QuoteArgument(arguments[index]);
    }
    LocalFree(arguments);
    return result;
}
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    const std::filesystem::path moduleDirectory = ModuleDirectory();
    std::filesystem::path originalLauncher;
    std::filesystem::path originalUi;
    try
    {
        originalLauncher = loader_names::Resolve(moduleDirectory, loader_names::BundleHash, loader_names::BundleBytes);
        originalUi = loader_names::Resolve(moduleDirectory, loader_names::UiHash, loader_names::UiBytes);
    }
    catch (const std::exception&)
    {
        MessageBoxW(nullptr, L"Dosyalar okunamadi veya ayni uygulamanin birden fazla kopyasi var. Klasorde tek bir ara baslatici ve tek bir ana uygulama birakin.", L"Loader - dosya secimi", MB_OK | MB_ICONERROR);
        return ERROR_INVALID_DATA;
    }
    const std::filesystem::path debugLog = moduleDirectory / L"serial_changer_debug.txt";

    if (originalLauncher.empty() || originalUi.empty())
    {
        MessageBoxW(
            nullptr,
            L"Ara baslatici veya ana uygulama ayni klasorde bulunamadi ya da icerigi bu surumle eslesmiyor. EXE dosyalarinin adlarini degistirebilirsiniz; birlikte ayni klasorde kalmalilar.",
            L"AMIDEWIN compatibility layer",
            MB_OK | MB_ICONERROR);
        return ERROR_FILE_NOT_FOUND;
    }

    SetEnvironmentVariableW(L"AMI_COMPAT_DEBUG_LOG", debugLog.c_str());
    std::wstring command = QuoteArgument(originalLauncher.wstring()) + ForwardedArguments();
    std::vector<wchar_t> mutableCommand(command.begin(), command.end());
    mutableCommand.push_back(L'\0');

    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(
            originalLauncher.c_str(),
            mutableCommand.data(),
            nullptr,
            nullptr,
            TRUE,
            0,
            nullptr,
            moduleDirectory.c_str(),
            &startup,
            &process))
    {
        const DWORD launchError = GetLastError();
        wchar_t details[1024]{};
        FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr,
            launchError, 0, details, static_cast<DWORD>(std::size(details)), nullptr);
        const std::wstring message = L"The original loader could not be started. Windows error " +
            std::to_wstring(launchError) + L": " + details;
        MessageBoxW(
            nullptr,
            message.c_str(),
            L"AMIDEWIN compatibility layer",
            MB_OK | MB_ICONERROR);
        return static_cast<int>(launchError);
    }

    CloseHandle(process.hThread);
    while (WaitForSingleObject(process.hProcess, 50) == WAIT_TIMEOUT)
    {
#ifndef LOADER_BACKEND_520_ONLY
        DeployCompatibilityLayer(moduleDirectory);
#endif
    }

    DWORD exitCode = ERROR_GEN_FAILURE;
    GetExitCodeProcess(process.hProcess, &exitCode);
    CloseHandle(process.hProcess);
    return static_cast<int>(exitCode);
}
