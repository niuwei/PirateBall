#include "GlobalDefined.h"
#include "BallBase.h"
#include "assert.h"


USING_NS_CC;

std::string BallBase::_textureMap[kTypeUnknown] = 
{"blue.png", "red.png", "green.png", "white.png", 
"enemy-blue.png", "enemy-red.png", "enemy-yellow.png",
"boom-blue.png", "boom-red.png", "boom-green.png",
"change-blue.png", "change-red.png", "change-green.png"};

std::string BallBase::_textureRingMap[kTypeNormal1] = 
{
	"ring-blue.png",
	"ring-red.png",
	"ring-yellow.png",
	"white.png"
};


BallBase::BallBase( BallType type )
	: _type(type)
	, _state(kStateNormal)
	, _collisionType(kNoneCollision)
	, _speed(0, 0)
	, _speedValue(0.0)
	, _canBeCollided(true)
	, _ringType(kTypeBulletUnknown)
{

}

BallBase::~BallBase()
{

}

const std::string& BallBase::getTextureName()
{
	return _textureMap[_type];
}

// bool BallBase::checkCollision( BallBase* pBall )
// {
// 	Vec2 pos1 = dynamic_cast<Sprite*>(this)->getPosition();
// 	Vec2 pos2 = dynamic_cast<Sprite*>(pBall)->getPosition();
// 	return BallBase::isCollision(pos1, getRadius(), pos2, pBall->getRadius());
// }
// 
// void BallBase::changeSpeedByCollision( BallBase* pBall )
// {
// 	Vec2 srcPos = dynamic_cast<Sprite*>(this)->getPosition();
// 	Vec2 dstPos = dynamic_cast<Sprite*>(pBall)->getPosition();
// 	Vec2 newSpeed = BallBase::calcSpeedByCollision(getSpeed(), srcPos, dstPos);
// 	setSpeed(newSpeed);
// }

cocos2d::Vec2 BallBase::calcSpeed( float speedValue, float angle )
{
	float newAngle = 90.0 - angle; // angle: up == 0, lefe == -90, righe == 90
	Vec2 speed = Vec2::forAngle(CC_DEGREES_TO_RADIANS(newAngle));
	speed.scale(speedValue);
	return speed;
}

// bool BallBase::isCollision( Vec2 pos1, float radius1, Vec2 pos2, float radius2 )
// {
// 	float dist = radius1 + radius2;
// 	return pos1.getDistanceSq(pos2) <= dist * dist;
// }
// 
// cocos2d::Vec2 BallBase::calcSpeedByCollision( Vec2 srcSpeed, Vec2 srcPos, Vec2 dstPos )
// {
// 	Vec2 dist = dstPos - srcPos;
// 	Vec2 speed = srcSpeed;
// 	Vec2 v = speed.project(dist);
// 	speed += -2 * v;
// 	return speed;
// }
