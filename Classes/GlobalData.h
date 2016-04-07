#ifndef _Global_Data_H_
#define _Global_Data_H_

#include <string>
#include <vector>
#include <map>
#include "scoped_ptr.h"
#include "GlobalDefined.h"


float float_mod(float f1, float f2);

class StageRecord;
class GlobalData
{
public:
	static GlobalData* getInstance();

	void init();
	bool setStageRecord(int id, int spendTime, int score);
	bool getStageRecord(int id, StageRecordItem* item);

	int getNextSectionStageId(int sectionId, int stageId);
	int getMaxEnabledStageId();

	static int getSectionStageId(int sectionId, int stageId);
	static std::string aes_encrypt(const std::string& strVal);
	static std::string aes_decrypt(const std::string& strVal);

	bool isEditMode() { return _gameMode == GAME_MODE_EDIT; }

	void setSelectedTag(int tag);
	int getSelectedTag()			{ return _selectedTag; }
	bool noSelected()				{ return _selectedTag == 0; }

	float getGridInterval()			{ return _gridInterval; }

private:
	GlobalData();
	~GlobalData();

	static GlobalData* s_instance;

public:
	bool							_isInited;
	int								_gameMode;
	int								_selectedTag;
	scoped_ptr<StageRecord>			_stageRecord;
	std::map<int, StageRecordItem>	_stageRecordMap;
	float							_gridInterval;
};


#endif