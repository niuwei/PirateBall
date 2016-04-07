#ifndef _EditLayer_H_
#define _EditLayer_H_

#include "HelloWorldScene.h"

class EnemyBall;
class BlockBase;
class BlockBox;
class EditLayer : public HelloWorld
{
public:
	virtual bool init();

	CREATE_FUNC(EditLayer);

	virtual EnemyBall* createEnemy(int type);
	virtual BlockBox* createBlock(int type);
	virtual BlockBase* createWheel();
	virtual BlockBase* createVortex();
	virtual BlockBase* createBomb();
	virtual void onUpdate(float dt);
	virtual void onMessage(unsigned int type, unsigned long param1, unsigned long param2);

private:
	void save();
	void showSelectedBallProperty();
	void showSelectedBlockBoxProperty();
	void deleteSelected();
	void changeEnemyType(int nTag, int type);
	void changeEnemyRingType(int nTag, int type);
	void changeBlockBoxType(int nTag, int type);
	void showPropertyLayer(cocos2d::Vec2 pos, int type, int ringType);
	void showBlockBoxPropertyLayer(cocos2d::Vec2 pos, int typeBox);
	void hidePropertyLayer();
	void hideBlockBoxPropertyLayer();
	void drawSelectedRect(cocos2d::Sprite* sprite);
	void switchCtrlLayerVisible();
	void showGrid(bool isShow);
};

#endif