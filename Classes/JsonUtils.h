#ifndef _Json_Utils_H_
#define _Json_Utils_H_

#include <string>
#include "json/document.h"

class JsonReadUtils
{
public:
	JsonReadUtils();
	~JsonReadUtils(void);

	bool parseFromFile(const std::string &fileName);
	bool parseFromString(const std::string& jsonStr);
	rapidjson::Value& getRoot();

	static const std::string getString(rapidjson::Value& jsonValue, const std::string& key);
	static int getInt(rapidjson::Value& jsonValue, const std::string& key);
	static int64_t getInt64(rapidjson::Value& jsonValue, const std::string& key);
	static unsigned int getUInt(rapidjson::Value& jsonValue, const std::string& key, unsigned int def_val = 0);
	static uint64_t getUInt64(rapidjson::Value& jsonValue, const std::string& key);
	static double getDouble(rapidjson::Value& jsonValue, const std::string& key);

private:
	const std::string getStringFromFile(const std::string &fileName);

	rapidjson::Document _mDoc;
};

class JsonWriteUtils
{
public:
	JsonWriteUtils();
	~JsonWriteUtils();

private:
	rapidjson::Document _mDoc;
};

#endif