#include "GlobalDefined.h"
#include "GameLoader.h"
#include "JsonUtils.h"
#include "cocos2d.h"
#include "json/writer.h"
#include "json/stringbuffer.h"
#include "StageLoader.h"


USING_NS_CC;

#define KEY_VERSION			"version"
#define KEY_SECTIONS		"sections"

GameLoader* GameLoader::s_instance = 0;

GameLoader* GameLoader::getInstance()
{
	if (!s_instance)
	{
		s_instance = new GameLoader();
	}
	return s_instance;
}

GameLoader::GameLoader()
	: _version(1)
	, _filename(GAME_LOADER_FILE)
	, _isInited(false)
{
	
}

GameLoader::~GameLoader()
{

}

bool GameLoader::loadFromFile(const std::string& filepath)
{
	if (filepath.empty())
		return false;

	JsonReadUtils json_read;
	if (!json_read.parseFromFile(filepath))
        save();

    if (!json_read.parseFromFile(filepath))
		return false;

	rapidjson::Value& root = json_read.getRoot();

	// version
	_version = JsonReadUtils::getInt(root, KEY_VERSION);

	// sections
	rapidjson::Value& sections = root[KEY_SECTIONS];
	if (!sections.IsArray())
		return false;

	unsigned int count = sections.Size();
    
    if (count == 0) {
        StageLoader loader;
        loader._stageId = 1;
        GameLoader::getInstance()->saveStage(1, &loader);
        count = sections.Size();
    }
    
	for (unsigned int i = 0; i < count; ++ i)
	{
		rapidjson::Value& val = sections[i];
		if (!val.IsArray() || val.Size() != 2)
			continue;

		rapidjson::Value& id = val[rapidjson::SizeType(0)];
		rapidjson::Value& path = val[rapidjson::SizeType(1)];

		if (!id.IsInt() || !path.IsString())
			continue;

		_idPathMap[id.GetInt()] = path.GetString();
	}

	return true;
}

bool GameLoader::load()
{
	clear();
	_isInited = true;
	return loadFromFile(_filename);
}

bool GameLoader::loadStage( int sectionId, int stageId, StageLoader* pStageLoader )
{
	assert(_isInited);
	assert(pStageLoader);
	if (sectionId != _sectionLoader._sectionId)
	{
		if (!GameLoader::getInstance()->loadSectionById(sectionId, &_sectionLoader))
			return false;
	}

	return _sectionLoader.loadStageById(stageId, pStageLoader);
}

bool GameLoader::loadSectionById( int id, SectionLoader* pLoader )
{
	assert(pLoader);
	pLoader->clear();
	SectionIdPathMap::iterator it = _idPathMap.find(id);
	if (it != _idPathMap.end())
	{
		pLoader->_filename = it->second;
		pLoader->_sectionId = id;
		return pLoader->loadSectionFromFile(it->second);
	}
	return false;
}

bool GameLoader::saveStage( int sectionId, StageLoader* pLoader )
{
	assert(_isInited);
	assert(pLoader);

	// 1. get section by id
	bool br = true;
	if (sectionId != _sectionLoader._sectionId)
	{
		br = loadSectionById(sectionId, &_sectionLoader);
	}

	// 2. add stage name to section loader
	if (pLoader->_filename.empty())
		pLoader->_filename = getStageFileName(sectionId, pLoader->_stageId);
	
	std::string stageName = pLoader->_filename;
	int sameNameId = 0, count = 1;
	while (_sectionLoader.hasSameNameStage(stageName, &sameNameId) &&
		sameNameId != pLoader->_stageId)
	{
		stageName = getStageFileName(sectionId, pLoader->_stageId, count ++);
	}

	// if has same name, rename stage with (i)
	if (count != 1 && sameNameId != pLoader->_stageId)
	{
		pLoader->_filename = stageName;
	}

	_sectionLoader.addStage(pLoader->_stageId, pLoader->_filename);

	// 3. save stage loader (or by new name)
	pLoader->save();

	if (!br)
	{
		// 4. if section not exist, add section loader to game loader
		_sectionLoader._filename = getSectionFileName(sectionId);
		addSection(sectionId, _sectionLoader._filename);

		// save game loader
		this->save();
	}

	// 5. save section loader
	_sectionLoader._sectionId = sectionId;
	_sectionLoader.save();

	return true;
}

bool GameLoader::save()
{
	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	// version
	document.AddMember(KEY_VERSION, _version, allocator);

	// stages
	rapidjson::Value sections(rapidjson::kArrayType);
	SectionIdPathMap::iterator it = _idPathMap.begin();
	for (; it != _idPathMap.end(); ++ it)
	{
		rapidjson::Value object(rapidjson::kArrayType);
		rapidjson::Value id(it->first);
		object.PushBack(id, allocator);
		rapidjson::Value name(it->second.c_str(), it->second.length());
		object.PushBack(name, allocator);

		sections.PushBack(object, allocator);
	}

	document.AddMember(KEY_SECTIONS, sections, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	document.Accept(writer);

	// save file
	auto path = FileUtils::getInstance()->getWritablePath();
	path.append(_filename);
	FILE* fp = std::fopen(path.c_str(), "w+");
	CCASSERT(fp != NULL, "file open error");
	auto str = StringUtils::format("%s", buffer.GetString());
	auto length = str.length();
	fwrite(str.c_str(), length, 1 ,fp);
	fclose(fp);

	return true;
}

void GameLoader::addSection( int sectionId, const std::string& filename )
{
	assert(_isInited);
	_idPathMap[sectionId] = filename;
}

std::string GameLoader::getSectionFileName( int sectionId )
{
	return StringUtils::format("section-%03d.json", sectionId);
}

std::string GameLoader::getStageFileName(int sectionId, int stageId, int count)
{
	if (count)
	{
		return StringUtils::format("stage-%02d-%03d(%d).json", sectionId, stageId, count);
	}
	return StringUtils::format("stage-%02d-%03d.json", sectionId, stageId);
}

void GameLoader::clear()
{
	_idPathMap.clear();
	_sectionLoader.clear();
}
