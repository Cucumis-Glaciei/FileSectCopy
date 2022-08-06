#include "FileClusterDistribution.h"

#include <string>
#include <Windows.h>
#include <iostream>
#include <filesystem>
#include <tchar.h>
#include <shlwapi.h>
#pragma comment( lib, "Shlwapi.lib" ) 

FileClusterDistribution::FileClusterDistribution(TCHAR* path) {

	if (path == nullptr) {
		throw std::runtime_error("[FileClusterDistribution] Target file path is null pointer.");
	}

	// std::filesystem::path could be made from TCHAR[]
	file_path = path;

	// Get File Handle for the path
	HANDLE file_handle = CreateFile(
		path,
		0x00,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (file_handle == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Failed to obtain file handle of " + file_path.string());
	}
	else {
		std::cout << "[FileClusterDistribution] Obtained file handle of " << file_path << ": " << file_handle << std::endl;
	}

	// Obtain the drive letter relevant to the file specified by "path"
	int drive_num = PathGetDriveNumber(path);
	file_driveletter = drive_num == -1 ? ' ' : (TCHAR)('A' + drive_num);

	// Check if the path has a correct drive letter
	if (!std::isalpha(file_driveletter)) {
		std::cout << "Target File: " << file_path.string() << " does not have Drive Letter" << std::endl;
		throw std::runtime_error("[FileClusterDistribution] Target File: " + file_path.string() + " does not have Drive Letter");
	}
	std::cout << "[FileClusterDistribution] Target File: " << file_path.string() << std::endl;
	_tprintf_s(_T("Drive Letter: %c\n"), file_driveletter);

	// Input buffer for FSCTL_GET_RETRIEVAL_POINTERS is starting virtual cluster number described by STARTING_VCN_INPUT_BUFFER structure
	STARTING_VCN_INPUT_BUFFER vcn_input{};
	vcn_input.StartingVcn.QuadPart = 0;

	retrieval_pointers = std::vector<unsigned char>(InitialPointersBufferSize);

	DWORD bytes_returned = 0;
	DWORD errorcode = 0;
	BOOL status_deviceiocontrol = FALSE;

	// Call DeviceIoControl with FSCTL_GET_RETRIEVAL_POINTERS control code
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
		// DeviceIoControl Does NOT return error code
		errorcode = GetLastError();
		// ERROR_MORE_DATA will be set if the target file is extremely fragmented & thus could not be fit to the output buffer
		if (errorcode == ERROR_MORE_DATA || errorcode == ERROR_INSUFFICIENT_BUFFER) {
			// retry calling DeviceIoControl with doubled output buffer size
			retrieval_pointers.resize(retrieval_pointers.size() * 2);
		}
		else if (errorcode != NO_ERROR) {
			throw std::runtime_error("[FileClusterDistribution] FSCTL_GET_RETRIEVAL_POINTERS failed with error code: " + errorcode);
		}
		std::cout << "Returned bytes of FSCTL_GET_RETRIEVAL_POINTERS: " << bytes_returned << std::endl;
		std::cout << "Status of FSCTL_GET_RETRIEVAL_POINTERS: " << status_deviceiocontrol << std::endl;
		std::cout << "Error Code of FSCTL_GET_RETRIEVAL_POINTERS: " << errorcode << std::endl;
	} while (status_deviceiocontrol != TRUE); // Return value TRUE indicates the success of the DeviceIoControl API

	for (unsigned int i = 0; i < bytes_returned; i++) {
		printf_s("%02X ", retrieval_pointers[i]);
	}
	putchar('\n');

	// casting char vector into RETRIEVAL_POINTERS_BUFFER
	RETRIEVAL_POINTERS_BUFFER* retbuf = (RETRIEVAL_POINTERS_BUFFER*)retrieval_pointers.data();

	// Parse the RETRIEVAL_POINTERS_BUFFER structure
	printf_s("Extent Count=%d, Starting VCN=%lld\n", retbuf->ExtentCount, retbuf->StartingVcn.QuadPart);
	for (unsigned int i = 0; i < retbuf->ExtentCount; i++) {
		printf_s("Next VCN=0x%llX LCN=%llX\n", retbuf->Extents[i].NextVcn.QuadPart, retbuf->Extents[i].Lcn.QuadPart);
	}

	// Obtain file size
	LARGE_INTEGER* fileSize = new LARGE_INTEGER();
	BOOL result_getFileSizeEx = GetFileSizeEx(file_handle, fileSize);
	if (result_getFileSizeEx != FALSE) {
		file_size = fileSize->QuadPart;
		_tprintf_s(_T("File Size: %lld\n"), file_size);
	}
	delete fileSize;
	// File handle must be closed after use
	CloseHandle(file_handle);

	getBytesPerCluster();
}

