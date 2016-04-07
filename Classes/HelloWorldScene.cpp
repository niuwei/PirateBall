#include "GlobalDefined.h"
#include "GlobalData.h"
#include "EnemyBall.h"
#include "HelloWorldScene.h"
#include "VisibleRect.h"
#include "Bullet.h"
#include <assert.h>
#include "StageLoader.h"
#include "GameLoader.h"
#include "MessageDispatcher.h"
#include "PauseLayer.h"
#include "cocostudio/CocoStudio.h"
#include "StartScene.h"
#include "SimpleAudioEngine.h"
#include "BlockBox.h"
#include "collision.h"
#include "BackgroundLayer.h"
#include "StageOverLayer.h"
#include "EditLayer.h"
#include "ScoreCounter.h"


USING_NS_CC;
USING_NS_CC_EXT;
using namespace ui;


Scene* HelloWorld::createScene(Layer** pLayer)
{
	Layer* layer = nullptr;
	if (GlobalData::getInstance()->isEditMode())
	{
		layer = EditLayer::create();
	}
	else
	{
		layer = HelloWorld::create();
	}
	assert(layer);

#ifdef USE_PhysicsIntegration
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	layer->setPhyWorld(scene->getPhysicsWorld());
#else
	// 'scene' is an autorelease object
	auto scene = Scene::create();
#endif // USE_PhysicsIntegration

    // add layer as a child to scene
    scene->addChild(layer, 0, TAG_CUR_MAIN_LAYER);

	*pLayer = layer;

    // return the scene
    return scene;
}

HelloWorld::HelloWorld()
	: _layout(nullptr)
	, _directLauncher(0.0)
{

}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }

	BackgroundLayer* bkLayer = BackgroundLayer::create();
	this->addChild(bkLayer, kZorderBackground, TAG_BACKGOURND_LAYER);

	auto menuUseLine = MenuItemImage::create("btn-use-line.png",
		"btn-use-line.png", [&](Ref* pSender){
		_isShowSightLine = true;
	});
	menuUseLine->setPosition(Vec2(
		VisibleRect::rightBottom().x - menuUseLine->getContentSize().width,
		menuUseLine->getContentSize().height));
	auto menu2 = Menu::create(menuUseLine, NULL);
	menu2->setPosition(Vec2::ZERO);
	this->addChild(menu2, 100);

	Node* node = CSLoader::createNode("PlayScene.csb");
	if (node)
	{
		Node* child = node->getChildByName("PanelMain");
		if (child)
		{
			child->removeFromParent();
			_layout = static_cast<ui::Layout*>(child);
			_layout->setVisible(true);
			_layout->setPosition(Vec2::ZERO);
			this->addChild(_layout);
			_layout->addTouchEventListener(CC_CALLBACK_2(HelloWorld::onTouchLayoutCallback, this));

			initTopBar(_layout);
			initBulletMenu(_layout);
			initLauncer(_layout, bkLayer->getLauncherPos());
		}
	}

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	_sectionId = 0;
	clear();
    return true;
}

void HelloWorld::initTopBar( cocos2d::Node* parent )
{
	assert(parent);
	Node* topBar = CSLoader::createNode("TopBar.csb");
	if (topBar)
	{
		topBar->setPosition(Vec2::ZERO);
		topBar->setTag(TAG_TOPBAR);
		parent->addChild(topBar);

		Button* buttonPause = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(topBar), "ButtonPause"));
		if (buttonPause)
		{
			buttonPause->addClickEventListener([&](Ref*) {

				PauseLayer* pauseLayer = static_cast<PauseLayer*>(getChildByTag(TAG_PAUSE_LAYER));
				if (!pauseLayer)
				{
					pauseLayer = PauseLayer::create();
					pauseLayer->setTag(TAG_PAUSE_LAYER);
					pauseLayer->setPosition(Vec2::ZERO);
					addChild(pauseLayer);
				}
				assert(pauseLayer);
				pauseLayer->setVisible(true);
				Director::getInstance()->pause();
			});
		}
	}
}

void HelloWorld::initBulletMenu(Node* parent)
{
	assert(parent);
	Node* bulletMenu = CSLoader::createNode("BulletMenu.csb");
	if (bulletMenu)
	{
		bulletMenu->setPosition(Vec2::ZERO);
		bulletMenu->setTag(TAG_BULLET_MENU);
		parent->addChild(bulletMenu);

		Button* buttonBlue = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(bulletMenu), "ButtonBlue"));
		if (buttonBlue)
		{
			buttonBlue->addClickEventListener([=](Ref*){
				prepareBullet(BallBase::kTypeBullet1);
			});
		}

		Button* buttonRed = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(bulletMenu), "ButtonRed"));
		if (buttonRed)
		{
			buttonRed->addClickEventListener([=](Ref*){
				prepareBullet(BallBase::kTypeBullet2);
			});
		}

		Button* buttonYellow = static_cast<Button*>(
			Helper::seekWidgetByName(static_cast<Layout*>(bulletMenu), "ButtonYellow"));
		if (buttonYellow)
		{
			buttonYellow->addClickEventListener([=](Ref*){
				prepareBullet(BallBase::kTypeBullet3);
			});
		}
	}
}

