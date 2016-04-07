#ifndef _LogoSence_H_
#define _LogoSence_H_

#include "cocos2d.h"

class LogoLayer : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(LogoLayer);

	void changeScene(float dt);
};

#endif