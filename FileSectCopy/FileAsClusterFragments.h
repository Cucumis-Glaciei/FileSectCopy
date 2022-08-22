#pragma once
#include <Windows.h>
#include <filesystem>
#include <atlstr.h>

/// <summary>
/// A structure to describe the location of file fragments on a volume, with the starting offsets of the continuous logical clusters and their quantity
/// ボリューム上に存在するファイル断片の所在を、論理クラスタ(Logical Cluster)としての開始位置および連続するクラスタ数を用いて記述するための構造体である。
/// </summary>
struct ClusterFragment
{
public:
	/// <summary>
	/// The offset (bytes) of the beginning of this file fragment on the volume 
	/// ファイル断片の開始点のボリューム上でのオフセット(バイト)
	/// </summary>
	LONGLONG startOffset;
	/// <summary>
	/// The length of this file fragment (bytes)
	/// ファイル断片の長さ(バイト)
	/// </summary>
	LONGLONG fragmentLength;

	/// <summary>
	/// The constructor of a file fragment
	/// </summary>
	/// <param name="startOffset"> The offset (bytes) of the beginning of this file fragment on the volume </param>
	/// <param name="fragmentLength"> The length of this file fragment (bytes) </param>
	ClusterFragment(LONGLONG startOffset, LONGLONG fragmentLength);
};

class FileAsClusterFragments
{
private:
	/// <summary>
	/// Total file size (bytes): even if this value is not a multiple of sector size of the volume, the data after the EOF offset will be ignored.
	/// ファイルの合計サイズ(バイト)。この値がボリュームのセクタ境界に一致しない場合には、EOF以後のデータを無視する
	/// </summary>
	LONGLONG totalFileSize;

	/// <summary>
	/// The volume device path containing the cluster fragments: e.g. "\\.\C:" "\\?\Harddisk0Partition2"
	/// クラスタ断片を含むボリュームへのパス：例えば"\\.\C:" "\\?\Harddisk0Partition2"など
	/// </summary>
	CString volume_device_path_str;

public:
	/// <summary>
	/// The vector object of cluster fragments constituting the file
	/// ファイルを構成するクラスタ断片からなるvectorオブジェクト
	/// </summary>
	std::vector<ClusterFragment> cluster_fragments;

	/// <summary>
	/// The constructor to specify the total file size & volume device path
	/// ファイルの合計サイズとボリュームへのパスを指定するコンストラクタ
	/// </summary>
	/// <param name="total_file_size"> Total size (bytes) for the file </param>
	/// <param name="volume_device_path"> Device file path of the volume in which the file is contained </param>
	FileAsClusterFragments(LONGLONG total_file_size, CString volume_device_path, std::vector<ClusterFragment> cluster_fragments);

	/// <summary>
	/// Extract the data in the cluster fragments possessed by this object, and output to another file: 
	/// このオブジェクトが保持するクラスタ断片からなるデータを抽出してほかのファイルへと出力する
	/// </summary>
	/// <param name="out_file_path"> The file path to extract the data </param>
	/// <returns> The size of the actually extracted data (bytes) </returns>
	LONGLONG ExtractToFile(CString out_file_path);

	// Reserved
	// std::vector<unsigned char> ExtractToMem();
};
