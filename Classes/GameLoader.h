#ifndef _Game_Loader_H_
#define _Game_Loader_H_

#include <string>
#include <map>
#include "SectionLoader.h"

class StageLoader;
class GameLoader
{
public:
	typedef std::map<int, std::string> SectionIdPathMap;

	static GameLoader* getInstance();

	void clear();
	bool load();
	bool loadFromFile(const std::string& filepath);
	bool loadSectionById(int id, SectionLoader* pLoader);
	bool loadStage(int sectionId, int stageId, StageLoader* pStageLoader);

	bool saveStage(int sectionId, StageLoader* pLoader);
	bool save();

	void addSection(int sectionId, const std::string& filename);

	std::string getSectionFileName(int sectionId);
	std::string getStageFileName(int sectionId, int stageId, int count = 0);

private:
	GameLoader();
	~GameLoader();

public:
	static GameLoader* s_instance;

	bool						_isInited;
	std::string					_filename;
	int							_version;
	SectionIdPathMap			_idPathMap;
	SectionLoader				_sectionLoader;
};

#endif