#pragma once

/// <summary>
/// �f�B�X�N��ɑ��݂���t�@�C���f�Ђ��A�_���N���X�^(Logical Cluster)�Ƃ��Ă̊J�n�ʒu����јA������N���X�^����p���ċL�q���邽�߂̍\���̂ł���B
/// </summary>
struct ClusterFlagment
{
public:
	unsigned long startClusterIndex;
	unsigned long clusterNumber;

	ClusterFlagment(unsigned long startClusterIndex, unsigned long clusterNumber);
};

