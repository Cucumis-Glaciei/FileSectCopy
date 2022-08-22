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
	if (argc < 3) { // Output usage when the argument is less than 2
		_tprintf(_T("Usage: FileSectCopy infile outfile\n"));
		return -1;
	}

	if (std::filesystem::exists(argv[2])) { // Overwrite protection
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

	LONGLONG bytes_written = 0; // Actual written data size
	try {
		FileAsClusterFragments file_extractor = clusterdistrib->getDistribution();
		bytes_written = file_extractor.ExtractToFile(argv[2]);
	}
	catch (std::exception e) {
		std::cout << e.what() << std::endl;
		return -1;
	}
	delete clusterdistrib;

	_tprintf_s(_T("Extraction of file %s to %s completed: Copied data: %lld bytes.\n"), argv[1], argv[2], bytes_written);
	return 0;
}
