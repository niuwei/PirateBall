#include "PropertyLayer.h"
#include "cocostudio/CocoStudio.h"
#include "BallBase.h"
#include "GlobalDefined.h"
#include "GlobalData.h"
#include "VisibleRect.h"
#include "MessageDispatcher.h"
#include "EnemyBall.h"

USING_NS_CC;
using namespace ui;


PropertyLayer* PropertyLayer::create(int type, int ringType)
{
	PropertyLayer* pRet = new(std::nothrow) PropertyLayer();
	if (pRet)
	{
		pRet->_type = type;
		pRet->_ringType = ringType;
		if (pRet->init())
		{
			pRet->autorelease();
			return pRet;
		}
		delete pRet;
		pRet = NULL;
	}
	return NULL;
}

bool PropertyLayer::init()
{
	if (!Layer::init())
		return false;

	Node* node = CSLoader::createNode("PropertyEnemy.csb");
	if (node)
	{
		node->setPosition(Vec2::ZERO);
		node->setTag(TAG_EDIT_PROPERTY);
		this->addChild(node);
		this->setLayerSize(node->getContentSize()); // set layer size
	}
	// ball type select button
	ImageView* imageSelected = static_cast<ImageView*>(getTypeImageView());
	if (imageSelected)
	{
		imageSelected->addClickEventListener(CC_CALLBACK_1(
			PropertyLayer::onImageSelecetedClick, this));
	}
	resetSelectedImage();

	// ring type select button
	ImageView* imageSelectedRing = static_cast<ImageView*>(getRingImageView());
	if (imageSelectedRing)
	{
		imageSelectedRing->addClickEventListener(CC_CALLBACK_1(
			PropertyLayer::onRingImageSelecetedClick, this));
	}
	resetRingSelectedImage();


	// ball type select list view
	Node* childSelect = CSLoader::createNode("ListViewHorizontal.csb");
	if (childSelect)
	{
		childSelect->setVisible(false);
		childSelect->setTag(TAG_EDIT_PROPERTY_TYPELIST);
		this->addChild(childSelect);

		ListView* listView = static_cast<ListView*>(Helper::seekWidgetByName(
			static_cast<Layout*>(childSelect), "ListView"));
		if (listView)
		{
			for (int i = BallBase::kTypeNormal1; i < BallBase::kTypeUnknown; ++ i)
			{
				ImageView* image = ImageView::create(BallBase::_textureMap[i]);
				image->setTouchEnabled(true);
				image->addClickEventListener(CC_CALLBACK_1(PropertyLayer::onImageItemClick, this, i));
				listView->addChild(image);
			}
			childSelect->setContentSize(listView->getContentSize());
		}
	}

	// ring type select list view
	Node* childSelectRing = CSLoader::createNode("ListViewHorizontal.csb");
	if (childSelectRing)
	{
		childSelectRing->setVisible(false);
		childSelectRing->setTag(TAG_EDIT_PROPERTY_RINGLIST);
		this->addChild(childSelectRing);

		ListView* listView = static_cast<ListView*>(Helper::seekWidgetByName(
			static_cast<Layout*>(childSelectRing), "ListView"));
		if (listView)
		{
			for (int i = BallBase::kRingTypeNormal1; i <= BallBase::kRingTypeUnknown; ++ i)
			{
				ImageView* image = ImageView::create(BallBase::_textureRingMap[i]);
				image->setTouchEnabled(true);
				image->addClickEventListener(CC_CALLBACK_1(PropertyLayer::onRingImageItemClick, this, i));
				listView->addChild(image);
			}
			childSelectRing->setContentSize(listView->getContentSize());
		}
	}
	return true;
}

void PropertyLayer::changeSelectedType( int type )
{
	if (_type != type)
	{
		_type = type;
		resetSelectedImage();
	}
}

void PropertyLayer::changeRingSelectedType( int type )
{
	if (_ringType != type)
	{
		_ringType = type;
		resetRingSelectedImage();
	}
}

void PropertyLayer::resetSelectedImage()
{
	ImageView* imageSelected = static_cast<ImageView*>(getTypeImageView());
	if (imageSelected)
	{
		imageSelected->loadTexture(BallBase::_textureMap[_type]);
	}
}

void PropertyLayer::resetRingSelectedImage()
{
	ImageView* imageSelected = static_cast<ImageView*>(getRingImageView());
	if (imageSelected)
	{
		imageSelected->loadTexture(BallBase::_textureRingMap[_ringType]);
	}
}

void PropertyLayer::setLayerPosition( cocos2d::Vec2 pos )
{
	auto size = getLayerSize();
	Vec2 lefttop;
	Size offsize(DEFAULT_BALL_RADIUS, DEFAULT_BALL_RADIUS);
	bool rightEnough = pos.x + offsize.width + size.width < VisibleRect::right().x;
	bool leftEnough = pos.x - offsize.width - size.width > VisibleRect::left().x;
	bool bottomEnough = pos.y - offsize.height - size.height > VisibleRect::bottom().y;
	bool topEnough = pos.y + offsize.height + size.height < VisibleRect::top().y;

	float rightX = pos.x + offsize.width;
	float leftX = pos.x - offsize.width - size.width;
	float maxX = VisibleRect::right().x - size.width;
	float bottomY = pos.y - offsize.height;
	float topY = pos.y + offsize.height + size.height;
	float minY = VisibleRect::bottom().y + size.height;
	if (rightEnough)
		lefttop.x = rightX;
	else if (leftEnough)
		lefttop.x = leftX;
	else
		lefttop.x = maxX;

	if (bottomEnough)
		lefttop.y = bottomY;
	else if (topEnough)
		lefttop.y = topY;
	else
		lefttop.y = minY;

	// Layer child's position base on left bottom.
	// Sprite child's position base on center.
	Vec2 leftbottom(lefttop.x, lefttop.y - size.height);
	setPosition(leftbottom);
}

