#ifndef _EnemyBall_H_
#define _EnemyBall_H_

#include "BallBase.h"
#include "cocos2d.h"
#include "TouchableSprite.h"
#include "effect/ensShatterNode.h"

class EnemyBall 
	: public CshatterSprite
	, public BallBase
{
public:
	EnemyBall();
	EnemyBall(BallBase::BallType type);
	virtual ~EnemyBall();

	CREATE_FUNC_EX(EnemyBall, BallBase::BallType);

	virtual bool init();
	void addMonster();
	float getRadius();

	virtual void onTouchBegin();
	virtual void onTouchMoved(Vec2& pos);
	virtual void onTouchEnd();

	void removeThis();

	void changeType(int newType);

	bool onCollisionByRing(BALLTYPE bulletType);
	bool onCollision(BALLTYPE bulletType);
	bool isSameColor(int myType, int bulletType);
	void destroyBall();
	void destroyRing();

	void addRing(int type);

	// ��鵽��ǰΪֹ��node�Ƿ��ڳ���״̬������Ϊ���ڳ�����֡��
	bool isLongPressed(unsigned int frames);

private:
	unsigned int _fcountStartPress;
	bool _isMoved;
	bool _isLongPressed;
};

typedef TouchableSprite<EnemyBall>	EnemyBallTouchable;

#endif