#pragma once
#include <Windows.h>
#include <filesystem>

/// <summary>
/// A structure to describe the location of file fragments on a volume, with the starting offsets of the continuous logical clusters and their quantity
/// 
/// �{�����[����ɑ��݂���t�@�C���f�Ђ̏��݂��A�_���N���X�^(Logical Cluster)�Ƃ��Ă̊J�n�ʒu����јA������N���X�^����p���ċL�q���邽�߂̍\���̂ł���B
/// </summary>
struct ClusterFragment
{
public:
	/// <summary>
	/// �t�@�C���f�Ђ̊J�n�_�̃{�����[����ł̃I�t�Z�b�g(�N���X�^��)
	/// </summary>
	LONGLONG startClusterIndex;
	/// <summary>
	/// �t�@�C���f�Ђ̒���
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
	std::filesystem::path volumeDevicePath;

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
	FileAsClusterFragments(LONGLONG total_file_size, TCHAR* volume_device_path);

	/// <summary>
	/// Extract the data of the file to another file: 
	/// </summary>
	/// <param name="out_file_path"> The file path to extract the data </param>
	/// <returns> The size of the actually extracted data (bytes) </returns>
	LONGLONG ExtractToFile(TCHAR* out_file_path);

	// Reserved
	// std::vector<unsigned char> ExtractToMem();
};