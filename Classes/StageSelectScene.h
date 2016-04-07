#ifndef _Stage_Select_Section_H_
#define _Stage_Select_Section_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"


class StageSelect : public cocos2d::Layer
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();
	void proInit();

	CREATE_FUNC(StageSelect);

	void onButtonClicked(Ref* pSender, int sectionId, int stageId);

	int _maxNextId;
};

#endif