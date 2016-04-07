#include "JsonUtils.h"
#include "cocos2d.h"

USING_NS_CC;

#define RETURN_IF(cond,p)           if((cond)) return (p)


JsonReadUtils::JsonReadUtils()
{
}

JsonReadUtils::~JsonReadUtils(void)
{
}

const std::string JsonReadUtils::getStringFromFile( const std::string &fileName)
{
	const std::string mstr = cocos2d::FileUtils::getInstance()->getStringFromFile(fileName);
	return mstr;
}

bool JsonReadUtils::parseFromFile( const std::string &fileName )
{
	if (FileUtils::getInstance()->isAbsolutePath(fileName))
	{
		std::string mstr = getStringFromFile(fileName);
		return parseFromString(mstr);
	}
	else
	{
		// first load file from writable path.
		std::string file_path = FileUtils::getInstance()->getWritablePath();
		file_path.append(fileName);
		std::string mstr = getStringFromFile(file_path);
		if (parseFromString(mstr))
		{
			return true;
		}
		else
		{
			// if load failed, second load file from source path
			file_path = FileUtils::getInstance()->fullPathForFilename(fileName);
			std::string mstr = getStringFromFile(file_path);
			return parseFromString(mstr);
		}
	}	
}

bool JsonReadUtils::parseFromString( const std::string& jsonStr )
{
	RETURN_IF(NULL==jsonStr.c_str()||!jsonStr.compare(""),false);
	_mDoc.Parse<rapidjson::kParseDefaultFlags>(jsonStr.c_str());
	return (!_mDoc.HasParseError()) && _mDoc.IsObject();
}

rapidjson::Value& JsonReadUtils::getRoot()
{
	return *(&_mDoc);
}

const std::string JsonReadUtils::getString( rapidjson::Value& jsonValue, const std::string& key )
{
	RETURN_IF(!jsonValue.IsObject()||!jsonValue.HasMember(key.c_str()), "");
	const rapidjson::Value &p = jsonValue[key.c_str()];
	RETURN_IF(!p.IsString(),"");
	return p.GetString();
}

int JsonReadUtils::getInt( rapidjson::Value& jsonValue, const std::string& key )
{
	RETURN_IF(!jsonValue.IsObject()||!jsonValue.HasMember(key.c_str()), 0);
	const rapidjson::Value &p = jsonValue[key.c_str()];
	RETURN_IF(!p.IsInt(),0);
	return p.GetInt();
}

int64_t JsonReadUtils::getInt64( rapidjson::Value& jsonValue, const std::string& key )
{
	RETURN_IF(!jsonValue.IsObject()||!jsonValue.HasMember(key.c_str()), 0);
	const rapidjson::Value &p = jsonValue[key.c_str()];
	RETURN_IF(!p.IsInt64(),0);
	return p.GetInt64();
}

unsigned int JsonReadUtils::getUInt( rapidjson::Value& jsonValue, const std::string& key, unsigned int def_val )
{
	RETURN_IF(!jsonValue.IsObject()||!jsonValue.HasMember(key.c_str()), def_val);
	const rapidjson::Value &p = jsonValue[key.c_str()];
	RETURN_IF(!p.IsUint(), def_val);
	return p.GetUint();
}

uint64_t JsonReadUtils::getUInt64( rapidjson::Value& jsonValue, const std::string& key )
{
	RETURN_IF(!jsonValue.IsObject()||!jsonValue.HasMember(key.c_str()), 0);
	const rapidjson::Value &p = jsonValue[key.c_str()];
	RETURN_IF(!p.IsUint64(),0);
	return p.GetUint64();
}

double JsonReadUtils::getDouble( rapidjson::Value& jsonValue, const std::string& key )
{
	RETURN_IF(!jsonValue.IsObject()||!jsonValue.HasMember(key.c_str()), 0.0);
	const rapidjson::Value &p = jsonValue[key.c_str()];
	if (p.IsDouble())
	{
		return p.GetDouble();
	}
	else if (p.IsInt())
	{
		return p.GetInt();
	}
	else
	{
		return 0.0;
	}
// 	RETURN_IF(!p.IsDouble(),0.0);
// 	return p.GetDouble();
}


//////////////////////////////////////////////////////////////////////////
JsonWriteUtils::JsonWriteUtils()
{
/*	_mDoc.SetObject();

	rapidjson::Document document;
	document.SetObject();
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	rapidjson::Value object1(rapidjson::kObjectType);
	rapidjson::Value object2(rapidjson::kObjectType);
	rapidjson::Value object3(rapidjson::kObjectType);
	rapidjson::Value object4(rapidjson::kArrayType);
	object1.AddMember("Name", "Yuxikuo", allocator);
	object1.AddMember("Age", "18", allocator);
	object1.AddMember("Sex", "man", allocator);

	object2.AddMember("Name", "WangDongGe", allocator);
	object2.AddMember("Age", "32", allocator);
	object2.AddMember("Sex", "woman", allocator);
	
	object3.AddMember("Name", "zhanhong", allocator);
	object3.AddMember("Age", "20", allocator);
	object3.AddMember("Sex", "man", allocator);
	
	object4.PushBack(object1,allocator);
	object4.PushBack(object2,allocator);
	object4.PushBack(object3,allocator);

	document.AddMember("info", object4, allocator);

	StringBuffer buffer;
	rapidjson::Writer<StringBuffer> writer(buffer);
	document.Accept(writer);
	log("%s",buffer.GetString());
	return StringUtils::format("%s",buffer.GetString());
*/
}

JsonWriteUtils::~JsonWriteUtils()
{

}