void PropertyLayer::onImageSelecetedClick( cocos2d::Ref* pSender )
{
	Node* listView = getTypeSelectList();
	if (listView)
	{
		if (listView->isVisible())
		{
			listView->setVisible(false);
		}
		else
		{
			ImageView* imageSelected = static_cast<ImageView*>(getTypeImageView());
			if (imageSelected)
			{
				Vec2 pos = imageSelected->getPosition();
				Size size = imageSelected->getContentSize();
				hideRingImageSelect();
				showImageSelect(pos, size);
			}
		}
	}
}

void PropertyLayer::onRingImageSelecetedClick( cocos2d::Ref* pSender )
{
	Node* listView = getRingSelectList();
	if (listView)
	{
		if (listView->isVisible())
		{
			listView->setVisible(false);
		}
		else
		{
			ImageView* imageSelected = static_cast<ImageView*>(getRingImageView());
			if (imageSelected)
			{
				Vec2 pos = imageSelected->getPosition();
				Size size = imageSelected->getContentSize();
				hideImageSelect();
				showRingImageSelect(pos, size);
			}
		}
	}
}

void PropertyLayer::showImageSelect( cocos2d::Vec2 pos, cocos2d::Size size )
{
	Node* listView = getTypeSelectList();
	if (listView)
	{
		Vec2 parentPos = getPosition();
		Size listviewSize = listView->getContentSize();
		Vec2 left_bottom;
		left_bottom.y = pos.y + size.height / 2;
		left_bottom.x = pos.x - listviewSize.width / 2;
		listView->setPosition(left_bottom);
		listView->setVisible(true);
	}
}

void PropertyLayer::hideImageSelect()
{
	Node* listView = getTypeSelectList();
	if (listView)
	{
		listView->setVisible(false);
	}
}

void PropertyLayer::pushChangeSelectedTypeMsg( int type )
{
	MessageDispatcher::getInstance()->pushMessage(
		MSG_CHANGEITEMTYPE, 
		GlobalData::getInstance()->getSelectedTag(), 
		type);
}

void PropertyLayer::pushRingChangeSelectedTypeMsg( int type )
{
	MessageDispatcher::getInstance()->pushMessage(
		MSG_CHANGEITEMRINGTYPE, 
		GlobalData::getInstance()->getSelectedTag(), 
		type);
}

void PropertyLayer::show( int type, cocos2d::Vec2 pos )
{
	changeSelectedType(type);
	setLayerPosition(pos);
	setVisible(true);
}

void PropertyLayer::hide()
{
	hideImageSelect();
	hideRingImageSelect();
	setVisible(false);
}

void PropertyLayer::onImageItemClick( cocos2d::Ref* pSender, int type )
{
	changeSelectedType(type);
	hideImageSelect();
	pushChangeSelectedTypeMsg(type);
}

void PropertyLayer::onRingImageItemClick( cocos2d::Ref* pSender, int type )
{
	changeRingSelectedType(type);
	hideRingImageSelect();
	pushRingChangeSelectedTypeMsg(type);
}

void PropertyLayer::showRingImageSelect( cocos2d::Vec2 pos, cocos2d::Size size )
{
	Node* listView = getRingSelectList();
	if (listView)
	{
		Vec2 parentPos = getPosition();
		Size listviewSize = listView->getContentSize();
		Vec2 left_bottom;
		left_bottom.y = pos.y + size.height / 2;
		left_bottom.x = pos.x - listviewSize.width / 2;
		listView->setPosition(left_bottom);
		listView->setVisible(true);
	}
}

void PropertyLayer::hideRingImageSelect()
{
	Node* listView = getRingSelectList();
	if (listView)
	{
		listView->setVisible(false);
	}
}

Node* PropertyLayer::getPropertyFrame()
{
	return this->getChildByTag(TAG_EDIT_PROPERTY);
}

Node* PropertyLayer::getTypeSelectList()
{
	return this->getChildByTag(TAG_EDIT_PROPERTY_TYPELIST);
}

Node* PropertyLayer::getRingSelectList()
{
	return this->getChildByTag(TAG_EDIT_PROPERTY_RINGLIST);
}

Node* PropertyLayer::getTypeImageView()
{
	Node* node = getPropertyFrame();
	if (node)
	{
		return Helper::seekWidgetByName(static_cast<Layout*>(node), "ImageSelected");
	}
	return nullptr;
}

Node* PropertyLayer::getRingImageView()
{
	Node* node = getPropertyFrame();
	if (node)
	{
		return Helper::seekWidgetByName(static_cast<Layout*>(node), "ImageSelectedRing");
	}
	return nullptr;
}
