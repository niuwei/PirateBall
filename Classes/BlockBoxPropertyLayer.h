#ifndef _BlockBoxPropertyLayer_H_
#define _BlockBoxPropertyLayer_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class BlockBoxPropertyLayer : public cocos2d::Layer
{
public:
	BlockBoxPropertyLayer();

	static BlockBoxPropertyLayer* create(int typeBox);

	virtual bool init();

	void show(cocos2d::Vec2 pos);
	void hide();

	void onImageSelecetedClick(cocos2d::Ref* pSender);
	void onImageItemClick(cocos2d::Ref* pSender, int type);

	void showImageSelect(cocos2d::Vec2 pos, cocos2d::Size size);
	void hideImageSelect();

	void changeSelectedType(int type);
	void resetSelectedImage();
	void pushChangeSelectedTypeMsg(int type);

private:
	cocos2d::ui::Layout*	_layout;
	int						_type;
};

#endif