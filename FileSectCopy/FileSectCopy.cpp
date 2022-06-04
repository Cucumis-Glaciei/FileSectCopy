// FileSectCopy.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <Windows.h>

int wmain(int argc, TCHAR** argv)
{
    if (argc < 2) {
        wprintf(L"Usage: FileSectCopy file\n");
        return -1;
    }

    std::cout << "Hello World!\n";

    TCHAR* file_path =  argv[1];

    HANDLE file_handle = CreateFileW(
        file_path,
        0x00,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (file_handle == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to obtain file handle of " << file_path << std::endl;
        return -1;
    }
    else {
        std::cout << "Obtained file handle of " << file_path << ": " << file_handle << std::endl;
    }

    unsigned char inbuf[64]{};
    unsigned char outbuf[512]{};
    DWORD bytes_returned = 0;


    BOOL status = DeviceIoControl(
        file_handle,
        FSCTL_GET_RETRIEVAL_POINTERS,
        inbuf,
        64,
        outbuf,
        512,
        &bytes_returned,
        NULL
    );

    std::cout << "Returned bytes of FSCTL_GET_RETRIEVAL_POINTERS: " << bytes_returned << std::endl;
    std::cout << "Status of FSCTL_GET_RETRIEVAL_POINTERS: " << status << std::endl;

    for (unsigned int i = 0; i < bytes_returned; i++) {
        printf_s("%02X ", outbuf[i]);
    }
    putchar('\n');

    // Verification of casting into RETRIEVAL_POINTERS_BUFFER
    RETRIEVAL_POINTERS_BUFFER* retbuf = (RETRIEVAL_POINTERS_BUFFER*)outbuf;
    
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
