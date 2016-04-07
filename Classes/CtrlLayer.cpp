#include "CtrlLayer.h"
#include "VisibleRect.h"
#include "GlobalDefined.h"
#include "HelloWorldScene.h"
#include "cocostudio/CocoStudio.h"
#include "MessageDispatcher.h"
#include "BlockBox.h"
#include "BallBase.h"

USING_NS_CC;
using namespace ui;


CtrlLayer::CtrlLayer()
{
	_sectionId = 0;
	_stageId = 0;
	for (int i = 0; i < BallBase::kTypeBulletUnknown; ++ i)
	{
		_bulletCount[i] = 0;
	}
}

bool CtrlLayer::init()
{
	if (!LayerColor::init())
		return false;

	initEditMenu(this);
	initSaveDialog(this);
	return true;
}

void CtrlLayer::menuSaveCallback( cocos2d::Ref* pSender )
{
	Node* saveDlg = this->getChildByTag(TAG_EDIT_SAVE_DLG);
	if (saveDlg)
	{
		saveDlg->setVisible(true);
	}

	setSectionId(_sectionId);
	setStageId(_stageId);
}

void CtrlLayer::menuBackCallback( cocos2d::Ref* pSender )
{
	MessageDispatcher::getInstance()->pushMessage(MSG_LOADSTAGE, 0, -1);
}

void CtrlLayer::menuNextCallback( cocos2d::Ref* pSender )
{
	MessageDispatcher::getInstance()->pushMessage(MSG_LOADSTAGE, 0, 1);
}

void CtrlLayer::menuCountAddCallback( cocos2d::Ref* pSender, int type )
{
	if (_bulletCount[type] < 10)
	{
		_bulletCount[type] ++;
		updateBulletCount();
		MessageDispatcher::getInstance()->pushMessage(MSG_SETBULLETCOUNT, type, _bulletCount[type]);
	}
}

void CtrlLayer::menuCountRemoveCallback( cocos2d::Ref* pSender, int type )
{
	if (_bulletCount[type] > 0)
	{
		_bulletCount[type] --;
		updateBulletCount();
		MessageDispatcher::getInstance()->pushMessage(MSG_SETBULLETCOUNT, type, _bulletCount[type]);
	}
}

void CtrlLayer::updateBulletCount()
{
	Layout* layout = static_cast<Layout*>(this->getChildByTag(TAG_EDIT_MEUN));
	if (layout)
	{
		Text* labelBlue = static_cast<Text*>(ui::Helper::seekWidgetByName(layout, "TextBlueCount"));
		if (labelBlue)
		{
			std::string label_str = StringUtils::format("%d", _bulletCount[BallBase::kTypeBullet1]);
			labelBlue->setString(label_str);
		}
		Text* labelRed = static_cast<Text*>(ui::Helper::seekWidgetByName(layout, "TextRedCount"));
		if (labelRed)
		{
			std::string label_str = StringUtils::format("%d", _bulletCount[BallBase::kTypeBullet2]);
			labelRed->setString(label_str);
		}
		Text* labelGreen = static_cast<Text*>(ui::Helper::seekWidgetByName(layout, "TextYellowCount"));
		if (labelGreen)
		{
			std::string label_str = StringUtils::format("%d", _bulletCount[BallBase::kTypeBullet3]);
			labelGreen->setString(label_str);
		}
	}
}

void CtrlLayer::textFieldEvent(Ref *pSender, ui::TextField::EventType type)
{
	switch (type)
	{
	case ui::TextField::EventType::ATTACH_WITH_IME:
		break;

	case ui::TextField::EventType::DETACH_WITH_IME:
		{
			ui::TextField* textField = dynamic_cast<ui::TextField*>(pSender);
		}
		break;

	case ui::TextField::EventType::INSERT_TEXT:
		break;

	case ui::TextField::EventType::DELETE_BACKWARD:
		break;

	default:
		break;
	}
}

void CtrlLayer::setSectionId( int id )
{
	Layout* saveDlg = static_cast<Layout*>(this->getChildByTag(TAG_EDIT_SAVE_DLG));
	if (saveDlg)
	{
		TextField* txtSectionId = static_cast<TextField*>(
			Helper::seekWidgetByName(saveDlg, "TextFieldSectionId"));
		if (txtSectionId)
		{
			txtSectionId->setString(StringUtils::format("%d", id));
		}
	}	
}

