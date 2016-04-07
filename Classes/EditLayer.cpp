#include "EditLayer.h"
#include "EnemyBall.h"
#include "BlockBox.h"
#include "VisibleRect.h"
#include "GlobalDefined.h"
#include "GlobalData.h"
#include "StageLoader.h"
#include "GameLoader.h"
#include "PropertyLayer.h"
#include "BlockBoxPropertyLayer.h"
#include "CtrlLayer.h"


bool EditLayer::init()
{
	if (!HelloWorld::init())
		return false;

	auto editItem = MenuItemImage::create("edit.png", "edit.png", [&](Ref* pSender){
		switchCtrlLayerVisible();
	});

	editItem->setPosition(Vec2(editItem->getContentSize().width/2 ,
		editItem->getContentSize().height/2));

	auto menu2 = Menu::create(editItem, NULL);
	menu2->setPosition(Vec2::ZERO);
	this->addChild(menu2, 100);
	
	return true;
}

EnemyBall* EditLayer::createEnemy( int type )
{
	EnemyBall* sprite1 = new(std::nothrow) EnemyBallTouchable();
	if (sprite1)
	{
		sprite1->setType(type);
		if (!sprite1->init())
		{
			delete sprite1;
			sprite1 = nullptr;
		}
		else
		{
			sprite1->autorelease();
		}
	}
	return sprite1;
}

BlockBox* EditLayer::createBlock( int type )
{
	BlockBox* sprite1 = new(std::nothrow) BlockBoxTouchable();
	if (!sprite1 || !sprite1->initWithBoxType(type))
	{
		delete sprite1;
		sprite1 = nullptr;
	}
	else
	{
		sprite1->autorelease();
	}
	return sprite1;
}

BlockBase* EditLayer::createWheel()
{
	BlockBase* sprite1 = new(std::nothrow) BlockWheelTouchable();
	if (!sprite1 || !sprite1->initWithFile(""))
	{
		delete sprite1;
		sprite1 = nullptr;
	}
	else
	{
		sprite1->autorelease();
	}
	return sprite1;
}

BlockBase* EditLayer::createVortex()
{
	BlockBase* sprite1 = new(std::nothrow) BlockVortexTouchable();
	if (!sprite1 || !sprite1->initWithFile(""))
	{
		delete sprite1;
		sprite1 = nullptr;
	}
	else
	{
		sprite1->autorelease();
	}
	return sprite1;
}

BlockBase* EditLayer::createBomb()
{
	BlockBase* sprite1 = new(std::nothrow) BlockBombTouchable();
	if (!sprite1 || !sprite1->initWithFile(""))
	{
		delete sprite1;
		sprite1 = nullptr;
	}
	else
	{
		sprite1->autorelease();
	}
	return sprite1;
}

void EditLayer::onUpdate( float dt )
{
	showSelectedBallProperty();
	showSelectedBlockBoxProperty();

#ifndef USE_PhysicsIntegration
	moveOnlyBullet();
#endif // !USE_PhysicsIntegration

	virtualBulletMove();

	// delete bullet out of visible rect
	checkBulletOutScreen();
}

void EditLayer::showSelectedBallProperty()
{
	for (auto& it : _itemTags)
	{
		EnemyBall* pBall = dynamic_cast<EnemyBall*>(this->getChildByTag(it));
		if (pBall)
		{
			if (pBall->getTag() == GlobalData::getInstance()->getSelectedTag())
			{
				showPropertyLayer(pBall->getPosition(), pBall->_type, pBall->_ringType);
				break;
			}
		}
	}
}

void EditLayer::showSelectedBlockBoxProperty()
{
	for (auto& it : _blockTags)
	{
		BlockBox* pBlock = dynamic_cast<BlockBox*>(this->getChildByTag(it));
		if (pBlock)
		{
			if (pBlock->getTag() == GlobalData::getInstance()->getSelectedTag())
			{
				showBlockBoxPropertyLayer(pBlock->getPosition(), pBlock->getBoxType());
				break;
			}
		}
	}
}

void EditLayer::onMessage( unsigned int type, unsigned long param1, unsigned long param2 )
{
	switch (type)
	{
	case MSG_DELETESELECTED:
		deleteSelected();
		break;

	case MSG_CHANGEITEMTYPE:
		changeEnemyType(param1, param2);
		break;

	case MSG_CHANGEITEMRINGTYPE:
		changeEnemyRingType(param1, param2);
		break;

	case MSG_CHANGEBOXTYPE:
		changeBlockBoxType(param1, param2);
		break;

	case MSG_CHANGESELECTED:
		{
			hidePropertyLayer();
			hideBlockBoxPropertyLayer();

			int old_sel = param1;
			int new_sel = param2;

			Node* pNode = this->getChildByTag(old_sel);
			if (pNode)
				pNode->removeChildByTag(TAG_SELECTED_RECT);

			Sprite* pSelected = dynamic_cast<Sprite*>(this->getChildByTag(new_sel));
			if (pSelected)
				drawSelectedRect(pSelected);
		}
		break;

	case MSG_ADDBLOCK:
		{
			switch (param1)
			{
			case kBlockBox:
				addBlock(Vec2(200, 200));
				break;

			case kBlockWheel:
				addWheel(Vec2(300, 300));
				break;

			case kBlockVortex:
				addVortex(Vec2(400, 400));
				break;

			case kBlockBomb:
				addBomb(Vec2(500, 500));
				break;
			default:
				break;
			}
		}
		break;

	case MSG_ADDENEMYITEM:
		addEnemy(BallBase::kTypeNormal1, Vec2(100, 100), BallBase::kTypeBulletUnknown);
		break;

	case MSG_SAVESTAGE:
		{
			_sectionId = param1;
			_stageId = param2;
			save();
		}
		break;

	case MSG_SETBULLETCOUNT:
		{
			_bulletCounter[param1] = param2;
			updateBulletCount();
		}
		break;

	default:
		break;
	}
}

