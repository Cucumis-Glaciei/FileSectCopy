#pragma once

/// <summary>
/// A structure to describe the location of file fragments on a volume, with the starting offsets of the continuous logical clusters and their quantity
/// 
/// ボリューム上に存在するファイル断片の所在を、論理クラスタ(Logical Cluster)としての開始位置および連続するクラスタ数を用いて記述するための構造体である。
/// </summary>
struct ClusterFlagment
{
public:
	unsigned long startClusterIndex;
	unsigned long clusterNumber;

	ClusterFlagment(unsigned long startClusterIndex, unsigned long clusterNumber);
};

