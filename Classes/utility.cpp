#include "utility.h"

USING_NS_CC;

namespace utility
{

Node* seekNodeByName(cocos2d::Node* root, const std::string& name)
{
	if (!root)
		return nullptr;
	
	if (root->getName() == name)
		return root;

	const auto& arrayNode = root->getChildren();
	for (auto& node : arrayNode)
	{
		Node* child = dynamic_cast<Node*>(node);
		if (child)
		{
			Node* res = seekNodeByName(child, name);
			if (res)
				return res;
		}
	}
	return nullptr;
}

};