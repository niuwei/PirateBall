#include "SectionLoader.h"
#include "StageLoader.h"
#include "JsonUtils.h"
#include "cocos2d.h"

#include "json/writer.h"
#include "json/stringbuffer.h"


#define KEY_VERSION			"version"
#define KEY_STAGES			"stages"


SectionLoader::SectionLoader()
	: _sectionId(0)
	, _version(1)
	, _filename("")
{

}

SectionLoader::~SectionLoader()
{

}

bool SectionLoader::loadStageById( int stageId, StageLoader* pStage )
{
	assert(pStage);
	StageIdPathMap::iterator it = _idPathMap.find(stageId);
	if (it != _idPathMap.end())
	{
		if (pStage->loadStageFromFile(it->second))
		{
			assert(stageId == pStage->_stageId);
			return true;
		}
	}
	return false;
}

bool SectionLoader::loadSectionFromFile( const std::string& filepath )
{
	if (filepath.empty())
		return false;

	JsonReadUtils json_read;
	if (!json_read.parseFromFile(filepath))
		return false;

	rapidjson::Value& root = json_read.getRoot();

	// version
	_version = JsonReadUtils::getInt(root, KEY_VERSION);

	// stages
	rapidjson::Value& stages = root[KEY_STAGES];
	if (!stages.IsArray())
		return false;

	unsigned int count = stages.Size();
	for (unsigned int i = 0; i < count; ++ i)
	{
		rapidjson::Value& val = stages[i];
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

bool SectionLoader::save()
{
	assert(!_filename.empty());

	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	// version
	document.AddMember(KEY_VERSION, _version, allocator);

	// stages
	rapidjson::Value stages(rapidjson::kArrayType);
	StageIdPathMap::iterator it = _idPathMap.begin();
	for (; it != _idPathMap.end(); ++ it)
	{
		rapidjson::Value object(rapidjson::kArrayType);
		rapidjson::Value id(it->first);
		object.PushBack(id, allocator);
		rapidjson::Value name(it->second.c_str(), it->second.length());
		object.PushBack(name, allocator);

		stages.PushBack(object, allocator);
	}

	document.AddMember(KEY_STAGES, stages, allocator);

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

int SectionLoader::check()
{
	return 0;
}

void SectionLoader::addStage( int stageId, const std::string& filename )
{
	_idPathMap[stageId] = filename;
}

bool SectionLoader::hasSameNameStage( const std::string& filename, int* stageId )
{
	StageIdPathMap::iterator it = _idPathMap.begin();
	for (; it != _idPathMap.end(); ++ it)
	{
		if (it->second == filename)
		{
			*stageId = it->first;
			return true;
		}
	}
	return false;
}

void SectionLoader::clear()
{
	_sectionId = 0;
	_idPathMap.clear();
	_filename.clear();
}


