#pragma once
#include <Windows.h>
#include <filesystem>
#include <atlstr.h>

/// <summary>
/// A structure to describe the location of file fragments on a volume, with the starting offsets of the continuous logical clusters and their quantity
/// �{�����[����ɑ��݂���t�@�C���f�Ђ̏��݂��A�_���N���X�^(Logical Cluster)�Ƃ��Ă̊J�n�ʒu����јA������N���X�^����p���ċL�q���邽�߂̍\���̂ł���B
/// </summary>
struct ClusterFragment
{
public:
	/// <summary>
	/// The offset (bytes) of the beginning of this file fragment on the volume 
	/// �t�@�C���f�Ђ̊J�n�_�̃{�����[����ł̃I�t�Z�b�g(�o�C�g)
	/// </summary>
	LONGLONG startOffset;
	/// <summary>
	/// The length of this file fragment (bytes)
	/// �t�@�C���f�Ђ̒���(�o�C�g)
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
	/// �t�@�C���̍��v�T�C�Y(�o�C�g)�B���̒l���{�����[���̃Z�N�^���E�Ɉ�v���Ȃ��ꍇ�ɂ́AEOF�Ȍ�̃f�[�^�𖳎�����
	/// </summary>
	LONGLONG totalFileSize;

	/// <summary>
	/// The volume device path containing the cluster fragments: e.g. "\\.\C:" "\\?\Harddisk0Partition2"
	/// �N���X�^�f�Ђ��܂ރ{�����[���ւ̃p�X�F�Ⴆ��"\\.\C:" "\\?\Harddisk0Partition2"�Ȃ�
	/// </summary>
	CString volume_device_path_str;

public:
	/// <summary>
	/// The vector object of cluster fragments constituting the file
	/// �t�@�C�����\������N���X�^�f�Ђ���Ȃ�vector�I�u�W�F�N�g
	/// </summary>
	std::vector<ClusterFragment> cluster_fragments;

	/// <summary>
	/// The constructor to specify the total file size & volume device path
	/// �t�@�C���̍��v�T�C�Y�ƃ{�����[���ւ̃p�X���w�肷��R���X�g���N�^
	/// </summary>
	/// <param name="total_file_size"> Total size (bytes) for the file </param>
	/// <param name="volume_device_path"> Device file path of the volume in which the file is contained </param>
	FileAsClusterFragments(LONGLONG total_file_size, CString volume_device_path, std::vector<ClusterFragment> cluster_fragments);

	/// <summary>
	/// Extract the data in the cluster fragments possessed by this object, and output to another file: 
	/// ���̃I�u�W�F�N�g���ێ�����N���X�^�f�Ђ���Ȃ�f�[�^�𒊏o���Ăق��̃t�@�C���ւƏo�͂���
	/// </summary>
	/// <param name="out_file_path"> The file path to extract the data </param>
	/// <returns> The size of the actually extracted data (bytes) </returns>
	LONGLONG ExtractToFile(CString out_file_path);

	// Reserved
	// std::vector<unsigned char> ExtractToMem();
};
