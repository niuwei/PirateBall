#ifndef _Block_Box_H_
#define _Block_Box_H_

#include "cocos2d.h"
#include "TouchableSprite.h"
#include "BlockBase.h"

class BlockBox : public BlockBase
{
public:
	typedef enum
	{
		kBoxTypeWhole,
		kBoxTypeTopLeft,
		kBoxTypeTop,
		kBoxTypeTopRight,
		kBoxTypeLeft,
		kBoxTypeCenter,
		KBoxTypeRight,
		kBoxTypeBottomLeft,
		kBoxTypeBottom,
		kBoxTypeBottomRight,
		kBoxTypeUnknown,
	} BlockBoxType;

	BlockBox() { _type = kBlockTypeBox; _extendType = kBoxTypeUnknown; }
	virtual ~BlockBox() {}

	bool initWithBoxType(int typeBox);

	virtual bool checkCollision(BallCollider* ball1);

	void changeType(int typeBox);
	int getBoxType() { return _extendType; }

	static std::string getTextureFile(int typeBox);


	static std::string		_textureBox[kBoxTypeUnknown];
};

typedef TouchableSprite<BlockBox>	BlockBoxTouchable;


class BlockWheel : public BlockBase
{
public:
	BlockWheel() { _type = kBlockTypeWheel; }
	virtual ~BlockWheel() {}

	virtual bool initWithFile(const std::string& filename);
	virtual void onCollision(Bullet* pBullet);
	virtual bool checkCollision(BallCollider* ball1);

private:
	int getColorTypeByAngle(float angle);
};

typedef TouchableSprite<BlockWheel>	BlockWheelTouchable;


class BlockVortex : public BlockBase
{
public:
	BlockVortex() { _type = kBlockTypeVortex; }
	virtual ~BlockVortex() {}

	virtual bool initWithFile(const std::string& filename);
	virtual void onCollision(Bullet* pBullet);
	virtual bool checkCollision(BallCollider* ball1);
};

typedef TouchableSprite<BlockVortex>	BlockVortexTouchable;

// bomb
class BlockBomb : public BlockBase
{
public:
	BlockBomb() { _type = kBlockTypeBomb; }
	virtual ~BlockBomb() {}

	virtual bool initWithFile(const std::string& filename);
	virtual void onCollision(Bullet* pBullet);
	virtual bool checkCollision(BallCollider* ball1);
};

typedef TouchableSprite<BlockBomb>	BlockBombTouchable;

#endif