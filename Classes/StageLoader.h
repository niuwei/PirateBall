#ifndef _Stage_Loader_H_
#define _Stage_Loader_H_

#include <string>
#include "cocos2d.h"
#include "BallBase.h"


class StageLoader
{
public:
	struct EnemyItem
	{
		int					_type;
		int					_ringType;
		cocos2d::Vec2		_pos;
	};

	typedef std::vector<EnemyItem> EmenyList;

	StageLoader();
	~StageLoader();

	bool load(const std::string& filepath);
	bool loadStageFromFile(const std::string& filepath);
	void save();

	void setStageId(int id);
	void addEnemy(int type, cocos2d::Vec2 pos, int ringType);
	void addBlock(int type, cocos2d::Vec2 pos, int extendType);
	void setBullets(int* bullet, int count);

	int			_stageId;
	EmenyList	_enemys;
	EmenyList	_blocks;
	
	int			_bullets[BallBase::kTypeBulletUnknown];
	std::string	_filename;
};


#endif