void EditLayer::deleteSelected()
{
	int tag = GlobalData::getInstance()->getSelectedTag();
	removeChildByTag(tag);
}

void EditLayer::changeEnemyType( int nTag, int type )
{
	EnemyBall* pBall = dynamic_cast<EnemyBall*>(this->getChildByTag(nTag));
	if (pBall)
	{
		pBall->changeType(type);	
	}
}

void EditLayer::changeEnemyRingType( int nTag, int type )
{
	EnemyBall* pBall = dynamic_cast<EnemyBall*>(this->getChildByTag(nTag));
	if (pBall)
	{
		pBall->addRing(type);	
	}
}

void EditLayer::changeBlockBoxType( int nTag, int type )
{
	BlockBox* pBox = dynamic_cast<BlockBox*>(this->getChildByTag(nTag));
	if (pBox)
	{
		pBox->changeType(type);	
	}
}

void EditLayer::showPropertyLayer(cocos2d::Vec2 pos, int type, int ringType)
{	
	PropertyLayer* layer = dynamic_cast<PropertyLayer*>(this->getChildByTag(TAG_PROPERTY_LAYER));
	if (layer == nullptr)
	{
		layer = PropertyLayer::create(type, ringType);
		this->addChild(layer);
		layer->setTag(TAG_PROPERTY_LAYER);
	}

	// 显示属性菜单的方法
	layer->show(type, pos);
}

void EditLayer::showBlockBoxPropertyLayer( cocos2d::Vec2 pos, int typeBox )
{
	BlockBoxPropertyLayer* layer = dynamic_cast<BlockBoxPropertyLayer*>(
		this->getChildByTag(TAG_BOX_PROPERTY_LAYER));
	if (layer == nullptr)
	{
		layer = BlockBoxPropertyLayer::create(typeBox);
		layer->setTag(TAG_BOX_PROPERTY_LAYER);
		this->addChild(layer);
	}

	layer->show(pos);
}

void EditLayer::hidePropertyLayer()
{
	PropertyLayer* layer = dynamic_cast<PropertyLayer*>(this->getChildByTag(TAG_PROPERTY_LAYER));
	if (layer != nullptr && layer->isVisible())
	{
		layer->hide();
	}
}

void EditLayer::hideBlockBoxPropertyLayer()
{
	BlockBoxPropertyLayer* layer = dynamic_cast<BlockBoxPropertyLayer*>(
		getChildByTag(TAG_BOX_PROPERTY_LAYER));
	if (layer != nullptr && layer->isVisible())
	{
		layer->hide();
	}
}

void EditLayer::drawSelectedRect( cocos2d::Sprite* sprite )
{
	assert(sprite);
	removeChildByTag(TAG_SELECTED_RECT);
	Size s = sprite->getContentSize();
	auto draw = DrawNode::create();
	draw->setTag(TAG_SELECTED_RECT);
	sprite->addChild(draw);
	draw->drawRect(Vec2::ZERO, Vec2(s.width, s.height), Color4F::WHITE );
}

void EditLayer::switchCtrlLayerVisible()
{
	CtrlLayer* layer = dynamic_cast<CtrlLayer*>(this->getChildByTag(TAG_CONTROL_LAYER));
	if (layer == nullptr)
	{
		layer = CtrlLayer::create();
		this->addChild(layer, kZorderCtrlLayer, TAG_CONTROL_LAYER);
		layer->setVisible(false);
		layer->setSectionStageId(_sectionId, _stageId);
	}

	layer->setBulletCounts(_bulletCounter[BallBase::kTypeBullet1],
		_bulletCounter[BallBase::kTypeBullet2],
		_bulletCounter[BallBase::kTypeBullet3]);
	layer->setPosition(0.0, 0.0);
	showGrid(!layer->isVisible());
	layer->setVisible(!layer->isVisible());
}

void EditLayer::showGrid( bool isShow )
{
	float interval = GlobalData::getInstance()->getGridInterval();
	removeChildByTag(TAG_EDIT_GRID);
	if (!isShow)
		return;

	float width = VisibleRect::getVisibleRect().size.width;
	float height = VisibleRect::getVisibleRect().size.height;

	auto draw = DrawNode::create();
	for (float i = 0; i <= width; i += interval)
	{
		draw->drawLine(Vec2(i, 0), Vec2(i, height), Color4F(1.0, 1.0, 1.0, 0.5));
	}

	for (float j = 0; j <= height; j += interval)
	{
		draw->drawLine(Vec2(0, j), Vec2(width, j), Color4F(1.0, 1.0, 1.0, 0.5));
	}
	draw->setOpacity(0);
	addChild(draw, kZorderGrid, TAG_EDIT_GRID);
}

void EditLayer::save()
{
	StageLoader loader;
	loader._stageId = _stageId;

	for (auto& it : _itemTags)
	{
		EnemyBall* pBall = dynamic_cast<EnemyBall*>(this->getChildByTag(it));
		if (pBall)
		{
			loader.addEnemy(pBall->_type, pBall->getPosition(), pBall->_ringType);
		}
	}
	for (auto& it : _blockTags)
	{
		BlockBase* pBall = dynamic_cast<BlockBase*>(this->getChildByTag(it));
		if (pBall)
		{
			loader.addBlock(pBall->_type, pBall->getPosition(), pBall->_extendType);
		}
	}

	loader.setBullets(_bulletCounter, BallBase::kTypeBulletUnknown);

	GameLoader::getInstance()->saveStage(_sectionId, &loader);
}