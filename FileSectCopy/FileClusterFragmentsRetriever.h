#pragma once
#include <string>
#include <vector>
#include <Windows.h>
#include "FileAsClusterFragments.h"
#include <atlstr.h>

/// <summary>
/// <para> A class to obtain and parse the list of the clusters on the volume associated to a file </para>
/// <para> ファイルがボリューム上で占有しているクラスタの一覧を取得・パースするためのクラス。 </para>
/// </summary>
class FileClusterDistribution
{
private:
	/// <summary>
	/// The class for storing the information of the volume about the clusters
	/// </summary>
	class VolumeClusterInfo
	{
	private:
		/// <summary> The file path included by the target volume, this value is used to specify the volume </summary>
		CString file_path_str_;
		/// <summary> Cluster size (bytes) </summary>
		LONGLONG bytes_per_cluster_;
		/// <summary> Sector size (bytes) </summary>
		LONGLONG bytes_per_sector_;
		/// <summary> Starting offset for retrieval_pointers, see https://docs.microsoft.com/en-us/windows/win32/api/winioctl/ns-winioctl-retrieval_pointer_base </summary>
		LONGLONG retrieval_pointers_offset_;

	public:
		/// <summary> The volume device path, e.g. "\\.\C:" </summary>
		CString volume_device_path_str_;
		/// <summary> The constructor specifying the file path included by the target volume </summary>
		VolumeClusterInfo(CString file_path_str = "");
		/// <summary> Obtain cluster size (bytes) </summary>
		LONGLONG GetBytesPerCluster() { return bytes_per_cluster_; };
		/// <summary> Obtain sector size (bytes) </summary>
		LONGLONG GetBytesPerSector() { return bytes_per_sector_; };
		/// <summary> Obtain starting offset for retrieval_pointers </summary>
		LONGLONG GetRetrievalPointersOffset() { return retrieval_pointers_offset_; };

	};

	/// <summary> RETRIEVAL_POINTERS_BUFFER size for the first try of FSCTL_GET_RETRIEVAL_POINTERS </summary>
	const int InitialPointersBufferSize = 8192;

	/// <summary> The file path to extract the data directly from the volume </summary>
	CString file_path;
	/// <summary> the buffer for the FSCTL_GET_RETRIEVAL_POINTERS </summary>
	std::vector<unsigned char> retrieval_pointers;
	/// <summary> The information of the volume about the clusters </summary>
	VolumeClusterInfo* volume_cluster_info_;

public:
	/// <summary> Actual size of the targetting file </summary>
	LONGLONG file_size = 0;

	/// <summary>
	/// <para> A constructor with the file path specifying a target file </para>
	/// <para> ファイルパス path で示されるファイルを対象としてコンストラクトする </para>
	/// </summary>
	/// <param name="path"> The file path to extract the data directly from the volume </param>
	FileClusterDistribution(CString path = "");

	/// <summary>
	/// <para> Obtain the cluster distribution on the volume containing the file, specified at the construction by the path </para>
	/// <para> コンストラクト時に与えられたパスで記述されるファイルに対して、そのファイルを格納しているボリューム上でのクラスタ分布を取得する </para>
	/// </summary>
	/// <returns>
	/// <para> The instance of the FileAsClusterFragments class posessing the cluster distribution of the targetting file </para>
	/// <para> 対象ファイルのクラスタ分布を保持する FileAsClusterFragments クラスのインスタンス </para>
	/// </returns>
	FileAsClusterFragments getDistribution();

};

