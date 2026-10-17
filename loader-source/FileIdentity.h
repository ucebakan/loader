#pragma once
#include <Windows.h>
#include <bcrypt.h>
#include <array>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace loader_names {
inline constexpr wchar_t BundleHash[]=L"B39D96667DFECCE5D11E33D9DAB6744DCDE09F93C097047E676417307ECC3FBB";
inline constexpr std::uintmax_t BundleBytes=112938496;
inline constexpr wchar_t UiHash[]=L"8730BEBE0F4B72D42D4DC6D6FD31A2F2193DD928ED971DDB353EEA775C79285C";
inline constexpr std::uintmax_t UiBytes=15990784;

inline std::wstring Sha256(const std::filesystem::path& path) {
    struct Handles {
        BCRYPT_ALG_HANDLE algorithm{}; BCRYPT_HASH_HANDLE hash{};
        std::vector<UCHAR> state;
        ~Handles() {if(hash)BCryptDestroyHash(hash);if(algorithm)BCryptCloseAlgorithmProvider(algorithm,0);}
    } handles;
    auto Check=[](NTSTATUS status) {if(status<0)throw std::runtime_error("SHA-256 provider failed");};
    Check(BCryptOpenAlgorithmProvider(&handles.algorithm,BCRYPT_SHA256_ALGORITHM,nullptr,0));
    DWORD length{},written{};
    Check(BCryptGetProperty(handles.algorithm,BCRYPT_OBJECT_LENGTH,reinterpret_cast<PUCHAR>(&length),sizeof(length),&written,0));
    handles.state.resize(length);
    Check(BCryptCreateHash(handles.algorithm,&handles.hash,handles.state.data(),length,nullptr,0,0));
    std::ifstream input(path,std::ios::binary);
    if(!input)throw std::runtime_error("Cannot read executable identity");
    std::array<UCHAR,65536> buffer{};
    while(input) {
        input.read(reinterpret_cast<char*>(buffer.data()),buffer.size());
        const auto count=input.gcount();
        if(count>0)Check(BCryptHashData(handles.hash,buffer.data(),static_cast<ULONG>(count),0));
    }
    if(!input.eof())throw std::runtime_error("Executable read failed");
    std::array<UCHAR,32> hash{};Check(BCryptFinishHash(handles.hash,hash.data(),static_cast<ULONG>(hash.size()),0));
    constexpr wchar_t digits[]=L"0123456789ABCDEF";
    std::wstring result;result.reserve(64);
    for(UCHAR byte:hash) {result.push_back(digits[byte>>4]);result.push_back(digits[byte&15]);}
    return result;
}

inline std::filesystem::path Resolve(const std::filesystem::path& directory,const std::wstring& expectedHash,std::uintmax_t expectedBytes) {
    std::filesystem::path selected;
    for(const auto& entry:std::filesystem::directory_iterator(directory)) {
        std::error_code error;
        if(!entry.is_regular_file(error)||error||_wcsicmp(entry.path().extension().c_str(),L".exe")!=0)continue;
        if(entry.file_size(error)!=expectedBytes||error)continue;
        if(Sha256(entry.path())!=expectedHash)continue;
        if(!selected.empty())throw std::runtime_error("Duplicate matching executable");
        selected=entry.path();
    }
    return selected;
}
}
