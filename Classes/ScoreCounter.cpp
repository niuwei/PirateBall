#include "GlobalDefined.h"
#include "ScoreCounter.h"
#include "MessageDispatcher.h"
#include "GlobalData.h"
#include "cocos2d.h"

USING_NS_CC;

ScoreCounter* ScoreCounter::_s_instance = 0;

ScoreCounter::ScoreCounter()
	: _scoreHistory(0)
{
	reset();
}

ScoreCounter::~ScoreCounter()
{

}

ScoreCounter* ScoreCounter::getInstance()
{
	if (!_s_instance)
	{
		_s_instance = new ScoreCounter;
	}
	return _s_instance;
}

unsigned int ScoreCounter::getScore()
{
	return _score;
}

unsigned int ScoreCounter::getTime()
{
	return _time;
}

void ScoreCounter::reset()
{
	_score = 0;
	_comboCount = 0;
	_time = 0;
	if (!GlobalData::getInstance()->isEditMode())
	{
		MessageDispatcher::getInstance()->pushMessage(MSG_SCORE);
	}
	
}

void ScoreCounter::onBulletShoot()
{
	_comboCount = 0;
}

void ScoreCounter::onBulletDestory()
{
	_comboCount = 0;
}

void ScoreCounter::onDestroyEnemy(float pos_x, float pos_y)
{
	_comboCount ++;
	unsigned int s = _comboCount * 1000;
	_score += s;
	if (!GlobalData::getInstance()->isEditMode())
	{
		MessageDispatcher::getInstance()->pushMessage(MSG_SCORE);
		MessageDispatcher::getInstance()->pushMessage(MSG_SHOWCOMBOCOUNT, _comboCount);
		Vec2* pos = new Vec2(pos_x, pos_y);
		MessageDispatcher::getInstance()->pushMessage(MSG_SHOWFLYSCORE, (unsigned long)pos, s);
	}
}

void ScoreCounter::onStageOver( StageOverParams& params )
{
	_time = params._timeSpent;
}

void ScoreCounter::onStageOverBulletLeaved( int count, float pos_x, float pos_y )
{
	unsigned int s = count * 1000;
	_score += s;
	if (!GlobalData::getInstance()->isEditMode())
	{
		MessageDispatcher::getInstance()->pushMessage(MSG_SCORE);
		Vec2* pos = new Vec2(pos_x, pos_y);
		MessageDispatcher::getInstance()->pushMessage(MSG_SHOWFLYSCORE, (unsigned long)pos, s);
	}
}

