#include "StageLoader.h"
#include "JsonUtils.h"
#include "cocos2d.h"

#include "json/writer.h"
#include "json/stringbuffer.h"



#define KEY_STAGE_ID				"stage-id"
#define KEY_ENEMYS					"enemys"
#define KEY_BLOCKS					"blocks"
#define KEY_ENEMY_TYPE				"type"
#define KEY_ENEMY_RING_TYPE			"ring-type"
#define KEY_ENEMY_EXTEND_TYPE		"extend-type"
#define KEY_ENEMY_POSX				"pos-x"
#define KEY_ENEMY_POSY				"pos-y"
#define KEY_BULLETS					"bullets"

USING_NS_CC;

StageLoader::StageLoader()
	: _stageId(0)
	, _filename("")
{
	for (int i = 0; i < BallBase::kTypeBulletUnknown; ++ i)
	{
		_bullets[i] = 0;
	}
}

StageLoader::~StageLoader()
{

}

bool StageLoader::load(const std::string& filepath)
{
	return loadStageFromFile(filepath);
}

bool StageLoader::loadStageFromFile( const std::string& filepath )
{
	///!!! 之前要检查路径是否存在
	JsonReadUtils json_read;
	if (!json_read.parseFromFile(filepath))
	{
		CCLOG("StageLoader: can not parse josn from file-%s.", filepath.c_str());
		return false;
	}
	
	rapidjson::Value& root = json_read.getRoot();

	// stage id
	_stageId = JsonReadUtils::getInt(root, KEY_STAGE_ID);

	// enemys
	rapidjson::Value& enemys = root[KEY_ENEMYS];
	if (enemys.IsArray())
	{
		for (unsigned int i = 0; i < enemys.Size(); ++ i)
		{
			rapidjson::Value& val = enemys[i];
			EnemyItem enemy;
			enemy._type = JsonReadUtils::getUInt(val, KEY_ENEMY_TYPE);
			enemy._pos.x = JsonReadUtils::getDouble(val, KEY_ENEMY_POSX);
			enemy._pos.y = JsonReadUtils::getDouble(val, KEY_ENEMY_POSY);
			enemy._ringType = JsonReadUtils::getUInt(val, KEY_ENEMY_RING_TYPE, BallBase::kTypeBulletUnknown);
			_enemys.push_back(enemy);
		}
	}

	// blocks
	rapidjson::Value& blocks = root[KEY_BLOCKS];
	if (blocks.IsArray())
	{
		for (unsigned int i = 0; i < blocks.Size(); ++ i)
		{
			rapidjson::Value& val = blocks[i];
			EnemyItem block;
			block._type = JsonReadUtils::getUInt(val, KEY_ENEMY_TYPE);
			block._pos.x = JsonReadUtils::getDouble(val, KEY_ENEMY_POSX);
			block._pos.y = JsonReadUtils::getDouble(val, KEY_ENEMY_POSY);
			block._ringType = JsonReadUtils::getUInt(val, KEY_ENEMY_EXTEND_TYPE);
			_blocks.push_back(block);
		}
	}

	// bullets
	rapidjson::Value& bullets = root[KEY_BULLETS];
	if (bullets.IsArray())
	{
		for (unsigned int i = 0; i < bullets.Size(); ++ i)
		{
			if (i < BallBase::kTypeBulletUnknown)
			{
				_bullets[i] = bullets[i].GetInt();
			}
		}
	}

	return true;
}

void StageLoader::save()
{
	assert(!_filename.empty());

	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	// stage id
	document.AddMember(KEY_STAGE_ID, _stageId, allocator);

	// enemys
	rapidjson::Value enemyArry(rapidjson::kArrayType);
	for (auto& it : _enemys)
	{
		rapidjson::Value object(rapidjson::kObjectType);
		object.AddMember(KEY_ENEMY_TYPE, it._type, allocator);
		object.AddMember(KEY_ENEMY_RING_TYPE, it._ringType, allocator);
		object.AddMember(KEY_ENEMY_POSX, it._pos.x, allocator);
		object.AddMember(KEY_ENEMY_POSY, it._pos.y, allocator);
		enemyArry.PushBack(object,allocator);
	}
	document.AddMember(KEY_ENEMYS, enemyArry, allocator);

	// blocks
	rapidjson::Value blockArry(rapidjson::kArrayType);
	for (auto& it : _blocks)
	{
		rapidjson::Value object(rapidjson::kObjectType);
		object.AddMember(KEY_ENEMY_TYPE, it._type, allocator);
		object.AddMember(KEY_ENEMY_EXTEND_TYPE, it._ringType, allocator);
		object.AddMember(KEY_ENEMY_POSX, it._pos.x, allocator);
		object.AddMember(KEY_ENEMY_POSY, it._pos.y, allocator);
		blockArry.PushBack(object,allocator);
	}
	document.AddMember(KEY_BLOCKS, blockArry, allocator);

	// bullet count
	rapidjson::Value bulletArry(rapidjson::kArrayType);
	for (int i = 0; i < BallBase::kTypeBulletUnknown; ++ i)
	{
		bulletArry.PushBack(_bullets[i], allocator);
	}
	document.AddMember(KEY_BULLETS, bulletArry, allocator);

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	document.Accept(writer);

	// save file
	auto path = FileUtils::getInstance()->getWritablePath();
	path.append(_filename);
	CCLOG("PirateBall - save stage path: %s", path.c_str());
	FILE* fp = std::fopen(path.c_str(), "w+");
	CCASSERT(fp != NULL, "file open error");
	auto str = StringUtils::format("%s",buffer.GetString());
	auto length = str.length();
	fwrite(str.c_str(), length, 1 ,fp);
	fclose(fp);
}

void StageLoader::setStageId( int id )
{
	_stageId = id;
}

void StageLoader::addEnemy( int type, cocos2d::Vec2 pos, int ringType )
{
	StageLoader::EnemyItem item;
	item._type = type;
	item._pos = pos;
	item._ringType = ringType;
	_enemys.push_back(item);
}

void StageLoader::addBlock( int type, cocos2d::Vec2 pos, int extendType )
{
	StageLoader::EnemyItem item;
	item._type = type;
	item._pos = pos;
	item._ringType = extendType;
	_blocks.push_back(item);
}

void StageLoader::setBullets( int* bullet, int count )
{
	assert(count == BallBase::kTypeBulletUnknown);
	for (int i = 0; i < BallBase::kTypeBulletUnknown; ++ i)
	{
		_bullets[i] = bullet[i];
	}
}
