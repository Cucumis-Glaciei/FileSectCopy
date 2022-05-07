// FileSectCopy.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <Windows.h>

int main()
{
    std::cout << "Hello World!\n";

    std::string file_path = "X:\\cucumis_glaciei.png";

    HANDLE volume_handle = CreateFileA(
        file_path.c_str(),
        0x00,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (volume_handle == INVALID_HANDLE_VALUE) {
        std::cout << "Failed to obtain file handle of " << file_path << std::endl;
        return -1;
    }
    else {
        std::cout << "Obtained file handle of " << file_path << ": " << volume_handle << std::endl;
    }

    char inbuf[64]{};
    char outbuf[512]{};
    DWORD bytes_returned = 0;


    BOOL status = DeviceIoControl(
        volume_handle,
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

    for (int i = 0; i < bytes_returned; i++) {
        printf("%02X ", outbuf[i]);
    }

    CloseHandle(volume_handle);
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