void CtrlLayer::setStageId( int id )
{
	Layout* saveDlg = static_cast<Layout*>(this->getChildByTag(TAG_EDIT_SAVE_DLG));
	if (saveDlg)
	{
		TextField* txtStageId = static_cast<TextField*>(
			Helper::seekWidgetByName(saveDlg, "TextFieldStageId"));
		if (txtStageId)
		{
			txtStageId->setString(StringUtils::format("%d", id));
		}
	}
}

void CtrlLayer::setSectionStageId( int sectionId, int stageId )
{
	_sectionId = sectionId;
	_stageId = stageId;
}

void CtrlLayer::initEditMenu( cocos2d::Node* parent )
{
	assert(parent);
	Node* editMenu = CSLoader::createNode("EditMenu.csb");
	if (editMenu)
	{
		editMenu->setPosition(Vec2::ZERO);
		editMenu->setTag(TAG_EDIT_MEUN);
		parent->addChild(editMenu);

		Button* buttonAdd = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(editMenu), "ButtonAdd"));
		if (buttonAdd)
		{
			buttonAdd->addClickEventListener([=](Ref*){
				Vec2 btnPosition = buttonAdd->getPosition();
				Size btnSize = buttonAdd->getContentSize();
				Vec2 pos(btnPosition.x - btnSize.width / 2,
					btnPosition.y + btnSize.height / 2);
				pos = editMenu->convertToWorldSpaceAR(pos);
				switchAddItemList(pos);
			});
		}

		Button* buttonDelete = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(editMenu), "ButtonDelete"));
		if (buttonDelete)
		{
			buttonDelete->addClickEventListener([=](Ref*){
				MessageDispatcher::getInstance()->pushMessage(MSG_DELETESELECTED);
			});
		}

		Button* buttonSave = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(editMenu), "ButtonSave"));
		if (buttonSave)
		{
			buttonSave->addClickEventListener(CC_CALLBACK_1(CtrlLayer::menuSaveCallback, this));
		}

		// add buttons
		std::string btnAdd[3] = {"ButtonBlueAdd", "ButtonRedAdd", "ButtonYellowAdd"};
		for (int i = 0; i < 3; ++ i)
		{
			Button* btn = static_cast<Button*>
				(Helper::seekWidgetByName(static_cast<Layout*>(editMenu), btnAdd[i]));
			if (btn)
			{
				btn->addClickEventListener(CC_CALLBACK_1(CtrlLayer::menuCountAddCallback, this, i));
			}
		}
		
		// remove buttons
		std::string btnRem[3] = {"ButtonBlueRemove", "ButtonRedRemove", "ButtonYellowRemove"};
		for (int i = 0; i < 3; ++ i)
		{
			Button* btn = static_cast<Button*>
				(Helper::seekWidgetByName(static_cast<Layout*>(editMenu), btnRem[i]));
			if (btn)
			{
				btn->addClickEventListener(CC_CALLBACK_1(CtrlLayer::menuCountRemoveCallback, this, i));
			}
		}
	}
}

void CtrlLayer::showAddItemList( cocos2d::Vec2 pos )
{
	Node* listView = this->getChildByTag(TAG_EDIT_LIST_ADD);
	if (!listView)
		listView = createAddItemList();

	if (!listView)
		return;

	listView->setPosition(pos);
	listView->setVisible(true);
}

void CtrlLayer::hideAddItemList()
{
	Node* listView = this->getChildByTag(TAG_EDIT_LIST_ADD);
	if (listView)
		listView->setVisible(false);
}

