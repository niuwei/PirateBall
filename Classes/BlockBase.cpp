#include "BlockBase.h"
#include "GlobalDefined.h"
#include "GlobalData.h"

USING_NS_CC;

std::string BlockBase::_textureBlock[BlockBase::kBlockTypeUnknown] =
{
	"",
	"block-stone.png",
	"wheel.png",
	"vortex.png",
	"bomb.png"
};

BlockBase::BlockBase()
{
	_canBeCollided = true;
	_type = kBlockTypeBox;
	_extendType = 0;
}

BlockBase::~BlockBase()
{

}

bool BlockBase::initWithFile( const std::string& filename )
{
	return Sprite::initWithFile(filename);
}

void BlockBase::onTouchBegin()
{
	this->setColor(Color3B::RED);

	int selTag = GlobalData::getInstance()->getSelectedTag();
	int thisTag = getTag();
	if (selTag != thisTag)
	{
		GlobalData::getInstance()->setSelectedTag(thisTag);
	}
}

void BlockBase::onTouchMoved( cocos2d::Vec2& pos )
{

}

void BlockBase::onTouchEnd()
{
	this->setColor(Color3B::WHITE);
}


