#include "ExtraNodeData.h"

using namespace gameplay;

gameplay::Node* createNodeWithExtraData(const char* id)
{
	gameplay::Node* node = Node::create(id);
	ExtraNodeData* extraNodeData = new ExtraNodeData;
	node->setUserPointer(extraNodeData, deleteExtraNodeData);
	return node;
}

void deleteExtraNodeData(void* extraNodeData)
{
	delete extraNodeData;
}