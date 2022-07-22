#pragma once
#include <Windows.h>

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

