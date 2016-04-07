#include "StageOverLayer.h"
#include "VisibleRect.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"
#include "StageSelectScene.h"
#include "GlobalDefined.h"
#include "MessageDispatcher.h"
#include "ScoreCounter.h"

USING_NS_CC;
using namespace ui;

StageOverLayer::StageOverLayer()
{

}

StageOverLayer::~StageOverLayer()
{

}

StageOverLayer* StageOverLayer::create( bool isSucceed )
{
	StageOverLayer* pRet = new(std::nothrow) StageOverLayer();
	if (pRet)
	{
		pRet->_isSucceed = isSucceed;
		if (pRet->init())
		{
			pRet->autorelease();
			return pRet;
		}
		delete pRet;
		pRet = NULL;
	}
	return NULL;
}

bool StageOverLayer::init()
{
	if (!Layer::init())
		return false;

	auto background = Sprite::create("stage-over-bg.png");
	if (background)
	{
		background->setPosition(VisibleRect::center());
		this->addChild(background);
	}

	if (_isSucceed)
	{
		initSucceed();
		
	}
	else
	{
		initFailed();
	}

	return true;
}

void StageOverLayer::initSucceed()
{
	Node* node = CSLoader::createNode("LevelClearedMenu.csb");
	if (node)
	{
		addChild(node);

		Button* buttonReturn = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(node), "ButtonReturn"));
		if (buttonReturn)
		{
			buttonReturn->addClickEventListener(CC_CALLBACK_1(
				StageOverLayer::onButtonReturn, this));
		}

		Button* buttonReload = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(node), "ButtonReload"));
		if (buttonReload)
		{
			buttonReload->addClickEventListener(CC_CALLBACK_1(
				StageOverLayer::onButtonReload, this));
		}

		Button* buttonContinue = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(node), "ButtonContinue"));
		if (buttonContinue)
		{
			buttonContinue->addClickEventListener(CC_CALLBACK_1(
				StageOverLayer::onButtonContinue, this));
		}

		int score = ScoreCounter::getInstance()->getScore();
		Text* txtScore = static_cast<Text*>(
			Helper::seekWidgetByName(static_cast<Layout*>(node), "TextScore"));
		if (txtScore)
		{
			std::string strScore = StringUtils::format("%d", score);
			_posScore = txtScore->getParent()->convertToWorldSpaceAR(txtScore->getPosition());
			txtScore->setString(strScore);
		}

		int time = ScoreCounter::getInstance()->getTime();
		Text* txtTime = static_cast<Text*>(
			Helper::seekWidgetByName(static_cast<Layout*>(node), "TextTime"));
		if (txtTime)
		{
			std::string strTime = StringUtils::format("%02d:%02d", time / 60, time % 60);
			_posTime = txtTime->getParent()->convertToWorldSpaceAR(txtTime->getPosition());
			txtTime->setString(strTime);
		}

		showBestScore(node);
		showBestTime();
	}
}

void StageOverLayer::initFailed()
{
	Node* node = CSLoader::createNode("LevelFailedMenu.csb");
	if (node)
	{
		addChild(node);

		Button* buttonReturn = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(node), "ButtonReturn"));
		if (buttonReturn)
		{
			buttonReturn->addClickEventListener(CC_CALLBACK_1(
				StageOverLayer::onButtonReturn, this));
		}

		Button* buttonReload = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(node), "ButtonReload"));
		if (buttonReload)
		{
			buttonReload->addClickEventListener(CC_CALLBACK_1(
				StageOverLayer::onButtonReload, this));
		}
	}
}

void StageOverLayer::onButtonReturn( cocos2d::Ref* pSender )
{
	Scene* scene = StageSelect::createScene();
	CCDirector::getInstance()->replaceScene(scene);
}

void StageOverLayer::onButtonReload( cocos2d::Ref* pSender )
{
	MessageDispatcher::getInstance()->pushMessage(MSG_LOADSTAGE);
	removeFromParent();
}

void StageOverLayer::onButtonContinue( cocos2d::Ref* pSender )
{
	MessageDispatcher::getInstance()->pushMessage(MSG_LOADSTAGE, 0, 1);
	removeFromParent();
}

void StageOverLayer::showBestScore(Node* parent)
{
	Sprite* bestScore = Sprite::create("end_bestscore.png");
	if (bestScore)
	{
		bestScore->setPosition(_posScore + Vec2(100.0, 0));
		parent->addChild(bestScore);
		bestScore->setScale(3.0);

		auto actScale = ScaleTo::create(1.0, 1.0, 1.0, 1.0);
		bestScore->runAction(actScale);
	}
}

void StageOverLayer::showBestTime()
{
	Sprite* node = Sprite::create("end_besttime.png");
	if (node)
	{
		node->setPosition(_posTime + Vec2(100.0, 0));
		addChild(node);
		node->setScale(3.0);

		auto actScale = ScaleTo::create(1.0, 1.0, 1.0, 1.0);
		node->runAction(actScale);
	}
}


