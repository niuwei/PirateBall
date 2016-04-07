#include "PauseLayer.h"
#include "utility.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"
#include "GlobalDefined.h"
#include "MessageDispatcher.h"
#include "StartScene.h"

USING_NS_CC;
using namespace ui;

bool PauseLayer::init()
{
	if (!Layer::init())
		return false;

	Node* pauseLayer = CSLoader::createNode("PauseLayer.csb");
	if (pauseLayer)
	{
		pauseLayer->setPosition(Vec2::ZERO);
		addChild(pauseLayer);

		Button* buttonStart = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(pauseLayer), "ButtonStart"));
		if (buttonStart)
		{
			buttonStart->addClickEventListener([=](Ref* pSender){

				this->setVisible(false);
				Director::getInstance()->resume();
			});
		}

		Button* buttonReload = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(pauseLayer), "ButtonReload"));
		if (buttonReload)
		{
			buttonReload->addClickEventListener([=](Ref* pSender){

				this->setVisible(false);
				Director::getInstance()->resume();
				MessageDispatcher::getInstance()->pushMessage(MSG_LOADSTAGE);
			});
		}
		
		Button* buttonReturn = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(pauseLayer), "ButtonReturn"));
		if (buttonReturn)
		{
			buttonReturn->addClickEventListener([=](Ref* pSender){

				this->setVisible(false);
				Director::getInstance()->resume();
				Scene* scene = StartLayer::createScene();
				CCDirector::getInstance()->replaceScene(scene);
			});
		}
	}
	return true;
}
