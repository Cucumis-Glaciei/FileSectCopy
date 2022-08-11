// FileSectCopy.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <fstream>
#include <Windows.h>
#include <vector>
#include <filesystem>
#include <locale>
#include <tchar.h>
#include <atlstr.h>

#include "FileClusterFragmentsRetriever.h"

int _tmain(int argc, TCHAR** argv)
{
	if (argc < 3) {
		_tprintf(_T("Usage: FileSectCopy infile outfile\n"));
		return -1;
	}

	if (std::filesystem::exists(argv[2])) {
		_tprintf(_T("outfile: %s already exists\n"), argv[2]);
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

	LONGLONG bytes_written = 0;
	try {
		FileAsClusterFragments file_extractor = clusterdistrib->getDistribution();
		bytes_written = file_extractor.ExtractToFile(argv[2]);
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
		return -1;
	}
	delete clusterdistrib;

	_tprintf_s(_T("Extraction of file %s to %s completed.\n"), argv[1], argv[2]);
	return 0;
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
