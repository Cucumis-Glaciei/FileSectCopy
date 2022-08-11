#pragma once
#include <Windows.h>
#include <filesystem>
#include <atlstr.h>

/// <summary>
/// A structure to describe the location of file fragments on a volume, with the starting offsets of the continuous logical clusters and their quantity
/// 
/// ボリューム上に存在するファイル断片の所在を、論理クラスタ(Logical Cluster)としての開始位置および連続するクラスタ数を用いて記述するための構造体である。
/// </summary>
struct ClusterFragment
{
public:
	/// <summary>
	/// ファイル断片の開始点のボリューム上でのオフセット(クラスタ数)
	/// </summary>
	LONGLONG startOffset;
	/// <summary>
	/// ファイル断片の長さ
	/// </summary>
	LONGLONG fragmentLength;

	ClusterFragment(LONGLONG startClusterIndex, LONGLONG fragmentLength);
};

class FileAsClusterFragments
{
private:
	/// <summary>
	/// Total file size (bytes) of cluster fragments: if the file size is not a multiple of sector size of the volume, the data after this offset will be ignored.
	/// </summary>
	LONGLONG totalFileSize;

	/// <summary>
	/// The volume device path containing the cluster fragments: e.g. "\\.\C:" "\\?\Harddisk0Partition2"
	/// </summary>
	CString volume_device_path_str;

public:
	/// <summary>
	/// The cluster fragments constituting the file
	/// </summary>
	std::vector<ClusterFragment> cluster_fragments;

	/// <summary>
	/// The constructor to specify the total file size & volume device path
	/// </summary>
	/// <param name="total_file_size"> Total size (bytes) for the file </param>
	/// <param name="volume_device_path"> Device file path of the volume in which the file is contained </param>
	FileAsClusterFragments(LONGLONG total_file_size, CString volume_device_path, std::vector<ClusterFragment> cluster_fragments);

	/// <summary>
	/// Extract the data of the file to another file: 
	/// </summary>
	/// <param name="out_file_path"> The file path to extract the data </param>
	/// <returns> The size of the actually extracted data (bytes) </returns>
	LONGLONG ExtractToFile(CString out_file_path);

	// Reserved
	// std::vector<unsigned char> ExtractToMem();
};
