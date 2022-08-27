#include "FileClusterFragmentsRetriever.h"

#include <string>
#include <Windows.h>
#include <iostream>
#include <tchar.h>
#include <atlstr.h>
#include <shlwapi.h>
#pragma comment( lib, "Shlwapi.lib" ) 

FileClusterDistribution::FileClusterDistribution(CString path) {

	if (path == "") {
		throw std::runtime_error("[FileClusterDistribution] Source file path is empty.");
	}

	_tprintf_s(_T("[FileClusterDistribution] Source File: %s\n"), path.GetBuffer());
	// copy the path of the source file 
	file_path = path;

	this->volume_cluster_info_ = new VolumeClusterInfo(path); // Extract information of the volume containing the file specified by the path

	// Get File Handle for the path
	HANDLE file_handle = CreateFile(
		path,
		0x00, // No access is required to call FSCTL_GET_RETRIEVAL_POINTERS
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (file_handle == INVALID_HANDLE_VALUE) {
		throw std::runtime_error("Failed to obtain the source file handle.");
	}
	_tprintf_s(_T("[FileClusterDistribution] Obtained file handle of %s: %p\n"), (LPCTSTR)file_path, file_handle);


	// Input buffer for FSCTL_GET_RETRIEVAL_POINTERS is starting virtual cluster number described by STARTING_VCN_INPUT_BUFFER structure
	STARTING_VCN_INPUT_BUFFER vcn_input{};
	vcn_input.StartingVcn.QuadPart = 0;

	retrieval_pointers = std::vector<unsigned char>(InitialPointersBufferSize);	// output buffer for DeviceIoControl

	DWORD bytes_returned = 0;
	BOOL status_deviceiocontrol = FALSE;

	// Call DeviceIoControl with FSCTL_GET_RETRIEVAL_POINTERS control code
	do {
		DWORD error_code = 0; // result of GetLastError

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
		error_code = GetLastError();
		// ERROR_MORE_DATA will be set if the source file is extremely fragmented & the information of retrieval pointers could not fit to the output buffer
		if (error_code == ERROR_MORE_DATA || error_code == ERROR_INSUFFICIENT_BUFFER) {
			// retry calling DeviceIoControl with doubled output buffer size
			retrieval_pointers.resize(retrieval_pointers.size() * 2);
		}
		else if (error_code != NO_ERROR) {	// Other errors could not be handled 
			CloseHandle(file_handle);
			throw std::runtime_error("[FileClusterDistribution] FSCTL_GET_RETRIEVAL_POINTERS failed with error code: " + error_code);
		}
		std::cout << "Returned bytes of FSCTL_GET_RETRIEVAL_POINTERS: " << bytes_returned << std::endl;
		std::cout << "Status of FSCTL_GET_RETRIEVAL_POINTERS: " << status_deviceiocontrol << std::endl;
		std::cout << "Error Code of FSCTL_GET_RETRIEVAL_POINTERS: " << error_code << std::endl;
	} while (status_deviceiocontrol != TRUE); // Return value TRUE indicates the success of the DeviceIoControl API


	// casting the buffer from char vector into RETRIEVAL_POINTERS_BUFFER
	RETRIEVAL_POINTERS_BUFFER* retbuf = (RETRIEVAL_POINTERS_BUFFER*)retrieval_pointers.data();

	// Parse the RETRIEVAL_POINTERS_BUFFER structure
	_tprintf_s(_T("Extent Count=%d, Starting VCN=%lld\n"), retbuf->ExtentCount, retbuf->StartingVcn.QuadPart);
	for (unsigned int i = 0; i < retbuf->ExtentCount; i++) {
		_tprintf_s(_T("Next VCN=0x%llX LCN=%llX\n"), retbuf->Extents[i].NextVcn.QuadPart, retbuf->Extents[i].Lcn.QuadPart);
	}

	// Obtain file size
	LARGE_INTEGER fileSize = LARGE_INTEGER();
	BOOL result_getFileSizeEx = GetFileSizeEx(file_handle, &fileSize);
	if (result_getFileSizeEx != FALSE) {
		file_size = fileSize.QuadPart;
		_tprintf_s(_T("File Size: %lld\n"), file_size);
	}
	else {
		CloseHandle(file_handle);
		throw std::runtime_error("[FileClusterDistribution] Could not obtain the size of the source file.");
	}

	// File handle must be closed after use
	CloseHandle(file_handle);

}

FileAsClusterFragments FileClusterDistribution::getDistribution()
{
	std::vector<ClusterFragment> cluster_fragment;
	RETRIEVAL_POINTERS_BUFFER* retbuf = (RETRIEVAL_POINTERS_BUFFER*)retrieval_pointers.data();
	LONGLONG prev_start_vcn = 0;

	for (unsigned int i = 0; i < retbuf->ExtentCount; i++) {
		LONGLONG fragment_length = retbuf->Extents[i].NextVcn.QuadPart - prev_start_vcn; // Length of the fragment (clusters)
		prev_start_vcn = retbuf->Extents[i].NextVcn.QuadPart;	// Update the previous member of the Extents[]

		cluster_fragment.push_back(
			ClusterFragment(
				(retbuf->Extents[i].Lcn.QuadPart * this->volume_cluster_info_->GetBytesPerCluster()) + this->volume_cluster_info_->GetRetrievalPointersOffset(),
				fragment_length * (this->volume_cluster_info_->GetBytesPerCluster())
			)
		);		// Add cluster_fragment object
	}

	FileAsClusterFragments file_cluster_fragments = FileAsClusterFragments(
		this->file_size,
		this->volume_cluster_info_->volume_device_path_str_,
		cluster_fragment
	);

	return file_cluster_fragments;
}

FileClusterDistribution::VolumeClusterInfo::VolumeClusterInfo(CString file_path_str)
{
	this->file_path_str_ = file_path_str;
	// Obtain the volume device file related to the file specified by "file_path_str"
	CString volume_mount_point    = CString('\0', MAX_PATH + 1);
	LPTSTR volume_mount_point_buf = volume_mount_point.GetBuffer();
	this->volume_device_path_str_ = CString('\0', MAX_PATH + 1);
	LPTSTR volume_device_path_str_buffer = volume_device_path_str_.GetBuffer();

	if (GetVolumePathName( // Get the NTFS mount point from the source file path
			(LPCTSTR)file_path_str, 
			volume_mount_point_buf, 
			volume_mount_point.GetLength()) == FALSE) {
		volume_mount_point.ReleaseBuffer();
		throw std::runtime_error("[VolumeClusterInfo] Could not obtain the mount point of the volume containing the source file");
	}
	volume_mount_point.ReleaseBuffer();

	if (GetVolumeNameForVolumeMountPoint( // Get volume device file with GUID from a NTFS mount point
			(LPCTSTR)volume_mount_point,
			volume_device_path_str_buffer,
			this->volume_device_path_str_.GetLength()) == FALSE) {
		this->volume_device_path_str_.ReleaseBuffer();
		throw std::runtime_error("[VolumeClusterInfo] Could not obtain the device file of the volume containing the source file");
	}
	this->volume_device_path_str_.ReleaseBuffer();
	this->volume_device_path_str_.Delete(this->volume_device_path_str_.GetLength() - 1, 1); // Remove trailing backslash
	
	_tprintf_s(_T("[VolumeClusterInfo] Volume device path: %s\n"), (LPCTSTR)this->volume_device_path_str_);

	// Obtain cluster & sector size for the volume
	DWORD sectors_per_cluster = 0;
	DWORD bytes_per_sector    = 0;
	DWORD num_free_clusters   = 0; // Not used
	DWORD num_of_clusters	  = 0; // Not used

	BOOL diskFreeSpace_result = GetDiskFreeSpace(
		this->volume_device_path_str_ + "\\", // This API needs trailing backslash for the directory path
		&sectors_per_cluster,
		&bytes_per_sector,
		&num_free_clusters, // Not used but could not be omitted
		&num_of_clusters    // ditto
	);
	if (diskFreeSpace_result == TRUE) {
		_tprintf_s(_T("[VolumeClusterInfo] Status of GetDiskFreeSpace: %d, Sectors / Cluster: %d, Bytes / Sector: %d, Number of free clusters: %d, Total number of clusters: %d\n"), diskFreeSpace_result, sectors_per_cluster, bytes_per_sector, num_free_clusters, num_of_clusters);
	}
	else {
		_tprintf_s(_T("[VolumeClusterInfo] Status of GetDiskFreeSpace: %d\n"), diskFreeSpace_result);
		throw std::runtime_error("[VolumeClusterInfo] GetDiskFreeSpace Failed.");
	}

	this->bytes_per_cluster_ = static_cast<LONGLONG>(sectors_per_cluster) * bytes_per_sector;
	this->bytes_per_sector_ = bytes_per_sector;

	// Obtain RetrievalPointerBase
	// Get File Handle for the volume device file
	HANDLE volume_handle = CreateFile(
		this->volume_device_path_str_,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (volume_handle == INVALID_HANDLE_VALUE) {
		CloseHandle(volume_handle);
		throw std::runtime_error("[VolumeClusterInfo] CreateFile for volume device file failed.");
	}
	_tprintf_s(_T("[VolumeClusterInfo] Obtained Volume Handle: 0x%llX\n"), (long long)volume_handle);

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

	_tprintf_s(_T("[VolumeClusterInfo] Status ofDeviceIoControl: %d, Returned Bytes: %d, Retrieval Pointers Base: %lld\n"),
		status_deviceiocontrol,
		returned_bytes,
		retrieval_pointer_base.QuadPart
	);
	if (status_deviceiocontrol == FALSE) {
		CloseHandle(volume_handle);
		throw std::runtime_error("[VolumeClusterInfo] Failed to call DeviceIoControl with FSCTL_GET_RETRIEVAL_POINTER_BASE.");
	}

	CloseHandle(volume_handle);

	this->retrieval_pointers_offset_ = retrieval_pointer_base.QuadPart * bytes_per_sector;

}
