#ifndef _ScoreCounter_H_
#define _ScoreCounter_H_

#include "cocos2d.h"

struct StageOverParams
{
	bool _isSucceed;
	int	_timeSpent;
};

class ScoreCounter
{
public:
	static ScoreCounter* getInstance();

	unsigned int getScore();
	unsigned int getTime();
	void reset();

	void onBulletShoot();
	void onBulletDestory();
	void onDestroyEnemy(float pos_x = 0, float pos_y = 0);
	void onStageOver(StageOverParams& params);
	void onStageOverBulletLeaved(int count, float pos_x = 0, float pos_y = 0);

private:
	ScoreCounter();
	~ScoreCounter();

	static ScoreCounter* _s_instance;

	unsigned int		_comboCount;
	unsigned int		_score;
	unsigned int		_scoreHistory;
	unsigned int		_time;
};

#endif