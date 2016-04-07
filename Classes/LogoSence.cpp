#include "LogoSence.h"
#include "GlobalDefined.h"
#include "VisibleRect.h"
#include "StartScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"


USING_NS_CC;

Scene* LogoLayer::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = LogoLayer::create();

	// add layer as a child to scene
	scene->addChild(layer, 0, TAG_LOGO_LAYER);

	// return the scene
	return scene;
}

bool LogoLayer::init()
{
	if (!Layer::init())
		return false;
    
    auto rootNode = CSLoader::createNode("MainScene.csb");
    
    addChild(rootNode);

	Sprite* logo = Sprite::create("Logo.png");
	logo->setPosition(VisibleRect::center());
	this->addChild(logo);
    
	schedule(CC_SCHEDULE_SELECTOR(LogoLayer::changeScene), 1.0f);

	return true;
}

void LogoLayer::changeScene( float dt )
{
	unschedule(CC_SCHEDULE_SELECTOR(LogoLayer::changeScene));

	auto scene = StartLayer::createScene();
	TransitionScene* trans = TransitionFade::create(0.8f, scene);
	Director::getInstance()->replaceScene(trans);
}
