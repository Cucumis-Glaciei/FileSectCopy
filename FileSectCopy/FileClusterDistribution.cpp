#include "FileClusterDistribution.h"

#include <string>
#include <Windows.h>
#include <iostream>


FileClusterDistribution::FileClusterDistribution(TCHAR* path) {

    // Get File Handle for path
    file_handle = CreateFileW(
        path,
        0x00,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (file_handle == INVALID_HANDLE_VALUE) {
        throw std::runtime_error("Failed to obtain file handle of " + file_path );
    }
    else {
        std::cout << "[FileClusterDistribution] Obtained file handle of " << file_path << ": " << file_handle << std::endl;
    }
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



