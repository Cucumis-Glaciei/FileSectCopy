#include "ClusterFragment.h"
#include <Windows.h>

ClusterFragment::ClusterFragment(LONGLONG startClusterIndex = 0, LONGLONG fragmentLength = 0)
{
	this->startClusterIndex = startClusterIndex;
	this->fragmentLength = fragmentLength;
}
