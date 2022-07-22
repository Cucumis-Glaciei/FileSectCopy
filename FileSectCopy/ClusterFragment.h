#pragma once
#include <Windows.h>

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
	LONGLONG startClusterIndex;
	/// <summary>
	/// ファイル断片の長さ
	/// </summary>
	LONGLONG fragmentLength;

	ClusterFragment(LONGLONG startClusterIndex, LONGLONG fragmentLength);
};

