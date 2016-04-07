#ifndef _Property_Layer_H_
#define _Property_Layer_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"

class PropertyLayer : public cocos2d::Layer
{
public:
	virtual bool init();

	static PropertyLayer* create(int type, int ringType);

	CC_SYNTHESIZE(cocos2d::Size, _size, LayerSize);

	void show(int type, cocos2d::Vec2 pos);
	void hide();

	void onImageSelecetedClick(cocos2d::Ref* pSender);
	void onImageItemClick(cocos2d::Ref* pSender, int type);

	void onRingImageSelecetedClick(cocos2d::Ref* pSender);
	void onRingImageItemClick(cocos2d::Ref* pSender, int type);
	void showRingImageSelect(cocos2d::Vec2 pos, cocos2d::Size size);
	void hideRingImageSelect();
	void changeRingSelectedType(int type);
	void resetRingSelectedImage();
	void pushRingChangeSelectedTypeMsg(int type);

	void changeSelectedType(int type);
	void showImageSelect(cocos2d::Vec2 pos, cocos2d::Size size);
	void hideImageSelect();
	void resetSelectedImage();
	void pushChangeSelectedTypeMsg(int type);

	void setLayerPosition(cocos2d::Vec2 pos);

private:
	Node* getPropertyFrame();
	Node* getTypeSelectList();
	Node* getRingSelectList();
	Node* getTypeImageView();
	Node* getRingImageView();

	cocos2d::ui::Layout* _layout;
	cocos2d::ui::Layout* _layoutSelect;
	cocos2d::ui::Layout* _layoutSelectRing;
	int _type;
	int _ringType;
};

#endif