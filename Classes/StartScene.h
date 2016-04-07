#ifndef _Start_Scene_H_
#define _Start_Scene_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

#include "extensions/cocos-ext.h"
#include "network/HttpClient.h"

class StartLayer : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(StartLayer);

	void onHttpRequestCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
};


#endif