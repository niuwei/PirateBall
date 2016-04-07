#ifndef _Stage_Record_H_
#define _Stage_Record_H_

#include "StoreSqlite.h"
#include <vector>
#include "GlobalDefined.h"

class StageRecord : public StoreSqlite
{
public:
	StageRecord();
	virtual ~StageRecord();

	bool writeRecord(int id, int spendTime, int score);
	bool getRecords(std::vector<StageRecordItem>* items, int offset, int rows);
	bool deleteRecord(int id);

protected:
	virtual bool NoTables();
	virtual bool CreateTables();
	virtual bool ResetTables();
	virtual bool UpdateDB(int cur_version);

};


#endif