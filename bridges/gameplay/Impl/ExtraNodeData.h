#ifndef CUBIQUITY_EXTRANODEDATA_H_
#define CUBIQUITY_EXTRANODEDATA_H_

#include "OctreeNode.h"

#include "gameplay.h"

template <typename VoxelType>
class ExtraNodeData
{
public:
	ExtraNodeData()
		:mTimeStamp(0)
		,mOctreeNode(0)
	{
	}

	Timestamp mTimeStamp;
	OctreeNode<VoxelType>* mOctreeNode;
};

template <typename VoxelType>
gameplay::Node* createNodeWithExtraData(const char* id = NULL) //NULL rather than '0' because that's what gameplay uses.
{
	gameplay::Node* node = Node::create(id);
	ExtraNodeData<VoxelType>* extraNodeData = new ExtraNodeData<VoxelType>;
	node->setUserPointer(extraNodeData, deleteExtraNodeData<VoxelType>);
	return node;
}

template <typename VoxelType>
void deleteExtraNodeData(void* ptr)
{
	ExtraNodeData<VoxelType>* extraNodeData = reinterpret_cast<ExtraNodeData<VoxelType>*>(ptr);
	delete extraNodeData;
}

#endif //CUBIQUITY_EXTRANODEDATA_H_
