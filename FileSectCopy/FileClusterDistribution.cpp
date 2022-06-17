#include "FileClusterDistribution.h"

#include <string>
#include <Windows.h>
#include <iostream>
#include <filesystem>

FileClusterDistribution::FileClusterDistribution(TCHAR* path) {

    if (path == nullptr) {
        throw std::runtime_error("[FileClusterDistribution] Target file path is null pointer.");
    }

    // std::filesystem::path could be made from TCHAR[]
    file_path = path;
    // Check if the path has a drive letter
    file_driveletter = file_path.root_name().string().at(0);
    if (!std::isalpha(file_driveletter)) {
        std::cout << "Target File: " << file_path.string() << " does not have Drive Letter" << std::endl;
        throw std::runtime_error("[FileClusterDistribution] Target File: " + file_path.string() + " does not have Drive Letter");
    }
    std::cout << "[FileClusterDistribution] Target File: " << file_path.string() << "\nDrive Letter: " << file_driveletter << std::endl;


    // Get File Handle for the path
    file_handle = CreateFile(
        path,
        0x00,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (file_handle == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to obtain file handle of " + file_path.string() );
    }
    else {
        std::cout << "[FileClusterDistribution] Obtained file handle of " << file_path << ": " << file_handle << std::endl;
    }

    STARTING_VCN_INPUT_BUFFER vcn_input{};
    vcn_input.StartingVcn.QuadPart = 0;

    retrieval_pointers = std::vector<unsigned char>(InitialPointersBufferSize);

    DWORD bytes_returned = 0;
    DWORD errorcode = 0;
    BOOL status_deviceiocontrol = FALSE;

    do {
        status_deviceiocontrol = DeviceIoControl(
            file_handle,
            FSCTL_GET_RETRIEVAL_POINTERS,
            &vcn_input,
            sizeof vcn_input,
            retrieval_pointers.data(),
            retrieval_pointers.size(),
            &bytes_returned,
            NULL
        );
        errorcode = GetLastError();
        if (errorcode == ERROR_MORE_DATA || errorcode == ERROR_INSUFFICIENT_BUFFER) {
            retrieval_pointers.resize(retrieval_pointers.size() * 2);
        }
        else if (errorcode != NO_ERROR) {
            throw std::runtime_error("[FileClusterDistribution] FSCTL_GET_RETRIEVAL_POINTERS failed with error code: " + errorcode);
        }
        std::cout << "Returned bytes of FSCTL_GET_RETRIEVAL_POINTERS: " << bytes_returned << std::endl;
        std::cout << "Status of FSCTL_GET_RETRIEVAL_POINTERS: " << status_deviceiocontrol << std::endl;
        std::cout << "Error Code of FSCTL_GET_RETRIEVAL_POINTERS: " << errorcode << std::endl;
    } while (status_deviceiocontrol != TRUE);

    for (unsigned int i = 0; i < bytes_returned; i++) {
        printf_s("%02X ", retrieval_pointers[i]);
    }
    putchar('\n');

    // Verification of casting into RETRIEVAL_POINTERS_BUFFER
    RETRIEVAL_POINTERS_BUFFER* retbuf = (RETRIEVAL_POINTERS_BUFFER*)retrieval_pointers.data();

    printf_s("Extent Count=%d, Starting VCN=%lld\n", retbuf->ExtentCount, retbuf->StartingVcn.QuadPart);
    for (unsigned int i = 0; i < retbuf->ExtentCount; i++) {
        printf_s("Next VCN=0x%llX LCN=%llX\n", retbuf->Extents[i].NextVcn.QuadPart, retbuf->Extents[i].Lcn.QuadPart);
    }

    CloseHandle(file_handle);


}

std::vector<ClusterFlagment> FileClusterDistribution::getDistribution()
{
	return std::vector<ClusterFlagment>();
}


RETRIEVAL_POINTERS_BUFFER FileClusterDistribution::getRetrievalPointers() {
	return RETRIEVAL_POINTERS_BUFFER();
}


long FileClusterDistribution::getRetrievalPointerBase() {
	return 0l;
}


int FileClusterDistribution::getClusterSize() {
	return 0;

}



