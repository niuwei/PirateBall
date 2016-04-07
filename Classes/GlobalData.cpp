#include "GlobalData.h"
#include "MessageDispatcher.h"
#include "aes.h"
#include "StageRecord.h"
#include "cocos2d.h"

USING_NS_CC;

#define AES_KEY						"AE52CE62D81F4fd49C1218EFD4B8BE98"

GlobalData* GlobalData::s_instance = 0;

GlobalData* GlobalData::getInstance()
{
	if (!s_instance)
	{
		s_instance = new GlobalData();
	}
	return s_instance;
}

GlobalData::GlobalData()
	: _isInited(false)
	, _selectedTag(0)
	, _gridInterval(20)
{
	
}

GlobalData::~GlobalData()
{

}

std::string GlobalData::aes_encrypt( const std::string& strVal )
{
	std::string strSrc = strVal;
	unsigned int align_size = strSrc.size();
	if (align_size % ENCRYPT_BLOCK_SIZE != 0)
	{
		align_size = (align_size / ENCRYPT_BLOCK_SIZE + 1) * ENCRYPT_BLOCK_SIZE;
		strSrc.resize(align_size, ' ');
	}

	// encrypt
	unsigned int len = strSrc.size();
	std::string szEncrpt(len, '\0');
	CAES aes(ENCRYPT_BLOCK_SIZE, (unsigned char*)AES_KEY);
	int loop = len / ENCRYPT_BLOCK_SIZE;
	for (int i = 0; i < loop; ++i)
	{
		aes.Cipher((unsigned char*)strSrc.c_str() + i * ENCRYPT_BLOCK_SIZE,
			(unsigned char*)szEncrpt.c_str() + i * ENCRYPT_BLOCK_SIZE);
	}
	return szEncrpt;
}

std::string GlobalData::aes_decrypt( const std::string& strVal )
{
	std::string szDecrypt(strVal.size(), '\0');
	CAES aes(ENCRYPT_BLOCK_SIZE, (unsigned char*)AES_KEY);
	int loop = szDecrypt.size() / ENCRYPT_BLOCK_SIZE;
	for (int i = 0; i < loop; ++i)
	{
		aes.InvCipher((unsigned char*)strVal.c_str() + i * ENCRYPT_BLOCK_SIZE,
			(unsigned char*)szDecrypt.c_str() + i * ENCRYPT_BLOCK_SIZE);
	}

	std::string::size_type pos = szDecrypt.find_last_not_of(" ");
	if (pos != szDecrypt.length() - 1)
	{
		szDecrypt.erase(pos + 1, szDecrypt.length() - pos - 1);
	}
	return szDecrypt;
}

void GlobalData::init()
{
	if (_isInited)
		return;
	_isInited = true;

	bool br;
	std::string path = FileUtils::getInstance()->getWritablePath();
	path.append("stage-record.db");
	_stageRecord.reset(new StageRecord);
	_stageRecord->Init(path);
	br = _stageRecord->Open();
	assert(br);

	int offset = 0, rows = 100, readCount = 0;
	do
	{
		std::vector<StageRecordItem> records;
		br = _stageRecord->getRecords(&records, offset, rows);
		assert(br);
		readCount = records.size();
		offset += readCount;

		std::for_each(records.begin(), records.end(), [&](StageRecordItem& item){
			_stageRecordMap[item.id] = item;
		});
	} while (readCount == rows);
	
}

bool GlobalData::setStageRecord( int id, int spendTime, int score )
{
	bool needWrite = false;
	std::map<int, StageRecordItem>::iterator it =
		_stageRecordMap.find(id);
	if (it != _stageRecordMap.end())
	{
		if (it->second.spendTime != spendTime ||
			it->second.score != score)
		{
			it->second.spendTime = spendTime;
			it->second.score = score;
			needWrite = true;
		}
	}
	else
	{
		StageRecordItem item;
		item.id = id;
		item.spendTime = spendTime;
		item.score = score;
		_stageRecordMap.insert(
			std::map<int, StageRecordItem>::value_type(id, item));
		needWrite = true;
	}

	bool br = true;
	if (needWrite)
	{
		br = _stageRecord->writeRecord(id, spendTime, score);
	}
	return br;
}

bool GlobalData::getStageRecord( int id, StageRecordItem* item )
{
	assert(item);
	std::map<int, StageRecordItem>::iterator it =
		_stageRecordMap.find(id);
	if (it != _stageRecordMap.end())
	{
		*item = it->second;
		return true;
	}
	return false;
}

int GlobalData::getNextSectionStageId( int sectionId, int stageId )
{
	int sectionStageId = getSectionStageId(sectionId, stageId + 1);
	std::map<int, StageRecordItem>::iterator it =
		_stageRecordMap.find(sectionStageId);
	if (it != _stageRecordMap.end())
	{
		return sectionStageId;
	}
	else
	{
		sectionStageId = getSectionStageId(sectionId + 1, 1);
		std::map<int, StageRecordItem>::iterator it =
			_stageRecordMap.find(sectionStageId);
		if (it != _stageRecordMap.end())
		{
			return sectionStageId;
		}
		else
		{
			return 0;
		}
	}
}

int GlobalData::getSectionStageId( int sectionId, int stageId )
{
	return sectionId * 1000 + stageId;
}

int GlobalData::getMaxEnabledStageId()
{
	int maxId = 0;
	std::map<int, StageRecordItem>::iterator it = _stageRecordMap.begin();
	for (; it != _stageRecordMap.end(); ++ it)
	{
		int id = it->first;
		if (it->second.score != 0)
		{
			if (id > maxId)
			{
				maxId = id;
			}
		}
	}

	if (maxId == 0)
	{
		maxId = 1001;
	}
	else
	{
		maxId = getNextSectionStageId(maxId / 1000, maxId % 1000);
	}
	return maxId;
}

void GlobalData::setSelectedTag( int tag )
{
	int oldSelTag = _selectedTag;
	_selectedTag = tag;
	MessageDispatcher::getInstance()->pushMessage(MSG_CHANGESELECTED, oldSelTag, _selectedTag);
}

//////////////////////////////////////////////////////////////////////////
//
float float_mod(float f1, float f2)
{
	int gene = 1;
	if (f1 < 0)
	{
		f1 = -f1;
		gene = -1;
	}

	if (f1 < f2)
		return gene * f1;

	return gene * float_mod(f1 - f2, f2);
}