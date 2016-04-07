#ifndef _Block_Base_H_
#define _Block_Base_H_

#include "cocos2d.h"

class Bullet;
class BallCollider;
class BlockBase : public cocos2d::Sprite
{
public:
	typedef enum enumBlockType
	{
		kBlockTypeBox = 1,
		kBlockTypeWheel,
		kBlockTypeVortex,
		kBlockTypeBomb,
		kBlockTypeUnknown,
	} BlockType;

	BlockBase();
	virtual ~BlockBase();

	virtual bool initWithFile(const std::string& filename);

	virtual void onCollision(Bullet* pBullet) {}

	virtual void onTouchBegin();
	virtual void onTouchMoved(cocos2d::Vec2& pos);
	virtual void onTouchEnd();

	virtual bool checkCollision(BallCollider* ball1) = 0;

	bool canBeCollided()				{ return _canBeCollided; }
	void setCanBeCollided(bool bVal)	{ _canBeCollided = bVal; }

	bool				_canBeCollided;
	BlockType			_type;
	int					_extendType;
	
	static std::string	_textureBlock[kBlockTypeUnknown];
};

#endif