#include "FileAsClusterFragments.h"
#include <Windows.h>
#include <tchar.h>
#include <atlstr.h>
#include <iostream>
#include <fstream>

ClusterFragment::ClusterFragment(LONGLONG startClusterIndex = 0, LONGLONG fragmentLength = 0)
{
	this->startOffset = startClusterIndex;
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

	// Get File Handle for the volume
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
		throw std::runtime_error("[FileAsClusterFragments] Failed to obtain volume handle.");
	}

	LONGLONG bytes_written = 0;
	std::ofstream out_file(out_file_path, std::ios::binary);
	for (ClusterFragment frag : cluster_fragments) {
		_tprintf_s(_T("[FileAsClusterFragments] Length(Bytes): 0x%llX\t Offset: 0x%llX\n"),
			frag.fragmentLength,
			frag.startOffset
		);

		LARGE_INTEGER offset{};
		offset.QuadPart = frag.startOffset;
		BOOL result_SetFilePointerEx = SetFilePointerEx( // Seek to the beginning of the file fragment: this API successes only when the offset is aligned at sector boundary.
			volume_handle,
			offset,
			NULL,
			FILE_BEGIN
		);
		if (result_SetFilePointerEx == FALSE) {
			_tprintf_s(_T("[FileAsClusterFragments] SetFilePointerEx Fail"));
			return -1;
		}

		std::vector<unsigned char> readbuf = std::vector<unsigned char>(frag.fragmentLength);
		DWORD bytes_read;
		BOOL result_readfile = ReadFile( // this API seccesses only when the number of bytes is multiply of sector size
			volume_handle,
			readbuf.data(),
			readbuf.size(),
			&bytes_read,
			NULL
		);
		if (result_readfile == FALSE) {
			DWORD err = GetLastError();
			_tprintf_s(_T("[FileAsClusterFragments] ReadFile Fail %d\n"), err);
		}
		
		//  Do not copy data after EOF
		LONGLONG write_length = readbuf.size() < (totalFileSize - bytes_written) ? readbuf.size() : (totalFileSize - bytes_written);
		out_file.write((char*)readbuf.data(), write_length);
		bytes_written += write_length;
		/*for (int i = 0; i < readbuf.size() && bytes_written < clusterdistrib->file_size; i++,bytes_written++) {
			_tprintf_s(_T("%02X "), readbuf[i]);
			if (i % 16 == 15) {
				putchar('\n');
			}
		}*/

	}
	CloseHandle(volume_handle);
	out_file.close();

	return bytes_written;
}
