#include "GlobalDefined.h"
#include "BlockBoxPropertyLayer.h"
#include "BlockBox.h"
#include "MessageDispatcher.h"
#include "cocostudio/CocoStudio.h"

USING_NS_CC;
using namespace ui;

BlockBoxPropertyLayer::BlockBoxPropertyLayer()
	: _layout(nullptr)
{

}

BlockBoxPropertyLayer* BlockBoxPropertyLayer::create( int typeBox )
{
	BlockBoxPropertyLayer* pRet = new(std::nothrow) BlockBoxPropertyLayer();
	if (pRet)
	{
		pRet->_type = typeBox;
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

bool BlockBoxPropertyLayer::init()
{
	if (!Layer::init())
		return false;

	Node* node = CSLoader::createNode("PropertyBlock.csb");
	if (node)
	{
		node->setPosition(Vec2::ZERO);
		node->setTag(TAG_EDIT_PROPERTY);
		this->addChild(node);

		ImageView* imageSelected = static_cast<ImageView*>(Helper::seekWidgetByName(
			static_cast<Layout*>(node), "ImageSelected"));
		if (imageSelected)
		{
			imageSelected->addClickEventListener(CC_CALLBACK_1(
				BlockBoxPropertyLayer::onImageSelecetedClick, this));
		}

		Node* listNode = CSLoader::createNode("ListViewHorizontal.csb");
		if (listNode)
		{
			listNode->setVisible(false);
			listNode->setTag(TAG_EDIT_PROPERTY_TYPELIST);
			node->addChild(listNode);

			ListView* listView = static_cast<ListView*>(Helper::seekWidgetByName(
				static_cast<Layout*>(listNode), "ListView"));
			if (listView)
			{
				for (int i = BlockBox::kBoxTypeWhole; i < BlockBox::kBoxTypeUnknown; ++ i)
				{
					ui::ImageView* image = ui::ImageView::create(BlockBox::getTextureFile(i));
					image->setTouchEnabled(true);
					image->addClickEventListener(CC_CALLBACK_1(BlockBoxPropertyLayer::onImageItemClick, this, i));
					listView->addChild(image);
				}
			}
			listNode->setContentSize(listView->getContentSize());
		}
	}

	return true;
}

void BlockBoxPropertyLayer::show( cocos2d::Vec2 pos )
{
	setPosition(pos);
	setVisible(true);
}

void BlockBoxPropertyLayer::hide()
{
	hideImageSelect();
	setVisible(false);
}

void BlockBoxPropertyLayer::onImageSelecetedClick( cocos2d::Ref* pSender )
{
	Layout* propertyNode = static_cast<Layout*>(this->getChildByTag(TAG_EDIT_PROPERTY));
	if (propertyNode)
	{
		Node* listView = propertyNode->getChildByTag(TAG_EDIT_PROPERTY_TYPELIST);
		if (listView && listView->isVisible())
		{
			listView->setVisible(false);
		}
		else
		{
			ImageView* imageSelected = static_cast<ImageView*>(
				Helper::seekWidgetByName(propertyNode, "ImageSelected"));
			Vec2 pos = imageSelected->getPosition();
			Size size = imageSelected->getContentSize();
			showImageSelect(pos, size);
		}
	}
}

void BlockBoxPropertyLayer::onImageItemClick( cocos2d::Ref* pSender, int type )
{
 	changeSelectedType(type);
 	hideImageSelect();
 	pushChangeSelectedTypeMsg(type);
}

void BlockBoxPropertyLayer::showImageSelect( cocos2d::Vec2 pos, cocos2d::Size size )
{
	Layout* propertyNode = static_cast<Layout*>(this->getChildByTag(TAG_EDIT_PROPERTY));
	if (propertyNode)
	{
		Layout* listView = static_cast<Layout*>(
			propertyNode->getChildByTag(TAG_EDIT_PROPERTY_TYPELIST));
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
}

void BlockBoxPropertyLayer::hideImageSelect()
{
	Node* propertyNode = this->getChildByTag(TAG_EDIT_PROPERTY);
	if (propertyNode)
	{
		Node* listView = propertyNode->getChildByTag(TAG_EDIT_PROPERTY_TYPELIST);
		if (listView)
		{
			listView->setVisible(false);
		}
	}
}

void BlockBoxPropertyLayer::changeSelectedType( int type )
{
	if (_type != type)
	{
		_type = type;
		resetSelectedImage();
	}
}

void BlockBoxPropertyLayer::resetSelectedImage()
{
	Layout* propertyNode = static_cast<Layout*>(this->getChildByTag(TAG_EDIT_PROPERTY));
	if (propertyNode)
	{
		ImageView* imageSelected = static_cast<ImageView*>(
			Helper::seekWidgetByName(propertyNode, "ImageSelected"));
		if (imageSelected)
		{
			imageSelected->loadTexture(BlockBox::getTextureFile(_type));
		}
	}
}

void BlockBoxPropertyLayer::pushChangeSelectedTypeMsg( int type )
{
	MessageDispatcher::getInstance()->pushMessage(
		MSG_CHANGEBOXTYPE, 
		GlobalData::getInstance()->getSelectedTag(), 
		type);
}

