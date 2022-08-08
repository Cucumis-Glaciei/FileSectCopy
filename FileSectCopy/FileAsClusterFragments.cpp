#include "FileAsClusterFragments.h"
#include <Windows.h>
#include <tchar.h>

ClusterFragment::ClusterFragment(LONGLONG startClusterIndex = 0, LONGLONG fragmentLength = 0)
{
	this->startClusterIndex = startClusterIndex;
	this->fragmentLength = fragmentLength;
}

FileAsClusterFragments::FileAsClusterFragments(LONGLONG total_file_size, TCHAR* volume_device_path)
{
	this->totalFileSize = total_file_size;
	this->volumeDevicePath = volume_device_path;

}

LONGLONG FileAsClusterFragments::ExtractToFile(TCHAR* out_file_path)
{
	TCHAR volume_path[32];
	_stprintf_s(volume_path, 32, _T("\\\\.\\%s:"), this->volumeDevicePath.c_str());

	// Get File Handle for the volume
	HANDLE volume_handle = CreateFile(
		_T("\\\\.\\X:"),
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
	LARGE_INTEGER offset{};
	offset.QuadPart = 256;
	LARGE_INTEGER file_pointer;
	BOOL result_SetFilePointerEx = SetFilePointer(
		volume_handle,
		512,
		0,
		FILE_BEGIN
	);
	if (result_SetFilePointerEx == FALSE) {
		_tprintf_s(_T("SetFilePointerEx Fail"));
		return -1;
	}
	std::vector<unsigned char> readbuf = std::vector<unsigned char>(512);
	DWORD bytes_read;
	BOOL result_readfile = ReadFile(
		volume_handle,
		readbuf.data(),
		512,
		&bytes_read,
		NULL
	);
	if (result_readfile == FALSE) {
		DWORD err = GetLastError();
		_tprintf_s(_T("ReadFile Fail %d\n"), err);
	}
	for (int i = 0; i < 256; i++) {
		_tprintf_s(_T("%02X "), readbuf[i]);
	}
	return LONGLONG();
}
