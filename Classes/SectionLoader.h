#ifndef _Section_Loader_H_
#define _Section_Loader_H_

#include <string>
#include <map>

class StageLoader;
class SectionLoader
{
public:
	typedef std::map<int, std::string> StageIdPathMap;

	SectionLoader();
	~SectionLoader();

	void clear();
	bool loadStageById(int stageId, StageLoader* pStage);
	bool loadSectionFromFile(const std::string& filepath);
	bool save();

	void addStage(int stageId, const std::string& filename);
	bool hasSameNameStage(const std::string& filename, int* stageId);

	int check();

	int						_version;
	int						_sectionId;
	StageIdPathMap			_idPathMap;
	std::string				_filename;
};


#endif