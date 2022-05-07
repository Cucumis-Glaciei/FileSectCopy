#include "FileClusterDistribution.h"

#include <string>
#include <Windows.h>


FileClusterDistribution::FileClusterDistribution(std::string path) {

}

std::vector<ClusterFlagment> FileClusterDistribution::getDistribution()
{
	return std::vector<ClusterFlagment>();
}


RETRIEVAL_POINTERS_BUFFER FileClusterDistribution::getRetrievalPointers() {
	return RETRIEVAL_POINTERS_BUFFER();
}


long FileClusterDistribution::getRetrievalPointerBase() {
	return 0l;
}


int FileClusterDistribution::getClusterSize() {
	return 0;

}



