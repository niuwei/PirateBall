#ifndef _Utility_H_
#define _Utility_H_

#include <string>
#include "cocos2d.h"

namespace utility
{
	cocos2d::Node* seekNodeByName(cocos2d::Node* root, const std::string& name);
};


#endif