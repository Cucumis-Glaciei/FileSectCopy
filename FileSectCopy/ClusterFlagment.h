#pragma once

/// <summary>
/// A structure to describe the location of file fragments on a volume, with the starting offsets of the continuous logical clusters and their quantity
/// 
/// �{�����[����ɑ��݂���t�@�C���f�Ђ̏��݂��A�_���N���X�^(Logical Cluster)�Ƃ��Ă̊J�n�ʒu����јA������N���X�^����p���ċL�q���邽�߂̍\���̂ł���B
/// </summary>
struct ClusterFlagment
{
public:
	unsigned long startClusterIndex;
	unsigned long clusterNumber;

	ClusterFlagment(unsigned long startClusterIndex, unsigned long clusterNumber);
};

