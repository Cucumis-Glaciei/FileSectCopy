#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include "FileAsClusterFragments.h"
#include <filesystem>
#include <atlstr.h>

/// <summary>
/// <para> A class to obtain and parse the list of the clusters on the volume associated to a file </para>
/// <para> ファイルがボリューム上で占有しているクラスタの一覧を取得・パースするためのクラス。 </para>
/// </summary>
class FileClusterDistribution
{
private:
	class VolumeClusterInfo
	{
	private:
		CString file_path_str_;
		LONGLONG bytes_per_cluster_;
		LONGLONG bytes_per_sector_;
		LONGLONG retrieval_pointers_offset_;
	public:
		CString volume_device_path_str_;
		VolumeClusterInfo(CString file_path_str = "");
		LONGLONG GetBytesPerCluster() { return bytes_per_cluster_; };
		LONGLONG GetBytesPerSector() { return bytes_per_sector_; };
		LONGLONG GetRetrievalPointersOffset() { return retrieval_pointers_offset_; };

	};

	const int InitialPointersBufferSize = 8192;

	std::filesystem::path file_path;
	std::vector<unsigned char> retrieval_pointers;
	VolumeClusterInfo* volume_cluster_info_;

public:
	LONGLONG file_size = 0;



	/// <summary>
	/// <para> A constructor with the file path pointing a target file </para>
	/// <para> ファイルパス path で示されるファイルを対象としてコンストラクトする。 </para>
	/// </summary>
	FileClusterDistribution(CString path = "");

	/// <summary>
	/// <para> コンストラクト時に与えられたパスで記述されるファイルに対して、その論理ボリューム上でのクラスタ分布を取得する。 <para>
	/// </summary>
	/// <returns>
	/// ClusterFlagment構造体のベクターで記述された、ファイルのクラスタ分布
	/// </returns>
	FileAsClusterFragments getDistribution();

};

