// FileSectCopy.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <Windows.h>
#include <vector>
#include <filesystem>
#include <locale>

#include "FileClusterDistribution.h"

int wmain(int argc, TCHAR** argv)
{
    if (argc < 2) {
        wprintf(L"Usage: FileSectCopy file\n");
        return -1;
    }

    try {
        FileClusterDistribution FileClusterDistribution(argv[1]);
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
        return -1;
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

    TCHAR* file_path_str =  argv[1];
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
