#ifndef _BackgroundLayer_H_
#define _BackgroundLayer_H_

#include "cocos2d.h"


class BackgroundLayer : public cocos2d::Layer
{
public:
	BackgroundLayer();

	CREATE_FUNC(BackgroundLayer);

	virtual bool init();
	cocos2d::Vec2 getLauncherPos();

private:
	cocos2d::Vec2		_launcherPos;
};

#endif