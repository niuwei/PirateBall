#ifndef _Collision_H_
#define _Collision_H_

#include "cocos2d.h"

class BallCollider;

class BoxCollider
{
public:
	BoxCollider();
	BoxCollider(cocos2d::Vec2 pos, 
		cocos2d::Size size,
		float angle);

	bool isCollision(BallCollider* pBall);

	cocos2d::Vec2	_pos;
	cocos2d::Size	_size;
	float			_angle;
};

//////////////////////////////////////////////////////////////////////////
class BallCollider
{
public:
	BallCollider();
	BallCollider(cocos2d::Vec2 pos, 
		float radius, 
		cocos2d::Vec2 speed);

	bool isCollision(BallCollider* pBall);
	bool isCollision(BoxCollider* pBox);
	cocos2d::Vec2 calcSpeed(BallCollider* pBall);

	cocos2d::Vec2	_pos;
	float			_radius;
	cocos2d::Vec2	_speed;
};

#endif