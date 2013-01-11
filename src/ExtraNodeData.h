#ifndef CUBIQUITY_EXTRANODEDATA_H_
#define CUBIQUITY_EXTRANODEDATA_H_

#include "OctreeNode.h"

#include "gameplay.h"

class ExtraNodeData
{
public:
	ExtraNodeData()
		:mTimeStamp(0)
		,mOctreeNode(0)
	{
	}

	uint32_t mTimeStamp;
	OctreeNode* mOctreeNode;
};

gameplay::Node* createNodeWithExtraData(const char* id = NULL); //NULL rather than '0' because that's what gameplay uses.

void deleteExtraNodeData(void* ptr);

#endif //CUBIQUITY_EXTRANODEDATA_H_
