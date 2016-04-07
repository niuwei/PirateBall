#include "GlobalDefined.h"
#include "GlobalData.h"
#include "EnemyBall.h"
#include "MessageDispatcher.h"
#include "cocostudio/CocoStudio.h"
#include "ScoreCounter.h"


USING_NS_CC;


EnemyBall::EnemyBall()
	: _fcountStartPress(0)
	, _isMoved(false)
	, _isLongPressed(false)
{

}

EnemyBall::EnemyBall(BallBase::BallType type)
	: BallBase(type)
	, _fcountStartPress(0)
	, _isMoved(false)
	, _isLongPressed(false)
{

}

EnemyBall::~EnemyBall()
{

}

bool EnemyBall::init()
{
	if ( !CshatterSprite::init(getTextureName()) )
		return false;

	addMonster();

	return true;
}

void EnemyBall::addMonster()
{
	std::string monsterFile[kTypeBulletUnknown] = {"MonsterBlue.csb", "MonsterRed.csb", "MonsterYellow.csb"};

	int i = (_type - kTypeNormal1) % kTypeBulletUnknown;
	Node* rootNode = CSLoader::createNode(monsterFile[i]);
	cocostudio::timeline::ActionTimeline* action = CSLoader::createTimeline(monsterFile[i]);
	rootNode->runAction(action);
	action->gotoFrameAndPlay(0, 80, true);
	Size size = getContentSize();
	rootNode->setPosition(Vec2(size.width / 2, size.height / 2));
	rootNode->setTag(TAG_BABY);
	addChild(rootNode);
}

void EnemyBall::changeType( int newType )
{
	if (newType != _type)
	{
		_type = static_cast<BALLTYPE>(newType);
		setTexture(_textureMap[_type]);

		removeChildByTag(TAG_BABY);
		addMonster();
	}
}

bool EnemyBall::isSameColor( int myType, int bulletType )
{
	if (myType < BallBase::kTypeNormal1)
		myType += BallBase::kTypeNormal1;

	return (myType - BallBase::kTypeNormal1) % 
		BallBase::kTypeBulletUnknown == bulletType;
}

bool EnemyBall::onCollisionByRing( BALLTYPE bulletType )
{
	Sprite* ring = static_cast<Sprite*>(getChildByTag(TAG_RING));
	if (ring)
	{
		if (isSameColor(_ringType, bulletType))
		{
			destroyRing();
			return true;
		}
	}
	return false;
}

bool EnemyBall::onCollision(BALLTYPE bulletType)
{
	bool removeThisEnemy = false;

	// has ring, do nothing on ball.
	if (onCollisionByRing(bulletType))
	{
		return removeThisEnemy;
	}

	if (isSameColor(_type, bulletType))
	{
		destroyBall();
		removeThisEnemy = true;
	}
	else
	{
		if (_type >= BallBase::kTypeChange1)
		{
			changeType(BallBase::kTypeChange1 + bulletType - BallBase::kTypeBullet1);
		}
	}

	if (_type >= BallType::kTypeBoom1 && _type <= BallBase::kTypeBoom3)
	{
		MessageDispatcher::getInstance()->pushMessage(MSG_DESTROYBULLET);
	}

	return removeThisEnemy;
}

void EnemyBall::destroyRing()
{
	Sprite* ring = static_cast<Sprite*>(getChildByTag(TAG_RING));
	if (ring)
	{
		removeChild(ring, true);
		_ringType = BallBase::kTypeBulletUnknown;
	}
	ScoreCounter::getInstance()->onDestroyEnemy(getPosition().x, getPosition().y);
}

void EnemyBall::destroyBall()
{
	// 爆炸效果
	setOpacity(0);
	stopAllActions();
	shatter::CshatterAction*shatter=shatter::CshatterAction::create(4);
	runAction(Sequence::create(shatter, CallFunc::create(CC_CALLBACK_0(EnemyBall::removeThis,this)), nullptr));

	Sprite* baby = static_cast<Sprite*>(getChildByTag(TAG_BABY));
	if (baby)
	{
		baby->stopAllActions();
		removeChild(baby, true);
	}
	ScoreCounter::getInstance()->onDestroyEnemy(getPosition().x, getPosition().y);
}

void EnemyBall::removeThis()
{
	removeFromParent();
}

void EnemyBall::onTouchBegin()
{
	this->setColor(Color3B::RED);
	_fcountStartPress = Director::getInstance()->getTotalFrames();
	_isLongPressed = false;

	int selTag = GlobalData::getInstance()->getSelectedTag();
	int thisTag = getTag();
	if (selTag != thisTag)
	{
		GlobalData::getInstance()->setSelectedTag(thisTag);
	}
}

void EnemyBall::onTouchMoved(Vec2& pos)
{
	_isMoved = true;
}

void EnemyBall::onTouchEnd()
{
	this->setColor(Color3B::WHITE);
	_isMoved = false;
	_fcountStartPress = 0;
	_isLongPressed = false;
}

bool EnemyBall::isLongPressed( unsigned int frames )
{
	if (_fcountStartPress == 0)
		return false;

	if (_isMoved)
		return false;

	// 只在第一次满足长按条件时返回true
	if (_isLongPressed)
		return false;

	unsigned int cur_frames = Director::getInstance()->getTotalFrames();
	_isLongPressed = (cur_frames - _fcountStartPress >= frames);
	return _isLongPressed;
}

void EnemyBall::addRing( int type )
{
	if (_ringType != type)
	{
		_ringType = type;
		
		if (_ringType == BallBase::kTypeBulletUnknown)
		{
			destroyRing();
			return;
		}

		std::string texture_path = BallBase::_textureRingMap[type];
		Sprite* ring = static_cast<Sprite*>(getChildByTag(TAG_RING));
		if (ring == nullptr)
		{
			ring = Sprite::create(texture_path);
			if (ring)
			{
				ring->setTag(TAG_RING);
				Size size = getContentSize();
				ring->setPosition(Vec2(size.width / 2, size.height / 2));
				this->addChild(ring);
			}
		}
		else
		{
			ring->setTexture(texture_path);
		}
	}
}

float EnemyBall::getRadius()
{
	return getContentSize().width / 2;
}

