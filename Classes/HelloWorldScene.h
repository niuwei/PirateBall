#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include <vector>
#include "ui/CocosGUI.h"
#include "BallBase.h"


class EnemyBall;
class BlockBase;
class BlockBox;
class StageLoader;
class HelloWorld : public cocos2d::Layer
{
	typedef std::vector<int> ItemTagList;
public:
	HelloWorld();

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene(Layer** pLayer);

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
	void initBulletMenu(cocos2d::Node* parent);
	void initLauncer(cocos2d::Node* parent, cocos2d::Vec2 pos);
	void initTopBar(cocos2d::Node* parent);
    
    // a selector callback
	void onTouchLayoutCallback(cocos2d::Ref* sender, cocos2d::ui::Widget::TouchEventType type);

#ifdef USE_PhysicsIntegration
	bool onContactBegin(cocos2d::PhysicsContact& contact);
#endif

    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);

protected:
// 	virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags) override;
// 	void onDraw(const cocos2d::Mat4 &transform, uint32_t flags);
// 	cocos2d::CustomCommand _customCommand;

protected:
//	void addSlider();
//	void valueChanged(cocos2d::Ref *sender, cocos2d::extension::Control::EventType controlEvent);

	// interface
public:
	void HandleMessage();

public:
// 	virtual bool onTouchBegan(cocos2d::Touch *touch, cocos2d::Event *unused_event); 
// 	virtual void onTouchMoved(cocos2d::Touch *touch, cocos2d::Event *unused_event); 
// 	virtual void onTouchEnded(cocos2d::Touch *touch, cocos2d::Event *unused_event);

//	void Active();
//	void Inactive();

	virtual void onEnter() override;
	virtual void onExit() override;

	void update(float dt);

	// stage
	bool loadStage(int sectionId, int stageId);
	void loadStageFromFile(const std::string& filepath);
	void buildStage(StageLoader& loader);
	void clear();
	void updataStageText();
	void updateScore();

	// luncher
	void updateBulletCount();
	void prepareBullet(int type);
	void setLuncherDirection(float angle);
	float getLuncherDirection();
	cocos2d::Vec2 getLuncherShootPos();
	void shoot();

	// action per frame
	bool checkBulletOutScreen();
	bool checkCollision();
	bool checkBlockCollision();
	void checkLongPressItem();
	void moveOnlyBullet();
	void virtualBulletMove();

	// game logic
	bool checkStageOver(bool& isSuccess);
	void onStageOver(bool isSuccess);
	void showStageOverLayer(bool isSuccess);
	void onTimerCheckBulletLeave(float dt);
	void updataTimeText(bool* pStart = NULL, bool isReset = false);

	virtual EnemyBall* createEnemy(int type);
	virtual BlockBox* createBlock(int type);
	virtual BlockBase* createWheel();
	virtual BlockBase* createVortex();
	virtual BlockBase* createBomb();
	virtual void onUpdate(float dt);
	virtual void onMessage(unsigned int type, unsigned long param1, unsigned long param2);

	// editor
	void newStage();
	void addEnemy(int type, const cocos2d::Vec2& pos, int ringType);
	void addBlock(const cocos2d::Vec2& pos, int typeBox = 0);
	void addWheel(const cocos2d::Vec2& pos);
	void addVortex(const cocos2d::Vec2& pos);
	void addBomb(const cocos2d::Vec2& pos);

	// effects
	void showFlyScore(cocos2d::Vec2 pos, int num);
	void showComboCount(int count);
	void destroyComboCount(float dt);


	ItemTagList				_itemTags;
	ItemTagList				_blockTags;
	int						_nItemTagBase;
	int						_sectionId;
	int						_stageId;

	bool					_isStageOvered;
	int						_repeatCount;

	int						_spendTimeSecond;
	bool					_isShowSightLine;

	cocos2d::ui::Layout* _layout;
	int _bulletCounter[BallBase::kTypeBulletUnknown];
	float _directLauncher;
	int _bulletType;

#ifdef USE_PhysicsIntegration
private:
	PhysicsWorld* m_world;

public:
	void setPhyWorld(PhysicsWorld* world){ m_world = world;}

#endif // USE_PhysicsIntegration

};

#endif // __HELLOWORLD_SCENE_H__
