#ifndef _Ctrl_Layer_H_
#define _Ctrl_Layer_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "BallBase.h"

class CtrlLayer : public cocos2d::LayerColor
{
public:
	CtrlLayer();

	virtual bool init();
	void initEditMenu(cocos2d::Node* parent);
	void initSaveDialog(cocos2d::Node* parent);
	void showAddItemList(cocos2d::Vec2 pos);
	void hideAddItemList();
	void switchAddItemList(cocos2d::Vec2 pos);
	cocos2d::Node* createAddItemList();

	CREATE_FUNC(CtrlLayer);

	void menuSaveCallback(cocos2d::Ref* pSender);
	void menuBackCallback(cocos2d::Ref* pSender);
	void menuNextCallback(cocos2d::Ref* pSender);
	void menuCountAddCallback(cocos2d::Ref* pSender, int type);
	void menuCountRemoveCallback(cocos2d::Ref* pSender, int type);

	void textFieldEvent(Ref* pSender, cocos2d::ui::TextField::EventType type);

	void updateBulletCount();

	void setSectionId(int id);
	void setStageId(int id);

	void setSectionStageId(int sectionId, int stageId);
	void setBulletCounts(int blueCount, int redCount, int yellowCount);

	int _bulletCount[BallBase::kTypeBulletUnknown];
	int _sectionId;
	int _stageId;
};

#endif