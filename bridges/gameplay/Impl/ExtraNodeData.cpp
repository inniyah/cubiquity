#include "Impl/ExtraNodeData.h"

using namespace gameplay;

gameplay::Node* createNodeWithExtraData(const char* id)
{
	gameplay::Node* node = Node::create(id);
	ExtraNodeData* extraNodeData = new ExtraNodeData;
	node->setUserPointer(extraNodeData, deleteExtraNodeData);
	return node;
}

void deleteExtraNodeData(void* ptr)
{
	ExtraNodeData* extraNodeData = reinterpret_cast<ExtraNodeData*>(ptr);
	delete extraNodeData;
}