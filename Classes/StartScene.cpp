#include "StartScene.h"
#include "VisibleRect.h"
#include "GlobalDefined.h"
#include "GlobalData.h"
#include "cocostudio/CocoStudio.h"
#include "HelloWorldScene.h"
#include "StageSelectScene.h"
#include "GameLoader.h"


USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;
using namespace network;

Scene* StartLayer::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = StartLayer::create();

	// add layer as a child to scene
	scene->addChild(layer, 0, TAG_START_LAYER);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool StartLayer::init()
{
	//////////////////////////////
	// 1. super init first
	if ( !Layer::init() )
	{
		return false;
	}
/*
	HttpRequest* request = new (std::nothrow) HttpRequest();
	request->setUrl("http://106.186.29.162:8080/hello");
	request->setRequestType(HttpRequest::Type::GET);
	request->setResponseCallback(CC_CALLBACK_2(StartLayer::onHttpRequestCompleted, this));

		request->setTag("GET test1");
		HttpClient::getInstance()->send(request);

	request->release();
*/
	GlobalData::getInstance()->init();
	GameLoader::getInstance()->load();
	
	auto background = Sprite::create("main-menu-bg.png");
	if (background)
	{
		background->setPosition(VisibleRect::center());
		this->addChild(background);
	}

	auto mainLogo = Sprite::create("main_pg_logo.png");
	if (mainLogo)
	{
		mainLogo->setPosition(VisibleRect::center() + Vec2(0.0, 160));
		this->addChild(mainLogo);
	}

	Node* node = CSLoader::createNode("MainManu.csb");
	if (node)
	{
		node->setPosition(Vec2::ZERO);
		addChild(node);

		Button* buttonStart = static_cast<Button*>(Helper::seekWidgetByName(
			static_cast<Layout*>(node), "ButtonStart"));
		if (buttonStart)
		{
			buttonStart->addClickEventListener([&](Ref* pSender){
				GlobalData::getInstance()->_gameMode = GAME_MODE_PLAY;
				Layer* pLayer = nullptr;
				Scene* scene = StageSelect::createScene(); 
				CCDirector::getInstance()->replaceScene(scene);
			});
		}
		ui::Button* buttonEdit = static_cast<ui::Button*>(ui::Helper::seekWidgetByName(
			static_cast<Layout*>(node), "ButtonEdit"));
		if (buttonEdit)
		{
			buttonEdit->addClickEventListener([&](Ref* pSender){
				GlobalData::getInstance()->_gameMode = GAME_MODE_EDIT;
				Layer* pLayer = nullptr;
				Scene* scene = StageSelect::createScene(); 
				CCDirector::getInstance()->replaceScene(scene);
			});
		}
		ui::Button* buttonExit = static_cast<ui::Button*>(ui::Helper::seekWidgetByName(
			static_cast<Layout*>(node), "ButtonExit"));
		if (buttonExit)
		{
			buttonExit->addClickEventListener([&](Ref* pSender){
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
				MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
				return;
#endif
				Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
				exit(0);
#endif
			});
		}
	}

	return true;
}

void StartLayer::onHttpRequestCompleted(HttpClient *sender, HttpResponse *response)
{
	if (!response)
	{
		return;
	}

	// You can get original request type from: response->request->reqType
	if (0 != strlen(response->getHttpRequest()->getTag())) 
	{
		log("%s completed", response->getHttpRequest()->getTag());
	}

	long statusCode = response->getResponseCode();
	char statusString[64] = {};
	sprintf(statusString, "HTTP Status Code: %ld, tag = %s", statusCode, response->getHttpRequest()->getTag());
	log("response code: %ld", statusCode);

	if (!response->isSucceed()) 
	{
		log("response failed");
		log("error buffer: %s", response->getErrorBuffer());
		return;
	}

	// dump data
	std::vector<char> *buffer = response->getResponseData();
	log("Http Test, dump data: ");
	for (unsigned int i = 0; i < buffer->size(); i++)
	{
		log("%c", (*buffer)[i]);
	}
	log("\n");
	if (response->getHttpRequest()->getReferenceCount() != 2)
	{
		log("request ref count not 2, is %d", response->getHttpRequest()->getReferenceCount());
	}
}