void HelloWorld::initLauncer( cocos2d::Node* parent, cocos2d::Vec2 pos )
{
	Node* spriteLauncher = CSLoader::createNode("Launcher.csb");
	if (spriteLauncher)
	{
		spriteLauncher->setPosition(pos);
		spriteLauncher->setTag(TAG_LAUNCHER);
		parent->addChild(spriteLauncher);

		ImageView* imgLauncher = static_cast<ImageView*>(
			Helper::seekWidgetByName(static_cast<Widget*>(spriteLauncher), "ImageLauncher"));
		if (imgLauncher)
		{
			imgLauncher->addClickEventListener([=](Ref*){
				shoot();
			});
		}
	}
}

void HelloWorld::updataStageText()
{
	if (_layout)
	{
		Layout* topBar = static_cast<Layout*>(_layout->getChildByTag(TAG_TOPBAR));
		if (topBar)
		{
			Text* txtStage = static_cast<Text*>(Helper::seekWidgetByName(topBar, "TextStage"));
			if (txtStage)
			{
				std::string strStage = StringUtils::format("Stage %02d-%02d", _sectionId, _stageId);
				txtStage->setString(strStage);
			}
			
		}
	}
}

void HelloWorld::updateScore()
{
	if (_layout)
	{
		Layout* topBar = static_cast<Layout*>(_layout->getChildByTag(TAG_TOPBAR));
		if (topBar)
		{
			Text* txtScore = static_cast<Text*>(Helper::seekWidgetByName(topBar, "TextScore"));
			if (txtScore)
			{
				unsigned int score = ScoreCounter::getInstance()->getScore();
				std::string strScore = StringUtils::format("Score: %d", score);
				txtScore->setString(strScore);
			}
		}
	}
}

void HelloWorld::updataTimeText(bool* pStart, bool isReset)
{
	if (_isStageOvered)
		return;
	
	static bool s_start = false;
	static unsigned int s_frames = 0;
	static unsigned int s_min = 0;
	static unsigned int s_sec = 0;

	bool timeChanged = false;
	if (isReset)
	{
		s_min = s_sec = 0;
		timeChanged = true;
	}

	if (pStart != NULL)
		s_start = *pStart;
	
	if (s_start)
	{
		if (s_frames == 0)
			s_frames = Director::getInstance()->getTotalFrames();
		else
		{
			unsigned int curFrame = Director::getInstance()->getTotalFrames();
			unsigned int during = curFrame - s_frames;
			if (during >= 60)
			{
				s_sec += during / 60;
				_spendTimeSecond = s_sec;
				if (s_sec >= 60)
				{
					s_min += s_sec / 60;
					s_sec %= 60;
				}
				s_frames = curFrame - (during % 60);
				timeChanged = true;
			}
		}
	}
	else
	{
		s_frames = 0;
	}
	
	if (timeChanged && _layout)
	{
		if (_layout)
		{
			Layout* topBar = static_cast<Layout*>(_layout->getChildByTag(TAG_TOPBAR));
			if (topBar)
			{
				Text* txtTime = static_cast<Text*>(Helper::seekWidgetByName(topBar, "TextTime"));
				if (txtTime)
				{
					std::string strTime = StringUtils::format("%02d:%02d", s_min, s_sec);
					txtTime->setString(strTime);
				}

			}
		}
	}
}

void HelloWorld::updateBulletCount()
{
	assert(_layout);
	if (_layout)
	{
		Widget* bulletMeun = static_cast<Widget*>(_layout->getChildByTag(TAG_BULLET_MENU));
		if (bulletMeun)
		{
			ui::Text* txtBlue = static_cast<ui::Text*>(ui::Helper::seekWidgetByName(bulletMeun, "TextBlue"));
			if (txtBlue)
			{
				std::string label_str = StringUtils::format("%d", _bulletCounter[BallBase::kTypeBullet1]);
				txtBlue->setString(label_str);
			}

			ui::Text* txtRed = static_cast<ui::Text*>(ui::Helper::seekWidgetByName(bulletMeun, "TextRed"));
			if (txtRed)
			{
				std::string label_str = StringUtils::format("%d", _bulletCounter[BallBase::kTypeBullet2]);
				txtRed->setString(label_str);
			}

			ui::Text* txtYellow = static_cast<ui::Text*>(ui::Helper::seekWidgetByName(bulletMeun, "TextYellow"));
			if (txtYellow)
			{
				std::string label_str = StringUtils::format("%d", _bulletCounter[BallBase::kTypeBullet3]);
				txtYellow->setString(label_str);
			}
		}
	}
	
	prepareBullet(BallBase::kTypeBulletUnknown);
}

void HelloWorld::prepareBullet( int type )
{
	if (_bulletCounter[type] != 0 ||
		type == BallBase::kTypeBulletUnknown)
	{
		_bulletType = type;

		if (_layout)
		{
			Widget* spriteLauncher = static_cast<Widget*>(_layout->getChildByTag(TAG_LAUNCHER));
			if (spriteLauncher)
			{
				ImageView* imgBullet = static_cast<ImageView*>(Helper::seekWidgetByName(spriteLauncher, "ImageSelected"));
				if (imgBullet)
				{
					imgBullet->loadTexture(BallBase::_textureMap[type]);
				}
			}
		}
	}
}

void HelloWorld::setLuncherDirection( float angle )
{
	if (_layout)
	{
		Sprite* spriteLauncher = static_cast<Sprite*>(_layout->getChildByTag(TAG_LAUNCHER));
		if (spriteLauncher)
		{
			_directLauncher = angle;
			spriteLauncher->setRotation(angle);
		}
	}
}

float HelloWorld::getLuncherDirection()
{
	return _directLauncher;
}

