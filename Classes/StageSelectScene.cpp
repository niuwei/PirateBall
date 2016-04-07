#include "StageSelectScene.h"
#include "GlobalDefined.h"
#include "GlobalData.h"
#include "VisibleRect.h"
#include "extensions/cocos-ext.h"
#include "GameLoader.h"
#include "HelloWorldScene.h"

USING_NS_CC;

Scene* StageSelect::createScene()
{
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = StageSelect::create();

	// add layer as a child to scene
	scene->addChild(layer, 0, TAG_STAGE_SELECT_LAYER);

	// return the scene
	return scene;
}

// on "init" you need to initialize your instance
bool StageSelect::init()
{
	if (!Layer::init())
		return false;

	proInit();

	// Create the page view
	ui::PageView* pageView = ui::PageView::create();
	pageView->setContentSize(Size(512.0f, 576.0f));
	pageView->setPosition(Vec2(VisibleRect::center().x - pageView->getContentSize().width / 2,
		VisibleRect::center().y - pageView->getContentSize().height / 2));

	typedef GameLoader::SectionIdPathMap IdSectionMap;
	IdSectionMap& idPaths = GameLoader::getInstance()->_idPathMap;
	IdSectionMap::iterator it = idPaths.begin();
	for (; it != idPaths.end(); ++ it)
	{
		int sectionId = it->first;
		ui::Layout* layout = ui::Layout::create();
		layout->setContentSize(Size(512.0f, 576.0f));

		ui::ImageView* imageView = ui::ImageView::create("scrollviewbg.png");
		imageView->setScale9Enabled(true);
		imageView->setContentSize(Size(512.0f, 576.0f));
		imageView->setPosition(Vec2(layout->getContentSize().width / 2.0f, layout->getContentSize().height / 2.0f));
		layout->addChild(imageView);

		ui::Text* label = ui::Text::create(StringUtils::format("section %d",(sectionId)), "fonts/Marker Felt.ttf", 30);
		label->setColor(Color3B(192, 192, 192));
		label->setPosition(Vec2(layout->getContentSize().width / 2.0f, 
			layout->getContentSize().height - label->getContentSize().height));
		layout->addChild(label);


		// load section
		SectionLoader sectionLoader;
		GameLoader::getInstance()->loadSectionById(sectionId, &sectionLoader);
		typedef SectionLoader::StageIdPathMap IdStageMap;
		IdStageMap& idStages = sectionLoader._idPathMap;
		IdStageMap::iterator itStage = idStages.begin();

		ui::VBox* outerBox = ui::VBox::create();
		outerBox->setContentSize(Size(512.0f, 500.0f));

		while (itStage != idStages.end())
		{
			ui::HBox* innerBox = ui::HBox::create();
			for (int i = 0; i < 3 && itStage != idStages.end(); ++ i)
			{
				int stageId = itStage->first;
				ui::Button *btn = ui::Button::create(
					"animationbuttonnormal.png",
					"animationbuttonpressed.png",
					"animationbuttondisable.png");
				btn->setTitleText(StringUtils::format("stage %d - %d", sectionId, stageId));
				btn->addClickEventListener( CC_CALLBACK_1(StageSelect::onButtonClicked, this, sectionId, stageId));

				if (GlobalData::getInstance()->_gameMode == GAME_MODE_PLAY)
				{
					int id = GlobalData::getSectionStageId(sectionId, stageId);
					if (_maxNextId != 0 && _maxNextId < id)
					{
						btn->setEnabled(false);
						btn->setBright(false);
					}
				}

				innerBox->addChild(btn);

				++ itStage;
			}
			ui::LinearLayoutParameter *parameter = ui::LinearLayoutParameter::create();
			parameter->setMargin(ui::Margin(0,0,0,100));
			innerBox->setLayoutParameter(parameter);

			outerBox->addChild(innerBox);
		}

		layout->addChild(outerBox);

		pageView->insertPage(layout, sectionId - 1);
	}

	this->addChild(pageView);

	return true;
}

void StageSelect::onButtonClicked( Ref* pSender, int sectionId, int stageId )
{
	ui::Button *btn = (ui::Button*)pSender;
	Layer* pLayer = nullptr;
	Scene* scene = HelloWorld::createScene(&pLayer);
	dynamic_cast<HelloWorld*>(pLayer)->loadStage(sectionId, stageId);
	CCDirector::getInstance()->replaceScene(scene);
}

void StageSelect::proInit()
{
	typedef GameLoader::SectionIdPathMap IdSectionMap;
	IdSectionMap& idPaths = GameLoader::getInstance()->_idPathMap;
	IdSectionMap::iterator it = idPaths.begin();
	for (; it != idPaths.end(); ++ it)
	{
		int sectionId = it->first;

		SectionLoader sectionLoader;
		GameLoader::getInstance()->loadSectionById(sectionId, &sectionLoader);
		typedef SectionLoader::StageIdPathMap IdStageMap;
		IdStageMap& idStages = sectionLoader._idPathMap;
		IdStageMap::iterator itStage = idStages.begin();
		for (; itStage != idStages.end(); ++ itStage)
		{
			int stageId = itStage->first;

			int id = GlobalData::getSectionStageId(sectionId, stageId);
			StageRecordItem item;
			if (!GlobalData::getInstance()->getStageRecord(id, &item))
			{
				GlobalData::getInstance()->setStageRecord(id, 0, 0);
			}
		}
	}
	_maxNextId = GlobalData::getInstance()->getMaxEnabledStageId();
}
