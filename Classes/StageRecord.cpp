#include "StageRecord.h"
#include "sqlite_utils.h"

const int kStageRecordVersion = 1;

StageRecord::StageRecord()
{
	db_version_ = kStageRecordVersion;
}

StageRecord::~StageRecord()
{

}

bool StageRecord::writeRecord( int id, int spendTime, int score )
{
	assert(db_);

	SQLTransaction transaction(db_);
	if (transaction.Begin() != SQLITE_OK)
		return false;

	SQLITE_UNIQUE_STATEMENT(pending_rd, *statement_cache_,
		"SELECT id FROM stage_record "
		"WHERE id = ?");
	if (!pending_rd.is_valid())
		return false;

	pending_rd->bind_int(0, id);
	int rv = pending_rd->step();
	if (rv == SQLITE_CORRUPT)
		return OnCorruptDatabase();

	if (rv == SQLITE_DONE)
	{
		SQLITE_UNIQUE_STATEMENT(statement, *statement_cache_,
			"INSERT INTO stage_record "
			"(id, spend_time, score) "
			"VALUES (?, ?, ?)");
		if (!statement.is_valid())
			return false;

		statement->bind_int(0, id);
		statement->bind_int(1, spendTime);
		statement->bind_int(2, score);

		int rv = statement->step();
		if (rv == SQLITE_CORRUPT)
			return OnCorruptDatabase();

		if (rv != SQLITE_DONE)
			return false;
	}
	else if (rv == SQLITE_ROW)
	{
		SQLITE_UNIQUE_STATEMENT(statement, *statement_cache_,
			"UPDATE stage_record SET "
			"spend_time = ?, score = ? "
			"WHERE id = ?");
		if (!statement.is_valid())
			return false;

		statement->bind_int(0, spendTime);
		statement->bind_int(1, score);
		statement->bind_int(2, id);

		int rv = statement->step();
		if (rv == SQLITE_CORRUPT)
			return OnCorruptDatabase();

		if (rv != SQLITE_DONE)
			return false;
	}
	else
	{
		return false;
	}

	if (transaction.Commit() != SQLITE_OK)
		return false;

	return true;
}

bool StageRecord::getRecords( std::vector<StageRecordItem>* items, int offset, int rows )
{
	assert(db_);

	if (items == NULL || offset < 0)
		return false;

	SQLTransaction transaction(db_);
	if (transaction.Begin() != SQLITE_OK)
		return false;

	if (rows < 0)
		rows = -1;

	SQLITE_UNIQUE_STATEMENT(statement, *statement_cache_,
		"SELECT * FROM stage_record LIMIT ?, ?");
	if (!statement.is_valid())
		return false;

	statement->bind_int(0, offset);
	statement->bind_int(1, rows);

	int rv;
	while ((rv = statement->step()) == SQLITE_ROW)
	{
		StageRecordItem item;
		item.id			= statement->column_int(0);
		item.spendTime	= statement->column_int(1);
		item.score		= statement->column_int(2);
		items->push_back(item);
	}

	if (rv == SQLITE_CORRUPT)
		return OnCorruptDatabase();

	if (transaction.Commit() != SQLITE_OK)
		return false;

	return (rv == SQLITE_DONE);
}

bool StageRecord::deleteRecord( int id )
{
	assert(db_);

	SQLTransaction transaction(db_);
	if (transaction.Begin() != SQLITE_OK)
		return false;

	SQLITE_UNIQUE_STATEMENT(statement, *statement_cache_,
		"DELETE FROM stage_record WHERE id = ?");
	if (!statement.is_valid())
		return false;

	statement->bind_int(0, id);

	int rv = statement->step();
	if (rv == SQLITE_CORRUPT)
		return OnCorruptDatabase();

	if (rv != SQLITE_DONE)
		return false;

	if (transaction.Commit() != SQLITE_OK)
		return false;

	return true;
}

bool StageRecord::NoTables()
{
	return !sqlite_utils::DoesSqliteTableExist(db_, "stage_record");
}

// 外层调用已支持事务
bool StageRecord::CreateTables()
{
	assert(db_);

	if (!ExecSql("CREATE TABLE stage_record ( "
		"id INTEGER PRIMARY KEY NOT NULL, "
		"spend_time INTEGER, "
		"score INTEGER)"))
		return false;

	return true;
}

bool StageRecord::ResetTables()
{
	assert(db_);

	SQLTransaction transaction(db_);
	if (transaction.Begin() != SQLITE_OK)
		return false;

	if (!ExecSql("DELETE FROM stage_record"))
		return false;

	if (transaction.Commit() != SQLITE_OK)
		return false;

	return true;
}

bool StageRecord::UpdateDB( int cur_version )
{
	if (cur_version < 0)
		return false;

	return true;
}