void HelloWorld::clear()
{
	_nItemTagBase = TAG_MOVING_ITEM_BASE;
	_stageId = 0;
	_spendTimeSecond = 0;
	_isStageOvered = false;
	
	ScoreCounter::getInstance()->reset();

	ItemTagList::iterator it = _itemTags.begin();
	for (; it != _itemTags.end();)
	{
		int tag = *it;
		removeChildByTag(tag);
		it = _itemTags.erase(it);
	}
	
	it = _blockTags.begin();
	for (; it != _blockTags.end();)
	{
		int tag = *it;
		removeChildByTag(tag);
		it = _blockTags.erase(it);
	}

	for (int i = 0; i < BallBase::kTypeBulletUnknown; ++ i)
	{
		_bulletCounter[i] = 2;
	}
	_directLauncher = 0.0;
	_bulletType = BallBase::kTypeBulletUnknown;

	_isShowSightLine = false;
	DrawNode* node = static_cast<DrawNode*>(getChildByTag(TAG_ARM_LINE));
	if (node != nullptr)
	{
		node->clear();
	}
	
	setLuncherDirection(0.0);
}

void HelloWorld::onEnter()
{
	Layer::onEnter();
	schedule(schedule_selector(HelloWorld::update)); //添加刷新函数

#ifdef USE_PhysicsIntegration
	auto node = Node::create();
	node->setPhysicsBody(PhysicsBody::createEdgeBox(VisibleRect::getVisibleRect().size));
	node->setPosition(VisibleRect::center());
	this->addChild(node);

	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(HelloWorld::onContactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
#endif // USE_PhysicsIntegration
/*
	auto sprite = Sprite::create("block.png");
	sprite->setTag(1010);
	auto body = PhysicsBody::createBox(sprite->getContentSize(), PhysicsMaterial());
	body->setDynamic(false);
	body->setGravityEnable(false);
	const Vector<PhysicsShape*>& shapes = body->getShapes();
	for (auto it : shapes)
	{
		it->setRestitution(1.0f);
	}
	sprite->setPhysicsBody(body);
	sprite->setPosition(Vec2(300, 300));
	this->addChild(sprite);

	auto sprite2 = Sprite::create("block.png");
	sprite2->setTag(1011);
	auto body2 = PhysicsBody::createCircle(sprite2->getContentSize().width / 2, PhysicsMaterial());
	body2->setGravityEnable(false);
	body2->setVelocity(Vec2(0.0, -400.0));
//	body2->getShape(0)->setRestitution(1.0f);
	const Vector<PhysicsShape*>& shapes2 = body2->getShapes();
	for (auto it : shapes2)
	{
		it->setRestitution(1.0f);
	}
	sprite2->setPhysicsBody(body2);
	sprite2->setPosition(Vec2(310, 800));
	this->addChild(sprite2);
*/
}

void HelloWorld::onExit()
{
	unschedule(schedule_selector(HelloWorld::update));//删除刷新
	Layer::onExit();
}

void HelloWorld::newStage()
{
	StageLoader loader;
	loader._stageId = _stageId + 1;
	buildStage(loader);
}

// bool HelloWorld::onTouchBegan( Touch *touch, Event *unused_event )
// {
// 	EnemyBall::changeSelected(0);
// 	return true;
// }
// 
// void HelloWorld::onTouchMoved( Touch *touch, Event *unused_event )
// {
// 
// }
// 
// void HelloWorld::onTouchEnded( Touch *touch, Event *unused_event )
// {
// 	
// }

// void HelloWorld::Active()
// {
// 	if (!_isActive)
// 	{
// 		_isActive = true;
// 		auto listener = EventListenerTouchOneByOne::create();
// 		listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
// 		listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
// 		listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
// 		listener->setSwallowTouches(true);
// 		_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
// 	}
// }

// void HelloWorld::Inactive()
// {
// 	if (_isActive)
// 	{
// 		_isActive = false;
// 		_eventDispatcher->removeEventListenersForTarget(this);
// 	}
// }

// void HelloWorld::draw( Renderer *renderer, const Mat4 &transform, uint32_t flags )
// {
// 	_customCommand.init(_globalZOrder);
// 	_customCommand.func = CC_CALLBACK_0(HelloWorld::onDraw, this, transform, flags);
// 	renderer->addCommand(&_customCommand);
// }
// 
// void HelloWorld::onDraw( const cocos2d::Mat4 &transform, uint32_t flags )
// {
// 	for (auto& it : _itemTags)
// 	{
// 		EnemyBall* pBall = dynamic_cast<EnemyBall*>(this->getChildByTag(it));
// 		if (pBall)
// 		{
// 			if (pBall->isSelected())
// 			{
// 				Vec2 pos = pBall->getPosition();
// 				Size s = pBall->getContentSize();
// 
// 				Vec2 v1 = pos - s / 2;
// 				Vec2 v2 = pos + s / 2;
// 				DrawPrimitives::drawRect(v1, v2);
// 				break;
// 			}
// 		}
// 	}
// }

void HelloWorld::onUpdate( float dt )
{	
	updataTimeText();

#ifndef USE_PhysicsIntegration
	moveOnlyBullet();
#endif // !USE_PhysicsIntegration

	if (_isShowSightLine)
	{
		virtualBulletMove();
	}

	// delete bullet out of visible rect
	checkBulletOutScreen();

	if (!_isStageOvered)
	{
		bool isSuccess;
		_isStageOvered = checkStageOver(isSuccess);
		if (_isStageOvered)
		{
			StageOverParams params;
			params._isSucceed = isSuccess;
			params._timeSpent = _spendTimeSecond;
			ScoreCounter::getInstance()->onStageOver(params);
			onStageOver(isSuccess);
		}
	}
}

void HelloWorld::showStageOverLayer( bool isSuccess )
{
	Layer* stageOver = static_cast<Layer*>(getChildByTag(TAG_STAGE_OVER_LAYER));
	if (!stageOver)
	{
		stageOver = StageOverLayer::create(isSuccess);
		addChild(stageOver, kZorderStageOverLayer, TAG_STAGE_OVER_LAYER);
	}
}

void HelloWorld::update( float dt )
{
//	checkLongPressItem();
	HandleMessage();
	onUpdate(dt);
}

void HelloWorld::virtualBulletMove()
{
	enum  { FRAME_COUNT = 80 };
	static float last_angle = 180;
	float directLauncher = getLuncherDirection();
	if (last_angle != directLauncher)
	{
		last_angle = directLauncher;

		Vec2 speed = BallBase::calcSpeed(BULLET_SPEED_VALUE, getLuncherDirection());
		Vec2 bulletPos = getLuncherShootPos();
		Vec2 positions[FRAME_COUNT];
		for (int i = 0; i < FRAME_COUNT * BULLET_SPEED_VALUE; ++ i)
		{
			bulletPos += (speed / BULLET_SPEED_VALUE);
			if (i % BULLET_SPEED_VALUE == 0)
				positions[i / BULLET_SPEED_VALUE] = bulletPos;

			// block
			std::vector<Vec2> speeds;
			for (auto& it : _blockTags)
			{
				BlockBase* pBlock = dynamic_cast<BlockBase*>(this->getChildByTag(it));
				if (pBlock)
				{
					BallCollider ball(bulletPos, DEFAULT_BALL_RADIUS, speed);
					if (pBlock->checkCollision(&ball))
					{
						if (pBlock->canBeCollided())
						{
							pBlock->setCanBeCollided(false);
							// 只对box类型的block做多对象碰撞合成
							if (pBlock->_type == BlockBase::kBlockTypeBox)
							{
								speeds.push_back(ball._speed);
							}
							else
							{
								speed = ball._speed;
							}
						}
					}
					else
					{
						if (!pBlock->canBeCollided())
							pBlock->setCanBeCollided(true);
					}
				}
			}

			if (!speeds.empty())
			{
				Vec2 newSpeed = Vec2::ZERO;
				std::vector<Vec2>::iterator it = speeds.begin();
				for (; it != speeds.end(); ++ it)
				{
					newSpeed += *it;
				}
				newSpeed.normalize();
				newSpeed *= BULLET_SPEED_VALUE;
				speed = newSpeed;

			}

			ItemTagList::iterator it = _itemTags.begin();
			for (; it != _itemTags.end(); ++ it)
			{
				EnemyBall* pBall = dynamic_cast<EnemyBall*>(this->getChildByTag(*it));
				if (pBall)
				{

					BallCollider ball1(bulletPos, DEFAULT_BALL_RADIUS, speed);
					BallCollider ball2(pBall->getPosition(), pBall->getRadius(), pBall->getSpeed());
					if (ball1.isCollision(&ball2))
					{
						if (pBall->canBeCollided())
						{
							speed = ball1._speed;
							pBall->setCanBeCollided(false);
							break;
						}
					}
					else
					{
						if (!pBall->canBeCollided())
							pBall->setCanBeCollided(true);
					}
				}
			}
		}

		DrawNode* node = static_cast<DrawNode*>(getChildByTag(TAG_ARM_LINE));
		if (node == nullptr)
		{
			node = DrawNode::create();
			node->setTag(TAG_ARM_LINE);
			addChild(node);
		}
		node->clear();
		node->drawPoints(positions, FRAME_COUNT, 4.0, Color4F::RED);
		node->setVisible(true);
	}
}

void HelloWorld::moveOnlyBullet()
{
	Bullet* pBullet = dynamic_cast<Bullet*>(this->getChildByTag(TAG_BULLET));
	if (pBullet)
	{
		for (int i = 0; i < BULLET_SPEED_VALUE; ++ i)
		{
			Vec2 speed = pBullet->getSpeed();
			Vec2 pos = pBullet->getPosition();
			pos += speed / BULLET_SPEED_VALUE;
			pBullet->setPosition(pos);

			// --
			// box的接触点，同时碰触两个顶点，取向量夹角中心
			// 设置控件状态，避免两次连续碰撞，就是没出来接着碰上的情况
			// --
			if (!pBullet->canBeCollided())
				continue;

			std::vector<Vec2> speeds;
			for (auto& it : _blockTags)
			{
				BlockBase* pBlock = dynamic_cast<BlockBase*>(this->getChildByTag(it));
				if (pBlock)
				{
					BallCollider ball(pBullet->getPosition(), pBullet->getRadius(), pBullet->getSpeed());
					if (pBlock->checkCollision(&ball))
					{
						if (pBlock->canBeCollided())
						{
							pBlock->setCanBeCollided(false);
							// 只对box类型的block做多对象碰撞合成
							if (pBlock->_type == BlockBase::kBlockTypeBox)
							{
								speeds.push_back(ball._speed);
							}
							else
							{
								pBullet->setSpeed(ball._speed);
							}
							pBlock->onCollision(pBullet);
						}
					}
					else
					{
						if (!pBlock->canBeCollided())
							pBlock->setCanBeCollided(true);
					}
				}
			}

			if (!speeds.empty())
			{
				Vec2 newSpeed = Vec2::ZERO;
				std::vector<Vec2>::iterator it = speeds.begin();
				for (; it != speeds.end(); ++ it)
				{
					newSpeed += *it;
				}
				newSpeed.normalize();
				newSpeed *= pBullet->getSpeedValue();
				pBullet->setSpeed(newSpeed);
			}

			ItemTagList::iterator it = _itemTags.begin();
			for (; it != _itemTags.end(); )
			{
				EnemyBall* pBall = dynamic_cast<EnemyBall*>(this->getChildByTag(*it));
				if (pBall)
				{
					assert((void*)pBall != (void*)pBullet);

					BallCollider ball1(pBullet->getPosition(), pBullet->getRadius(), pBullet->getSpeed());
					BallCollider ball2(pBall->getPosition(), pBall->getRadius(), pBall->getSpeed());
					if (ball1.isCollision(&ball2))
					{
//						speeds.push_back(ball1._speed);
						if (pBall->canBeCollided())
						{
						
						
						pBullet->setSpeed(ball1._speed);
						pBall->setCanBeCollided(false);
						if (pBall->onCollision(pBullet->_type))
						{
							it = _itemTags.erase(it);
//							continue;
						}
						break;
						}
					}
					else
					{
						if (!pBall->canBeCollided())
							pBall->setCanBeCollided(true);
					}
				}
				++ it;
			}

			

// 			checkBlockCollision();
// 			checkCollision();
		}
	}
}

EnemyBall* HelloWorld::createEnemy( int type )
{
	return EnemyBall::create((BallBase::BallType)type);
}

BlockBox* HelloWorld::createBlock( int type )
{
	BlockBox* sprite1 = new(std::nothrow) BlockBox();
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

BlockBase* HelloWorld::createWheel()
{
	BlockBase* sprite1 = new(std::nothrow) BlockWheel();
	if (!sprite1 || !sprite1->initWithFile(""))
	{
		delete sprite1;
		sprite1 = nullptr;
	}
	else
	{
		sprite1->autorelease();
		sprite1->runAction(RepeatForever::create(RotateBy::create(4, 180)));
	}
	return sprite1;
}

BlockBase* HelloWorld::createVortex()
{
	BlockBase* sprite1 = new(std::nothrow) BlockVortex();
	if (!sprite1 || !sprite1->initWithFile(""))
	{
		delete sprite1;
		sprite1 = nullptr;
	}
	else
	{
		sprite1->autorelease();
		sprite1->runAction(RepeatForever::create(RotateBy::create(8, 180)));
	}
	return sprite1;
}

BlockBase* HelloWorld::createBomb()
{
	BlockBase* sprite1 = new(std::nothrow) BlockBomb();
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

void HelloWorld::addEnemy( int type, const cocos2d::Vec2& pos, int ringType )
{
	if (type < BallBase::kTypeNormal1)
		type += (BallBase::kTypeNormal1 - BallBase::kTypeBullet1);
	
	EnemyBall* sprite1 = createEnemy(type);
	if (!sprite1)
		return;

	sprite1->setPosition(pos);
	int tag = _nItemTagBase ++;
	sprite1->setTag(tag);

#ifdef USE_PhysicsIntegration
	auto body = PhysicsBody::createCircle(sprite1->getRadius(), PhysicsMaterial());
	body->setDynamic(false);
	body->setGravityEnable(false);
	const Vector<PhysicsShape*>& shapes = body->getShapes();
	for (auto it : shapes)
	{
		it->setRestitution(1.0f);
	}
	sprite1->setPhysicsBody(body);
#endif // USE_PhysicsIntegration

	sprite1->addRing(ringType);
	this->addChild(sprite1);
	_itemTags.push_back(tag);
}

void HelloWorld::addBlock( const cocos2d::Vec2& pos, int typeBox )
{
	BlockBox* sprite1 = createBlock(typeBox);
	assert(sprite1);
	sprite1->setPosition(pos);
	int tag = _nItemTagBase ++;
	sprite1->setTag(tag);
	this->addChild(sprite1);
	_blockTags.push_back(tag);
}

void HelloWorld::addWheel( const cocos2d::Vec2& pos )
{
	BlockBase* sprite1 = createWheel();
	assert(sprite1);
	sprite1->setPosition(pos);
	int tag = _nItemTagBase ++;
	sprite1->setTag(tag);
	this->addChild(sprite1);
	_blockTags.push_back(tag);
}

void HelloWorld::addVortex( const cocos2d::Vec2& pos )
{
	BlockBase* sprite1 = createVortex();
	assert(sprite1);
	sprite1->setPosition(pos);
	int tag = _nItemTagBase ++;
	sprite1->setTag(tag);
	this->addChild(sprite1);
	_blockTags.push_back(tag);
}

void HelloWorld::addBomb( const cocos2d::Vec2& pos )
{
	BlockBase* sprite1 = createBomb();
	assert(sprite1);
	sprite1->setPosition(pos);
	int tag = _nItemTagBase ++;
	sprite1->setTag(tag);
	this->addChild(sprite1);
	_blockTags.push_back(tag);
}


// void HelloWorld::addSlider()
// {
// 	Label* displayValueLabel = Label::createWithTTF("value = 0", "fonts/Marker Felt.ttf", 32);
// 	displayValueLabel->setPosition(VisibleRect::bottom() + Vec2(0, 20));
// 	addChild(displayValueLabel, 0, TAG_SLIDER_LABEL);
// 
// 	// Add the slider
// 	ControlSlider *slider = ControlSlider::create("sliderTrack.png","sliderProgress.png" ,"sliderThumb.png");
// 	slider->setAnchorPoint(Vec2(0.5f, 1.0f));
// 	slider->setMinimumValue(-90.0f); // Sets the min value of range
// 	slider->setMaximumValue(90.0f); // Sets the max value of range
// 	slider->setValue(0.0f);
// 	slider->setPosition(VisibleRect::bottom() + Vec2(0, 60));
// 	slider->setTag(TAG_SLIDER);
// 	slider->addTargetWithActionForControlEvents(this, 
// 		cccontrol_selector(HelloWorld::valueChanged), 
// 		Control::EventType::VALUE_CHANGED);
// 	this->addChild(slider);
// }

// void HelloWorld::valueChanged( Ref *sender, Control::EventType controlEvent )
// {
// 	ControlSlider* pSlider = (ControlSlider*)sender;
// 	float value = pSlider->getValue();
// 	// Change value of label.
// 	Label* valueLabel = dynamic_cast<Label*>(getChildByTag(TAG_SLIDER_LABEL));
// 	if (valueLabel)
// 	{
// 		valueLabel->setString(String::createWithFormat("value = %.02f", value)->getCString());
// 	}
// 
// 	setLuncherDirection(value);
// }

bool HelloWorld::checkBulletOutScreen()
{
	Bullet* pBall = dynamic_cast<Bullet*>(this->getChildByTag(TAG_BULLET));
	if (pBall)
	{
		Vec2 pos = pBall->getPosition();
		Rect rtView = VisibleRect::getVisibleRect();
		if (!rtView.containsPoint(pos))
		{
			removeChild(pBall);
			return true;
		}
	}
	return false;
}

bool HelloWorld::checkCollision()
{
	Bullet* pBullet = dynamic_cast<Bullet*>(this->getChildByTag(TAG_BULLET));
	if (pBullet == nullptr || !pBullet->canBeCollided())
		return false;

	ItemTagList::iterator it = _itemTags.begin();
	for (; it != _itemTags.end(); ++ it)
	{
		EnemyBall* pBall = dynamic_cast<EnemyBall*>(this->getChildByTag(*it));
		if (pBall)
		{
			assert((void*)pBall != (void*)pBullet);

			BallCollider ball1(pBullet->getPosition(), pBullet->getRadius(), pBullet->getSpeed());
			BallCollider ball2(pBall->getPosition(), pBall->getRadius(), pBall->getSpeed());
			if (ball1.isCollision(&ball2))
			{
				pBullet->setSpeed(ball1._speed);
				if (pBall->onCollision(pBullet->_type))
				{
					_itemTags.erase(it);
				}
				return true;
			}
		}
	}
	return false;
}

bool HelloWorld::checkBlockCollision()
{
	Bullet* pBullet = dynamic_cast<Bullet*>(this->getChildByTag(TAG_BULLET));
	if (pBullet == nullptr || !pBullet->canBeCollided())
		return false;

	for (auto& it : _blockTags)
	{
		BlockBase* pBlock = dynamic_cast<BlockBase*>(this->getChildByTag(it));

		BallCollider ball(pBullet->getPosition(), pBullet->getRadius(), pBullet->getSpeed());
		if (pBlock->checkCollision(&ball))
		{
			if (pBlock->canBeCollided())
			{
				pBlock->setCanBeCollided(false);
				pBullet->setSpeed(ball._speed);
				pBlock->onCollision(pBullet);
				return true;
			}
		}
		else
		{
			pBlock->setCanBeCollided(true);
		}
	}
	return false;
}

bool HelloWorld::loadStage( int sectionId, int stageId )
{
	_sectionId = sectionId;
	_stageId = stageId;

	StageLoader stageLoader;
	if (GameLoader::getInstance()->loadStage(sectionId, stageId, &stageLoader))
	{
		buildStage(stageLoader);
		return true;
	}
	
	return false;
}

void HelloWorld::loadStageFromFile( const std::string& filepath )
{
	StageLoader loader;
	if (loader.loadStageFromFile(filepath))
	{
		buildStage(loader);
	}
}

void HelloWorld::buildStage( StageLoader& loader )
{
	clear();
	assert(loader._stageId != 0);
	_stageId = loader._stageId;

	for (auto& it : loader._enemys)
	{
		addEnemy(it._type, it._pos, it._ringType);
	}

	for (auto& it : loader._blocks)
	{
		switch (it._type)
		{
		case BlockBase::kBlockTypeBox:
			addBlock(it._pos, it._ringType);
			break;

		case BlockBase::kBlockTypeWheel:
			addWheel(it._pos);
			break;

		case BlockVortex::kBlockTypeVortex:
			addVortex(it._pos);
			break;

		case BlockVortex::kBlockTypeBomb:
			addBomb(it._pos);
			break;

		default:
			break;
		}
		
	}

	for (int i = 0; i < BallBase::kTypeBulletUnknown; ++ i)
	{
		_bulletCounter[i] = loader._bullets[i];
		updateBulletCount();
	}

	updataStageText();
	bool start = true;
	updataTimeText(&start, true);
}

void HelloWorld::HandleMessage()
{
	Message Msg;
	while (MessageDispatcher::getInstance()->popMessage(Msg))
	{
		switch (Msg.type)
		{
		case MSG_NEWSTAGE:
			newStage();
			break;

		case MSG_LOADSTAGE:
			{
				int stageId = _stageId;
				int sectionId = _sectionId;
				
				if (Msg.param1 == 0) // step by param2 from current stage.
				{
					int stepCount = Msg.param2;
					assert(stepCount >= 0);
					while (stepCount)
					{
						int nextId = GlobalData::getInstance()->getNextSectionStageId(sectionId, stageId);
						if (nextId != 0)
						{
							sectionId = nextId / 1000;
							stageId = nextId % 1000;
						}
						else
						{
							assert(0);
							break;
						}
						stepCount --;
					}
				}
				else// if this params isn't 0, load stage id by this number.
				{
					stageId = Msg.param1;
					if (Msg.param2 != 0)
					{
						sectionId = Msg.param2;
					}
				}

				loadStage(sectionId, stageId);
			}
			break;

// 		case MSG_LEVELCLEARED:
// 			{
// 				onStageOver(true);
// 			}
// 			break;

		case MSG_LOADSTAGEFILE:
			{
				std::string* pStr = (std::string*)Msg.param1;
				loadStageFromFile(*pStr);
				delete pStr;
			}
			break;

		case MSG_DESTROYBULLET:
			{
				Bullet* pBullet = dynamic_cast<Bullet*>(this->getChildByTag(TAG_BULLET));
				if (pBullet)
				{
					pBullet->removeFromParent();
				}
			}
			break;

		default:
			onMessage(Msg.type, Msg.param1, Msg.param2);
			break;
		}
	}
}

void HelloWorld::checkLongPressItem()
{
// 	for (auto& it : _itemTags)
// 	{
// 		EnemyBall* pBall = dynamic_cast<EnemyBall*>(this->getChildByTag(it));
// 		if (pBall)
// 		{
// 			if (pBall->isSelected())
// 			{
// 				if (pBall->isLongPressed(LONG_PRESS_INTERVAL))
// 				{
// 					showPropertyLayer(pBall->getPosition(), pBall->_type);
// 				}
// 				break;
// 			}
// 		}
// 	}
}

void HelloWorld::shoot()
{
	if (_bulletType >= BallBase::kTypeBulletUnknown)
		return;
	
	Bullet* pBullet = dynamic_cast<Bullet*>(this->getChildByTag(TAG_BULLET));
	if (pBullet == nullptr)
	{
		pBullet = Bullet::create((BallBase::BallType)_bulletType);
		pBullet->setPosition(getLuncherShootPos());
		pBullet->setTag(TAG_BULLET);
		pBullet->setAngle(getLuncherDirection());

#ifdef USE_PhysicsIntegration
		auto body = PhysicsBody::createCircle(pBullet->getRadius(), PhysicsMaterial());
		body->setGravityEnable(false);
		const Vector<PhysicsShape*>& shapes = body->getShapes();
		for (auto it : shapes)
		{
			it->setRestitution(1.0f);
		}
		body->setVelocity(pBullet->getSpeed() * 60);
		pBullet->setPhysicsBody(body);
#endif // USE_PhysicsIntegration

		this->addChild(pBullet);
		_bulletCounter[_bulletType] --;
		updateBulletCount();
		prepareBullet(BallBase::kTypeBulletUnknown);
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("effect01.mp3");
		ScoreCounter::getInstance()->onBulletShoot();
//		int _audioID = experimental::AudioEngine::play2d("effect01.mp3");
	}
}

void HelloWorld::onTouchLayoutCallback( Ref* sender, ui::Widget::TouchEventType type )
{
	static Vec2 last_pos = Vec2::ZERO;
	static std::list<pair<unsigned int, float> >	frames_angle;
	unsigned int stat_frames = 6;
	switch (type)
	{
	case ui::Widget::TouchEventType::BEGAN:
		{
			GlobalData::getInstance()->setSelectedTag(0);
			last_pos = Vec2::ZERO;
		}
		break;

	case ui::Widget::TouchEventType::MOVED:
		{
			Vec2 pos = _layout->getTouchMovePosition();
			if (!last_pos.isZero())
			{
				// 离发射越近，滑动转的角度越大
				unsigned int p = static_cast<unsigned int>(last_pos.y / 100);
				float angle = getLuncherDirection() + (pos.x - last_pos.x) / (2 + p);
				if (angle > 90.0)
					angle = 90.0;
				else if (angle < -90.0)
					angle = -90.0;
				setLuncherDirection(angle);
				
				// 为了防止手指触控时产生抖动导致对不准
				// 保存触控结束前的10帧内的angle
				// 在触控结束时恢复10帧内的角度值
				unsigned int cur_frames = Director::getInstance()->getTotalFrames();
 				frames_angle.push_front(make_pair(cur_frames, angle));
				while (frames_angle.size() > stat_frames)
				{
					frames_angle.pop_back();
				}
			}
			last_pos = pos;
		}
		break;

	case ui::Widget::TouchEventType::ENDED:
		{
			// 查找保存的处于10帧内角度，用于恢复，防止手指离开触屏时的抖动
			float prevate_finger_shark_angle = 0;
			unsigned int cur_frames = Director::getInstance()->getTotalFrames();
			std::list<pair<unsigned int, float> >::iterator it = frames_angle.begin();
			for (; it != frames_angle.end(); ++ it)
			{
				if (cur_frames - it->first > stat_frames)
					break;

				prevate_finger_shark_angle = it->second;
			}
			
			if (prevate_finger_shark_angle != 0)
			{
				setLuncherDirection(prevate_finger_shark_angle);
			}
		}
		break;

	case ui::Widget::TouchEventType::CANCELED:
		{

		}
		break;

	default:
		break;
	}
}

bool HelloWorld::checkStageOver( bool& isSuccess )
{
	// no bullet activity
	Bullet* pBullet = dynamic_cast<Bullet*>(this->getChildByTag(TAG_BULLET));
	if (pBullet != nullptr)
		return false;

	// no bullet prepared, stage over
	bool hasBullet = false;
	for (int i = 0; i < BallBase::kTypeBulletUnknown; ++ i)
	{
		if (_bulletCounter[i] != 0)
		{
			hasBullet = true;
			break;
		}
	}

	// no enemy, stage over
	bool hasEnemy = false;
	for (auto& it : _itemTags)
	{
		EnemyBall* pBall = dynamic_cast<EnemyBall*>(this->getChildByTag(it));
		if (pBall)
		{
			assert((void*)pBall != (void*)pBullet);
			hasEnemy = true;
			break;
		}
	}
	
	bool stageOver = !hasEnemy || !hasBullet;
	isSuccess = !hasEnemy;
	return stageOver;
}

void HelloWorld::onStageOver( bool isSuccess )
{
	if (isSuccess)
	{
		_repeatCount = 0;
		this->schedule(schedule_selector(HelloWorld::onTimerCheckBulletLeave), 1.0);
	}
	else
	{
		showStageOverLayer(false);
	}
}

void HelloWorld::onTimerCheckBulletLeave( float dt )
{
	Vec2 pos[3] = {Vec2(42.0, 212.0), Vec2(114.0, 225.0), Vec2(190.0, 236.0)};
	if (_repeatCount == 3)
	{
		// record completed
		int curId = GlobalData::getSectionStageId(_sectionId, _stageId);
		GlobalData::getInstance()->setStageRecord(curId, _spendTimeSecond, 
			ScoreCounter::getInstance()->getScore());
		showStageOverLayer(true);
		this->unschedule(schedule_selector(HelloWorld::onTimerCheckBulletLeave));
	}
	else if (_repeatCount < 3)
	{
		ScoreCounter::getInstance()->onStageOverBulletLeaved(
			_bulletCounter[_repeatCount], pos[_repeatCount].x, pos[_repeatCount].y);
		_bulletCounter[_repeatCount] = 0;
		updateBulletCount();
		_repeatCount ++;
	}
	else
	{
		assert(0);
	}
}

cocos2d::Vec2 HelloWorld::getLuncherShootPos()
{
	Vec2 ret(Vec2::ZERO);
	if (_layout)
	{
		Widget* spriteLauncher = static_cast<Widget*>(_layout->getChildByTag(TAG_LAUNCHER));
		if (spriteLauncher)
		{
			ImageView* imgBullet = static_cast<ImageView*>(Helper::seekWidgetByName(spriteLauncher, "ImageSelected"));
			if (imgBullet)
			{			
				ret = imgBullet->convertToWorldSpaceAR(Vec2::ZERO);
				/* 注释的方法得到父节点的位置，和上面的结果相同，但必须保持父子关系
				ret = imgBullet->getPosition();
				ret = PointApplyAffineTransform(ret, spriteLauncher->getNodeToParentAffineTransform());
				*/
			}
		}
	}
	return ret;
}

void HelloWorld::onMessage( unsigned int type, unsigned long param1, unsigned long param2 )
{
	switch (type)
	{
	case MSG_SCORE:
		updateScore();
		break;

	case MSG_SHOWFLYSCORE:
		{
			Vec2* pos = (Vec2*)param1;
			showFlyScore(*pos, param2);
			delete pos;
		}
		break;

	case MSG_SHOWCOMBOCOUNT:
		showComboCount(param1);
		break;

	default:
		break;
	}
	
}

#ifdef USE_PhysicsIntegration
bool HelloWorld::onContactBegin(PhysicsContact& contact)
{
	PhysicsBody* a = contact.getShapeA()->getBody();
	PhysicsBody* b = contact.getShapeB()->getBody();
	PhysicsBody* body = (a->getCategoryBitmask() == 0x04 || a->getCategoryBitmask() == 0x08) ? a : b;
	CC_UNUSED_PARAM(body);
	CC_ASSERT(body->getCategoryBitmask() == 0x04 || body->getCategoryBitmask() == 0x08);

	return true;
}
#endif

void HelloWorld::showFlyScore( cocos2d::Vec2 pos, int num )
{
	std::string strScore = StringUtils::format("+%d", num);
//	auto label = Label::createWithTTF(strScore, "fonts/Marker Felt.ttf", 24);
	auto label = Label::createWithBMFont("helvetica-geneva-32.fnt", strScore);
	if (label)
	{
		label->setPosition(pos);
		addChild(label, 100);

		auto moveBy = MoveBy::create(0.5, Vec2(0.0, 40.0));
		label->runAction(Sequence::create(moveBy, 
			CallFunc::create([=](){
				label->removeFromParent();
		}), nullptr));
	}
}

void HelloWorld::showComboCount( int count )
{
	destroyComboCount(0);
	
	Sprite* combo = Sprite::create("combo.png");
	if (combo)
	{
		std::string filename = StringUtils::format("combo_number_%d.png", count);
		Sprite* num = Sprite::create(filename);
		if (num)
		{
			Size sz = combo->getContentSize();
			num->setPosition(Vec2(sz.width / 2, -30));
			combo->addChild(num);
		}

		combo->setTag(TAG_COMBO);
		combo->setPosition(VisibleRect::left() + Vec2(100, 0));
		addChild(combo, 101);

		this->scheduleOnce(schedule_selector(HelloWorld::destroyComboCount), 0.5);
	}
}

void HelloWorld::destroyComboCount(float dt)
{
	auto node = this->getChildByTag(TAG_COMBO);
	if (node)
	{
		node->removeFromParent();
	}
}
