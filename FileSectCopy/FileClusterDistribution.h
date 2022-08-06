#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include "ClusterFragment.h"
#include <filesystem>

/// <summary>
/// <para> A class to obtain and parse the list of the clusters on the volume associated to a file </para>
/// <para> ファイルがボリューム上で占有しているクラスタの一覧を取得・パースするためのクラス。 </para>
/// </summary>
class FileClusterDistribution
{
private:
	const int InitialPointersBufferSize = 8192;

	std::filesystem::path file_path;
	TCHAR file_driveletter;
	//	HANDLE file_handle;
	std::vector<unsigned char> retrieval_pointers;
	long retrieval_pointers_base = 0;

	//class RetrievalPointerBase
	//{
	//public:
	//	RetrievalPointerBase();
	//};

	//class DriveInfo 
	//{
	//public:
	//	DriveInfo();
	//};
	/// <summary>
	/// <para> A method to obtain the result of the RETRIEVAL_POINTERS_BUFFER type, calling the DeviceIoControl API with the FSCTL_GET_RETRIEVAL_POINTERS control code </para>
	/// <para> FSCTL_GET_RETRIEVAL_POINTERS 制御コード で DeviceIoControl API を呼び出し、RETRIEVAL_POINTERS_BUFFER 型の結果を取得する。 </para>
	/// </summary>
	/// <returns></returns>
	RETRIEVAL_POINTERS_BUFFER getRetrievalPointers();

	/// <summary>
/// <para> A method to obtain the cluster size of the volume containing the file. </para>
/// <para> ファイルが格納されているボリュームのクラスタサイズを取得する。 </para>
/// </summary>
/// <returns></returns>
	void getBytesPerCluster();

public:
	int bytes_per_cluster = 0;
	int bytes_per_sector = 0;
	LONGLONG file_size = 0;

	/// <summary>
	/// <para> A method to obtain the sector number used as the starting point of the logical cluster number, calling the DeviceIoControl API with the FSCTL_GET_RETRIEVAL_POINTER_BASE control code. </para>
	/// <para> That is, if the logical cluster number (LCN) in the result of FSCTL_GET_RETRIEVAL_POINTERS is zero, that file fragment starts from this sector. </para>
	/// FSCTL_GET_RETRIEVAL_POINTER_BASE でDeviceIoControl API を呼び出し、ロジカルクラスタ番号のオフセットとなるセクタ番号を取得する。 
	/// </summary>
	/// <returns></returns>
	LONGLONG getRetrievalPointerBase();


	/// <summary>
	/// <para> A constructor with the file path pointing a target file </para>
	/// <para> ファイルパス path で示されるファイルを対象としてコンストラクトする。 </para>
	/// </summary>
	FileClusterDistribution(TCHAR* path = nullptr);

	/// <summary>
	/// <para> コンストラクト時に与えられたパスで記述されるファイルに対して、その論理ボリューム上でのクラスタ分布を取得する。 <para>
	/// </summary>
	/// <returns>
	/// ClusterFlagment構造体のベクターで記述された、ファイルのクラスタ分布
	/// </returns>
	std::vector<ClusterFragment> getDistribution();

	/// <summary>
	/// <para> コンストラクト時に与えられたパスで記述されるファイルに対して、そのドライブレターを取得する。 </para>
	/// </summary>
	/// <returns></returns>
	TCHAR getDriveLetter();
};

