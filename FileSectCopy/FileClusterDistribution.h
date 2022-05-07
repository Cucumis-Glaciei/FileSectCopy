#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include "ClusterFlagment.h"

/// <summary>
/// ファイルがボリューム上で占有しているクラスタの一覧を取得・パースするためのクラス。
/// </summary>
class FileClusterDistribution
{
private:
	std::string file_path;
	char volume_of_file;
	HANDLE file_handle;


	/// <summary>
	/// FSCTL_GET_RETRIEVAL_POINTERS で DeviceIoControl API を呼び出し、RETRIEVAL_POINTERS_BUFFER 型の結果を取得する。
	/// </summary>
	/// <returns></returns>
	RETRIEVAL_POINTERS_BUFFER getRetrievalPointers();

	/// <summary>
	/// FSCTL_GET_RETRIEVAL_POINTER_BASE でDeviceIoControl API を呼び出し、ロジカルクラスタ番号のオフセットとなるセクタ番号を取得する。 
	/// </summary>
	/// <returns></returns>
	long getRetrievalPointerBase();

	/// <summary>
	/// ファイルが格納されているボリュームのクラスタサイズを取得する。
	/// </summary>
	/// <returns></returns>
	int getClusterSize();

public:
/// <summary>
/// ファイルパス path で示されるファイルを対象としてコンストラクトする。
/// </summary>
	FileClusterDistribution(std::string path);

	/// <summary>
	/// コンストラクト時に与えられたパスで記述されるファイルに対して、その論理ボリューム上でのクラスタ分布を取得する。
	/// </summary>
	/// <returns>
	/// ClusterFlagment構造体のベクターで記述された、ファイルのクラスタ分布
	/// </returns>
	std::vector<ClusterFlagment> getDistribution();

};

