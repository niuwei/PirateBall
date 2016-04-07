#ifndef _Bullet_H_
#define _Bullet_H_

#include "cocos2d.h"
#include "BallBase.h"

class Bullet 
	: public cocos2d::Sprite
	, public BallBase
{
public:
	Bullet(BallBase::BallType type);

	CREATE_FUNC_EX(Bullet, BallBase::BallType);

	virtual void onEnter();
	virtual void onExit();

	void setAngle(float angle);
	void changeType(int type);
	float getRadius();

	virtual bool init();

	cocos2d::ParticleSystemQuad*    _emitter;

};

#endif