cocos2d::Node* CtrlLayer::createAddItemList()
{
	Node* listView = CSLoader::createNode("ListViewVertical.csb");
	if (listView)
	{
		listView->setTag(TAG_EDIT_LIST_ADD);
		listView->setPosition(Vec2(0, 0));
		listView->setVisible(false);
		this->addChild(listView);

		ListView* list = static_cast<ListView*>(
			Helper::seekWidgetByName(static_cast<ListView*>(listView), "ListView"));
		if (list)
		{
			// add enemy ball
			{
				ImageView* image = ImageView::create(BallBase::_textureMap[BallBase::kTypeNormal1]);
				image->setTouchEnabled(true);
				image->addClickEventListener([](Ref*){
					MessageDispatcher::getInstance()->pushMessage(MSG_ADDENEMYITEM);
				});
				list->addChild(image);
			}
			
			// add block box
			{
				ImageView* image = ImageView::create(BlockBox::getTextureFile(0));
				image->setTouchEnabled(true);
				image->addClickEventListener([](Ref*){
					MessageDispatcher::getInstance()->pushMessage(MSG_ADDBLOCK, kBlockBox);
				});
				list->addChild(image);
			}

			// add block wheel
			{
				ImageView* image = ImageView::create(BlockBase::_textureBlock[BlockBase::kBlockTypeWheel]);
				image->setTouchEnabled(true);
				image->addClickEventListener([](Ref*){
					MessageDispatcher::getInstance()->pushMessage(MSG_ADDBLOCK, kBlockWheel);
				});
				list->addChild(image);
			}

			// add block vortex
			{
				ImageView* image = ImageView::create(BlockBase::_textureBlock[BlockBase::kBlockTypeVortex]);
				image->setTouchEnabled(true);
				image->addClickEventListener([](Ref*){
					MessageDispatcher::getInstance()->pushMessage(MSG_ADDBLOCK, kBlockVortex);
				});
				list->addChild(image);
			}

			// add block bomb
			{
				ImageView* image = ImageView::create(BlockBase::_textureBlock[BlockBase::kBlockTypeBomb]);
				image->setTouchEnabled(true);
				image->addClickEventListener([](Ref*){
					MessageDispatcher::getInstance()->pushMessage(MSG_ADDBLOCK, kBlockBomb);
				});
				list->addChild(image);
			}
		}
	}
	return listView;
}

void CtrlLayer::switchAddItemList( cocos2d::Vec2 pos )
{
	Node* listView = this->getChildByTag(TAG_EDIT_LIST_ADD);
	if (listView && listView->isVisible())
		hideAddItemList();
	else
		showAddItemList(pos);
}

void CtrlLayer::setBulletCounts( int blueCount, int redCount, int yellowCount )
{
	_bulletCount[BallBase::kTypeBullet1] = blueCount;
	_bulletCount[BallBase::kTypeBullet2] = redCount;
	_bulletCount[BallBase::kTypeBullet3] = yellowCount;
	updateBulletCount();
}

void CtrlLayer::initSaveDialog( cocos2d::Node* parent )
{
	Node* saveDialog = CSLoader::createNode("EditSave.csb");
	if (saveDialog)
	{
		saveDialog->setTag(TAG_EDIT_SAVE_DLG);
		saveDialog->setPosition(Vec2(0, 0));
		saveDialog->setVisible(false);
		this->addChild(saveDialog);

		Text* txtCancel = static_cast<Text*>(Helper::seekWidgetByName(
			static_cast<Layout*>(saveDialog), "TextCancel"));
		if (txtCancel)
		{
			txtCancel->addClickEventListener([=](Ref* pSender){
				saveDialog->setVisible(false);
			});
		}

		Text* txtOk = static_cast<Text*>(Helper::seekWidgetByName(
			static_cast<Layout*>(saveDialog), "TextOK"));
		if (txtOk)
		{
			txtOk->addClickEventListener([=](Ref* pSender){

				__String sectionIdStr = "";
				TextField* txtSectionId = static_cast<TextField*>(
					Helper::seekWidgetByName(static_cast<Layout*>(saveDialog), "TextFieldSectionId"));
				if (txtSectionId)
				{
					sectionIdStr = txtSectionId->getString();
				}
				int sectionId = sectionIdStr.intValue();

				__String stageIdStr = "";
				TextField* txtStageId = static_cast<TextField*>(
					Helper::seekWidgetByName(static_cast<Layout*>(saveDialog), "TextFieldStageId"));
				if (txtStageId)
				{
					stageIdStr = txtStageId->getString();
				}
				int stageId = stageIdStr.intValue();

				if (sectionId != 0 && stageId != 0)
				{
					MessageDispatcher::getInstance()->pushMessage(MSG_SAVESTAGE, sectionId, stageId);
					saveDialog->setVisible(false);
				}
			});
		}
	}
}
