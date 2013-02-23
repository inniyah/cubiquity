#ifndef CUBIQUITY_EXTRANODEDATA_H_
#define CUBIQUITY_EXTRANODEDATA_H_

#include "OctreeNode.h"

#include "gameplay.h"

template <typename VertexType>
class ExtraNodeData
{
public:
	ExtraNodeData()
		:mTimeStamp(0)
		,mOctreeNode(0)
	{
	}

	Timestamp mTimeStamp;
	OctreeNode<VertexType>* mOctreeNode;
};

template <typename VertexType>
gameplay::Node* createNodeWithExtraData(const char* id = NULL) //NULL rather than '0' because that's what gameplay uses.
{
	gameplay::Node* node = Node::create(id);
	ExtraNodeData<VertexType>* extraNodeData = new ExtraNodeData<VertexType>;
	node->setUserPointer(extraNodeData, deleteExtraNodeData<VertexType>);
	return node;
}

template <typename VertexType>
void deleteExtraNodeData(void* ptr)
{
	ExtraNodeData<VertexType>* extraNodeData = reinterpret_cast<ExtraNodeData<VertexType>*>(ptr);
	delete extraNodeData;
}

#endif //CUBIQUITY_EXTRANODEDATA_H_
