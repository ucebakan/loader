#include "FileIdentity.h"
#include <iostream>

int wmain(int argc,wchar_t** argv) try {
    if(argc!=2)throw std::runtime_error("Expected package directory");
    const std::filesystem::path directory=argv[1];
    const auto bundle=loader_names::Resolve(directory,loader_names::BundleHash,loader_names::BundleBytes);
    const auto ui=loader_names::Resolve(directory,loader_names::UiHash,loader_names::UiBytes);
    if(bundle.empty()||ui.empty())throw std::runtime_error("Expected package executable missing");
    std::wcout<<L"Resolved bundle: "<<bundle.filename().wstring()<<L"\nResolved UI: "<<ui.filename().wstring()<<L"\n";
    std::wcout<<L"CHECK ONLY: no process launch, backend deployment, or hardware operation.\n";
    return 0;
} catch(const std::exception& error) {std::cerr<<error.what()<<"\n";return 1;}