std::vector<ClusterFragment> FileClusterDistribution::getDistribution()
{
	std::vector<ClusterFragment> cluster_fragment;
	RETRIEVAL_POINTERS_BUFFER* retbuf = (RETRIEVAL_POINTERS_BUFFER*)retrieval_pointers.data();
	LONGLONG prev_start_vcn = 0;

	for (unsigned int i = 0; i < retbuf->ExtentCount; i++) {
		LONGLONG fragment_length = retbuf->Extents[i].NextVcn.QuadPart - prev_start_vcn;
		prev_start_vcn = retbuf->Extents[i].NextVcn.QuadPart;
		cluster_fragment.push_back(ClusterFragment(retbuf->Extents[i].Lcn.QuadPart, fragment_length));
	}

	return cluster_fragment;
}

TCHAR FileClusterDistribution::getDriveLetter()
{
	return file_driveletter;
}


RETRIEVAL_POINTERS_BUFFER FileClusterDistribution::getRetrievalPointers() {
	return RETRIEVAL_POINTERS_BUFFER();
}


LONGLONG FileClusterDistribution::getRetrievalPointerBase() {
	TCHAR volume_path[32];
	_stprintf_s(volume_path, 32, _T("\\\\.\\%c:"), file_driveletter);
	//{ '\\' , '\\', '.', '\\', drive_letter, ':', (char)0 };

	printf("Drive Letter: %ls\n", volume_path);

	// Get File Handle for the volume
	HANDLE volume_handle = CreateFile(
		volume_path,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (volume_handle == INVALID_HANDLE_VALUE) {
		return -1;
	}
	printf("%lld\n", (long long)volume_handle);


	LARGE_INTEGER retrieval_pointer_base{};
	DWORD returned_bytes;
	BOOL status_deviceiocontrol = DeviceIoControl(
		volume_handle,
		FSCTL_GET_RETRIEVAL_POINTER_BASE,
		(LPVOID)NULL, // No need for input buffer
		(DWORD)0,
		&retrieval_pointer_base,
		sizeof(LARGE_INTEGER),
		&returned_bytes,
		NULL
	);

	printf("Status ofDeviceIoControl: %d, Returned Bytes: %d, Retrieval Pointers Base: %lld\n",
		status_deviceiocontrol,
		returned_bytes,
		retrieval_pointer_base.QuadPart
	);

	CloseHandle(volume_handle);

	return retrieval_pointer_base.QuadPart;
}


void FileClusterDistribution::getBytesPerCluster() {
	TCHAR rootPath[16];
	_stprintf_s(rootPath, 16, _T("%c:\\"), file_driveletter);
	//{drive_letter, ':', '\\', (char)0};
	DWORD sectors_per_cluster = 0;
	DWORD bytesPerSector = 0;
	DWORD numFreeClusters = 0;
	DWORD numOfClusters = 0;

	BOOL diskFreeSpace_result;

	diskFreeSpace_result = GetDiskFreeSpace(
		rootPath,
		&sectors_per_cluster,
		&bytesPerSector,
		&numFreeClusters,
		&numOfClusters
	);
	if (diskFreeSpace_result == TRUE) {
		printf("Status of GetDiskFreeSpace: %d, Sectors / Cluster: %d, Bytes / Sector: %d, Number of free clusters: %d, Total number of clusters: %d\n", diskFreeSpace_result, sectors_per_cluster, bytesPerSector, numFreeClusters, numOfClusters);
	}
	else {
		printf("Status of GetDiskFreeSpace: %d\n", diskFreeSpace_result);
	}

	this->bytes_per_cluster = sectors_per_cluster * bytesPerSector;
	this->bytes_per_sector = bytesPerSector;
}



