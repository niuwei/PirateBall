#include "BlockBox.h"
#include "GlobalData.h"
#include "Bullet.h"
#include "collision.h"
#include "MessageDispatcher.h"

USING_NS_CC;

std::string BlockBox::_textureBox[BlockBox::kBoxTypeUnknown] = 
{ "block-stone.png",
"box-top-left.png", "box-top.png", "box-top-right.png",
"box-left.png", "box-center.png", "box-right.png",
"box-bottom-left.png", "box-bottom.png", "box-bottom-right.png"};


bool BlockBox::checkCollision( BallCollider* ball1 )
{
	float angle = 0.0;
	BoxCollider box(getPosition(), getContentSize(), angle);
	if (ball1->isCollision(&box))
	{
		return true;
	}
	return false;
}

void BlockBox::changeType( int typeBox )
{
	if (typeBox != getBoxType())
	{
		_extendType = typeBox;
		setTexture(getTextureFile(typeBox));
	}
}

std::string BlockBox::getTextureFile( int typeBox )
{
	assert(typeBox < BlockBox::kBoxTypeUnknown);
	return _textureBox[typeBox];
}

bool BlockBox::initWithBoxType( int typeBox )
{
	_extendType = typeBox;
	assert(typeBox < BlockBox::kBoxTypeUnknown);
	std::string texture_file = getTextureFile(_extendType);
	return initWithFile(texture_file);
}

//////////////////////////////////////////////////////////////////////////
//
bool BlockWheel::initWithFile( const std::string& filename )
{
	std::string blockFile = filename;
	if (blockFile.empty())
	{
		blockFile = BlockBase::_textureBlock[_type];
	}

	if (!BlockBase::initWithFile(blockFile))
		return false;

	auto halo = Sprite::create("wheel-halo.png");
	if (halo)
	{
		Size size = getContentSize();
		halo->setPosition(Vec2(size.width / 2, size.height / 2));
		addChild(halo);
	}

	return true;
}

void BlockWheel::onCollision( Bullet* pBullet )
{
	assert(pBullet);
	Vec2 wheel_bullet = pBullet->getPosition() - getPosition();
	float a1 = getRotation(); // 顺时针为+
	a1 = float_mod(a1, 360);
	float a2 = CC_RADIANS_TO_DEGREES(wheel_bullet.getAngle());
	float angle = a1 + a2;
	int type = getColorTypeByAngle(angle);
	pBullet->changeType(type);
}

int BlockWheel::getColorTypeByAngle( float angle )
{
	angle = float_mod(angle, 360);
	angle = angle < 0 ? 360 + angle : angle;
	assert(angle >= 0 && angle < 360);
	if (angle > 90 && angle <= 210)
	{
		return BallBase::kTypeBullet3;
	}
	else if (angle > 210 && angle <= 330)
	{
		return BallBase::kTypeBullet2;
	}
	else
	{
		return BallBase::kTypeBullet1;
	}
}

bool BlockWheel::checkCollision( BallCollider* ball1 )
{
	BallCollider ball2(getPosition(), getContentSize().width / 2, Vec2::ZERO);
	if (ball1->isCollision(&ball2))
	{
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//

bool BlockVortex::initWithFile( const std::string& filename )
{
	std::string blockFile = filename;
	if (blockFile.empty())
	{
		blockFile = BlockBase::_textureBlock[_type];
	}

	if (!BlockBase::initWithFile(blockFile))
		return false;

	return true;
}

void BlockVortex::onCollision( Bullet* pBullet )
{
	assert(pBullet);
	pBullet->setCanBeCollided(false);
	pBullet->setSpeed(Vec2::ZERO);

	Vec2 bulletPos = pBullet->getPosition();
	Vec2 blockPos = getPosition();
	Size bulletSize = pBullet->getContentSize();

	Sprite* rotateSprite = Sprite::create();
	getParent()->addChild(rotateSprite);
	rotateSprite->setPosition(blockPos);
	Vec2 pos = rotateSprite->convertToNodeSpace(bulletPos);
	Vec2 pos2 = rotateSprite->convertToNodeSpace(blockPos);
	pBullet->retain();
	pBullet->removeFromParent();
	rotateSprite->addChild(pBullet);
	pBullet->release();
	pBullet->setPosition(pos);

	auto actionBy = RotateBy::create(0.6, 360);
	auto moveTo = MoveTo::create(0.5, pos2);
	auto scaleBy = ScaleBy::create(0.5, 0.0);
	auto spawn = Spawn::create(scaleBy, moveTo, nullptr);
	pBullet->runAction(spawn);
	rotateSprite->runAction(Sequence::create(actionBy, 
		CallFunc::create([=](){
			pBullet->removeFromParent();
			rotateSprite->removeFromParent();
	}), nullptr));
}

bool BlockVortex::checkCollision( BallCollider* ball1 )
{
	BallCollider ball2(getPosition(), getContentSize().width / 2, Vec2::ZERO);
	if (ball1->isCollision(&ball2))
	{
		ball1->_speed = Vec2::ZERO; // vortex can stop bullet.
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//
bool BlockBomb::initWithFile( const std::string& filename )
{
	// 把光晕放在后面，实体放在前面
	// 计算尺寸的时候是计算实体的尺寸
	std::string blockFile = filename;
	if (blockFile.empty())
	{
		blockFile = BlockBase::_textureBlock[_type];
	}

	if (!BlockBase::initWithFile(blockFile))
		return false;

	auto sprite = Sprite::create("bomb-halo.png");
	if (!sprite)
		return false;

	Size size = getContentSize();
	sprite->setPosition(Vec2(size.width / 2, size.height / 2));
	addChild(sprite, -1);

	return true;
}

void BlockBomb::onCollision( Bullet* pBullet )
{
	assert(pBullet);
	pBullet->setCanBeCollided(false);
	pBullet->setSpeed(Vec2::ZERO);
	MessageDispatcher::getInstance()->pushMessage(MSG_DESTROYBULLET);
	removeFromParent();
}

bool BlockBomb::checkCollision( BallCollider* ball1 )
{
	Size sz = getContentSize();
	BallCollider ball2(getPosition(), getContentSize().width / 2, Vec2::ZERO);
	if (ball1->isCollision(&ball2))
	{
		ball1->_speed = Vec2::ZERO; // bomb can destroy bullet.
		return true;
	}
	return false;
}
