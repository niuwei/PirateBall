#ifndef _StageOverLayer_H_
#define _StageOverLayer_H_

#include "cocos2d.h"

class StageOverLayer : public cocos2d::Layer
{
public:
	StageOverLayer();
	virtual ~StageOverLayer();

	static StageOverLayer* create(bool isSucceed);

	virtual bool init();
	void initSucceed();
	void initFailed();

	void showBestScore(cocos2d::Node* parent);
	void showBestTime();

	void onButtonReturn(cocos2d::Ref* pSender);
	void onButtonReload(cocos2d::Ref* pSender);
	void onButtonContinue(cocos2d::Ref* pSender);

private:
	bool					_isSucceed;
	cocos2d::Vec2			_posScore;
	cocos2d::Vec2			_posTime;
};

#endif