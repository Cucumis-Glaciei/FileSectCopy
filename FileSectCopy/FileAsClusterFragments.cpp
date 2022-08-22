#include "FileAsClusterFragments.h"
#include <Windows.h>
#include <tchar.h>
#include <atlstr.h>
#include <iostream>
#include <fstream>

ClusterFragment::ClusterFragment(LONGLONG startOffset = 0, LONGLONG fragmentLength = 0)
{
	this->startOffset = startOffset;
	this->fragmentLength = fragmentLength;
}

FileAsClusterFragments::FileAsClusterFragments(LONGLONG total_file_size, CString volume_device_path, std::vector<ClusterFragment> cluster_fragments)
{
	this->totalFileSize = total_file_size;
	this->volume_device_path_str = volume_device_path;
	this->cluster_fragments = cluster_fragments;
}

LONGLONG FileAsClusterFragments::ExtractToFile(CString out_file_path)
{
	_putts(_T("-------------------------------------------"));

	// Get File Handle of the volume containing the cluster fragments
	HANDLE volume_handle = CreateFile(
		(LPCTSTR)this->volume_device_path_str,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (volume_handle == INVALID_HANDLE_VALUE) {
		DWORD err = GetLastError();
		_tprintf_s(_T("[FileAsClusterFragments] Failed to obtain volume handle: %s; Err code %d\n"), (LPCTSTR)volume_device_path_str, err);
		throw std::runtime_error("[FileAsClusterFragments] Failed to obtain the volume handle.");
	}

	LONGLONG bytes_written = 0;		// written data size to the out_file_path
	std::ofstream out_file(out_file_path, std::ios::binary);	// file stream for data output

	for (ClusterFragment frag : cluster_fragments) {	// Loop for each cluster fragment in order to copy the data to file
		_tprintf_s(_T("[FileAsClusterFragments] Length(Bytes): 0x%llX\t Offset: 0x%llX\n"),
			frag.fragmentLength,
			frag.startOffset
		);

		LARGE_INTEGER offset{};		// SetFilePointerEx does not accept LONGLONG but LARGE_INTEGER union
		offset.QuadPart = frag.startOffset;
		BOOL result_SetFilePointerEx = SetFilePointerEx( // Seek to the beginning of the file fragment: this API successes only when the offset is aligned at sector boundary.
			volume_handle,
			offset,
			NULL,
			FILE_BEGIN
		);
		if (result_SetFilePointerEx == FALSE) {
			_tprintf_s(_T("[FileAsClusterFragments] SetFilePointerEx Fail"));
			throw std::runtime_error("[FileAsClusterFragments] SetFilePointerEx Fail");
		}

		std::vector<unsigned char> readbuf = std::vector<unsigned char>(frag.fragmentLength);
		DWORD bytes_read;
		BOOL result_readfile = ReadFile( // this API seccesses only when the number of bytes is multiply of sector size if the file handle is related to the physical drive or volume
			volume_handle,
			readbuf.data(),
			readbuf.size(),
			&bytes_read,	// lpNumberOfBytesRead argument is not allowed to set NULL 
			NULL
		);
		if (result_readfile == FALSE) {
			DWORD err = GetLastError();
			_tprintf_s(_T("[FileAsClusterFragments] ReadFile Fail %d\n"), err);
			throw std::runtime_error("[FileAsClusterFragments] ReadFile Fail ");
		}
		
		//  Do not copy data after EOF
		LONGLONG write_length = readbuf.size() < (totalFileSize - bytes_written) ? readbuf.size() : (totalFileSize - bytes_written);
		out_file.write((char*)readbuf.data(), write_length);
		bytes_written += write_length;

	}

	// Close dynamic resources
	CloseHandle(volume_handle);
	out_file.close();

	return bytes_written;
}
