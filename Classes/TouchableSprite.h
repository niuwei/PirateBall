#ifndef _TouchableSprite_H_
#define _TouchableSprite_H_

#include "cocos2d.h"
#include "GlobalData.h"

USING_NS_CC;

template<class T>
class TouchableSprite : public T
{
public:
	void onTouchBegin()						{ T::onTouchBegin(); }
	virtual void onTouchMoved(Vec2& pos)	{ T::onTouchMoved(pos); }
	virtual void onTouchEnd()				{ T::onTouchEnd(); }
	
public:
// 	static TouchableSprite* create(int priority = 0)
// 	{
// 		auto ret = new (std::nothrow) TouchableSprite();
// 		if (ret)
// 		{
// 			ret->_fixedPriority = priority;
// 			if (ret->init())
// 			{
// 				ret->autorelease();
// 				return ret;
// 			}
// 		}
// 		CC_SAFE_DELETE(ret);
// 		return ret;
// 	}

public:
	TouchableSprite()
		: _listener(nullptr)
		, _fixedPriority(0)
		, _removeListenerOnTouchEnded(false)
		, _enableTouch(true)
		, _originPos(Vec2::ZERO)
		, _enableTileTouch(true)
	{
	}

public:
	virtual void onEnter()
	{
		T::onEnter();
		
		auto listener = EventListenerTouchOneByOne::create();
		listener->setSwallowTouches(true);

		listener->onTouchBegan = [=](Touch* touch, Event* event){

			Vec2 locationInNode = this->convertToNodeSpace(touch->getLocation());
			Size s = this->getContentSize();
			Rect rect = Rect(0, 0, s.width, s.height);

			if (rect.containsPoint(locationInNode))
			{
				_originPos = locationInNode - this->getAnchorPointInPoints();
				onTouchBegin();
				return true;
			}
			return false;
		};

		listener->onTouchMoved = [](Touch* touch, Event* event){

			auto target = static_cast<TouchableSprite*>(event->getCurrentTarget());
			Vec2 pos = target->getPosition() + touch->getDelta();
			Vec2 location = touch->getLocation();
			pos = location - target->_originPos;
			if (target->_enableTileTouch)
			{
				target->onTileTouch(pos, GlobalData::getInstance()->getGridInterval(), 5);
			}
			
			target->onTouchMoved(pos);
			target->setPosition(pos);
		};

		listener->onTouchEnded = [=](Touch* touch, Event* event){

			if (_removeListenerOnTouchEnded)
			{
				Director::getInstance()->getEventDispatcher()->
					removeEventListener(listener);
			}
			this->onTouchEnd();
		};

		if (_fixedPriority != 0)
		{
			Director::getInstance()->getEventDispatcher()->
				addEventListenerWithFixedPriority(listener, _fixedPriority);
		}
		else
		{
			Director::getInstance()->getEventDispatcher()->
				addEventListenerWithSceneGraphPriority(listener, this);
		}

		_listener = listener;
	}

	virtual void onExit()
	{
		Director::getInstance()->getEventDispatcher()->
			removeEventListener(_listener);

		T::onExit();
	}

	void removeListenerOnTouchEnded(bool toRemove) { _removeListenerOnTouchEnded = toRemove; };

	inline EventListener* getListener() { return _listener; }

	void onTileTouch(Vec2& pos, float gridLen, float touchLen)
	{	
		Size size = this->getContentSize();

		float left = pos.x - size.width / 2;
		float right = pos.x + size.width / 2;
		float top = pos.y + size.height / 2;
		float bottom = pos.y - size.height / 2;

		float interval = gridLen;

		left = float_mod(left, interval);
		left = (left > interval / 2) ? (left - interval) : left;
		right = float_mod(right, interval);
		right = (right > interval / 2) ? (right - interval) : right;
		bottom = float_mod(bottom, interval);
		bottom = (bottom > interval / 2) ? (bottom - interval) : bottom;
		top = float_mod(top, interval);
		top = (top > interval / 2) ? (top - interval) : top;

		float dist = touchLen;
		if (left < dist && left > -dist)
		{
			pos.x -= left;
		}
		else if (right < dist && right > -dist)
		{
			pos.x -= right;
		}

		if (bottom < dist && bottom > -dist)
		{
			pos.y -= bottom;
		}
		else if (top < dist && top > -dist)
		{
			pos.y -= top;
		}
	}

private:
	EventListener* _listener;
	int _fixedPriority;
	bool _removeListenerOnTouchEnded;
	bool _enableTouch;
	Vec2 _originPos;
	bool _enableTileTouch;
};

typedef TouchableSprite<class T> TouchableSpriteT;

#endif