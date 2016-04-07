#include "BackgroundLayer.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalDefined.h"
#include "utility.h"
#include "BallBase.h"

USING_NS_CC;
using namespace ui;


BackgroundLayer::BackgroundLayer()
	: _launcherPos(Vec2::ZERO)
{

}

bool BackgroundLayer::init()
{
	if (!Layer::init())
		return false;

	Node* bkLayer = CSLoader::createNode("BackgroundLayer.csb");
	if (bkLayer)
	{
		bkLayer->setPosition(Vec2::ZERO);
		addChild(bkLayer);

		// get launcher sprite position mark on background by a node
		Node* launcherPos = utility::seekNodeByName(bkLayer, "NodeLauncher");
		assert(launcherPos);
		_launcherPos = launcherPos->getPosition();
	}

	return true;
}

cocos2d::Vec2 BackgroundLayer::getLauncherPos()
{
	return _launcherPos;
}

