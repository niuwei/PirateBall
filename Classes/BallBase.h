#ifndef _BALL_BASE_H_
#define _BALL_BASE_H_

#include <utility>

#include "cocos2d.h"

USING_NS_CC;

#define CREATE_FUNC_EX(__TYPE__, _PARAM_TYPE_) \
	static __TYPE__* create(_PARAM_TYPE_  p) \
{ \
	__TYPE__ *pRet = new(std::nothrow) __TYPE__(p); \
	if (pRet && pRet->init()) \
{ \
	pRet->autorelease(); \
	return pRet; \
} \
	else \
{ \
	delete pRet; \
	pRet = NULL; \
	return NULL; \
} \
}

class BallBase
{
public:
	typedef enum enumBallType
	{
		kTypeBullet1 = 0,	// blue
		kTypeBullet2,		// red
		kTypeBullet3,		// green(yellow)
		kTypeBulletUnknown,
		kTypeNormal1,
		kTypeNormal2,
		kTypeNormal3,
		kTypeBoom1,
		kTypeBoom2,
		kTypeBoom3,
		kTypeChange1,
		kTypeChange2,
		kTypeChange3,
		kTypeUnknown,
	} BallType;

	typedef enum enumRingType
	{
		kRingTypeNormal1,
		kRingTypeNormal2,
		kRingTypeNormal3,
		kRingTypeUnknown,
	} RingType;

	typedef enum enumBallState
	{
		kStateNormal,
		kStateSelected,
		kStateDestroy,
	} BallState;

	typedef enum enumCollisionType
	{
		kNoneCollision,
		kBulletCollision,
	} CollisionType;

	BallBase(BallType type = kTypeUnknown);
	virtual ~BallBase();

	const std::string& getTextureName();

	CC_SYNTHESIZE(Vec2, _speed, Speed);
	CC_SYNTHESIZE(float, _speedValue, SpeedValue);

// 	bool checkCollision(BallBase* pBall);
// 	void changeSpeedByCollision(BallBase* pBall);
	static Vec2 calcSpeed(float speedValue, float angle);
//	static bool isCollision(Vec2 pos1, float radius1, Vec2 pos2, float radius2);
//	static Vec2 calcSpeedByCollision(Vec2 srcSpeed, Vec2 srcPos, Vec2 dstPos);
	
	void setType(int type)				{ _type = static_cast<BallType>(type); }
	bool canBeCollided()				{ return _canBeCollided; }
	void setCanBeCollided(bool bVal)	{ _canBeCollided = bVal; }

	bool						_canBeCollided;
	BallType					_type;
	BallState					_state;
	CollisionType				_collisionType;
	static std::string			_textureMap[kTypeUnknown];
	static std::string			_textureRingMap[kTypeNormal1];

	int							_ringType;
};


#endif