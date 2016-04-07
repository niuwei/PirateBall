#include "GlobalDefined.h"
#include "Bullet.h"

Bullet::Bullet(BallBase::BallType type)
	: BallBase(type)
{
	_collisionType = BallBase::kBulletCollision;
}

bool Bullet::init()
{
	if ( !Sprite::initWithFile(getTextureName()) )
		return false;

	setSpeedValue(BULLET_SPEED_VALUE);

	return true;
}

void Bullet::setAngle( float angle )
{
	setSpeed(BallBase::calcSpeed(getSpeedValue(), angle));
}

void Bullet::onEnter()
{
	Sprite::onEnter();

	_emitter = ParticleSun::createWithTotalParticles(50);
	_emitter->setPosition(getContentSize() / 2.0);
	this->addChild(_emitter, -1);
}

void Bullet::onExit()
{
	_emitter->removeFromParent();
	Sprite::onExit();
}

void Bullet::changeType( int type )
{
	if (type != _type)
	{
		setType(type);
		setTexture(getTextureName());
	}
}

float Bullet::getRadius()
{
	return getContentSize().width / 2;
}
