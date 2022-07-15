// FileSectCopy.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <Windows.h>
#include <vector>
#include <filesystem>
#include <locale>
#include <tchar.h>

#include "FileClusterDistribution.h"

int wmain(int argc, TCHAR** argv)
{
	if (argc < 2) {
		wprintf(L"Usage: FileSectCopy file\n");
		return -1;
	}

	FileClusterDistribution* clusterdistrib;

	try {
		clusterdistrib = new FileClusterDistribution(argv[1]);
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
		return -1;
	}

	puts("-------------------------------------------");
	// PoC for FSCTL_GET_RETRIEVAL_POINTER_BASE 
	char drive_letter = clusterdistrib->getDriveLetter();
	TCHAR volume_path[32];
	_stprintf_s(volume_path, 32, _T("\\\\.\\%c:"), drive_letter);
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

	delete clusterdistrib;
	puts("-------------------------------------------");

	TCHAR rootPath[16];
	_stprintf_s(rootPath, 16, _T("%c:\\"), drive_letter);
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
		printf("Status of GetDiskFreeSpace: %d, Sectors / Cluster: %d, Bytes / Cluster: %d, Number of free clusters: %d, Total number of clusters: %d\n", diskFreeSpace_result, sectors_per_cluster, bytesPerSector, numFreeClusters, numOfClusters);
	}
	else {
		printf("Status of GetDiskFreeSpace: %d\n", diskFreeSpace_result);
	}

	return 0;
}

int wmain_old(int argc, TCHAR** argv)
{
	if (argc < 2) {
		wprintf(L"Usage: FileSectCopy file\n");
		return -1;
	}

	std::cout << "Hello World!\n";

	TCHAR* file_path_str = argv[1];
	std::filesystem::path file_path = file_path_str;
	char drive_letter = file_path.root_name().string().at(0);
	if (!std::isalpha(drive_letter)) {
		std::cout << "Target File: " << file_path.string() << " does not have Drive Letter" << std::endl;
		return -1;
	}
	std::cout << "Target File: " << file_path.string() << "\nDrive Letter: " << drive_letter << std::endl;


	HANDLE file_handle = CreateFile(
		file_path_str,
		0x00,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	if (file_handle == INVALID_HANDLE_VALUE) {
		std::cout << "Failed to obtain file handle of " << file_path_str << std::endl;
		return -1;
	}
	else {
		std::cout << "Obtained file handle of " << file_path_str << ": " << file_handle << std::endl;
	}

	STARTING_VCN_INPUT_BUFFER vcn_input{};
	vcn_input.StartingVcn.QuadPart = 0;
	std::vector<unsigned char> outvec(128);
	DWORD bytes_returned = 0;
	DWORD errorcode = 0;
	BOOL status_deviceiocontrol = FALSE;

	do {
		status_deviceiocontrol = DeviceIoControl(
			file_handle,
			FSCTL_GET_RETRIEVAL_POINTERS,
			&vcn_input,
			sizeof vcn_input,
			outvec.data(),
			outvec.size(),
			&bytes_returned,
			NULL
		);
		errorcode = GetLastError();
		if (errorcode == ERROR_MORE_DATA || errorcode == ERROR_INSUFFICIENT_BUFFER) {
			outvec.resize(outvec.size() * 2);
		}
		else if (errorcode != NO_ERROR) {
			return -1;
		}
		std::cout << "Returned bytes of FSCTL_GET_RETRIEVAL_POINTERS: " << bytes_returned << std::endl;
		std::cout << "Status of FSCTL_GET_RETRIEVAL_POINTERS: " << status_deviceiocontrol << std::endl;
		std::cout << "Error Code of FSCTL_GET_RETRIEVAL_POINTERS: " << errorcode << std::endl;
	} while (status_deviceiocontrol != TRUE);

	for (unsigned int i = 0; i < bytes_returned; i++) {
		printf_s("%02X ", outvec[i]);
	}
	putchar('\n');

	// Verification of casting into RETRIEVAL_POINTERS_BUFFER
	RETRIEVAL_POINTERS_BUFFER* retbuf = (RETRIEVAL_POINTERS_BUFFER*)outvec.data();

	printf_s("Extent Count=%d, Starting VCN=%lld\n", retbuf->ExtentCount, retbuf->StartingVcn.QuadPart);
	for (unsigned int i = 0; i < retbuf->ExtentCount; i++) {
		printf_s("Next VCN=0x%llX LCN=%llX\n", retbuf->Extents[i].NextVcn.QuadPart, retbuf->Extents[i].Lcn.QuadPart);
	}

	CloseHandle(file_handle);
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
