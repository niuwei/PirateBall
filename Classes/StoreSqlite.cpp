#include "StoreSqlite.h"
#include <assert.h>
#include "cocos2d.h"
#include "sqlite_utils.h"
#include "sqlite_compiled_statement.h"

USING_NS_CC;


StoreSqlite::StoreSqlite()
	: db_(NULL)
	, statement_cache_(NULL)
	, db_version_(0)
{

}

StoreSqlite::~StoreSqlite()
{

}

void StoreSqlite::Init( const std::string& filename )
{
	filename_ = filename;
}

bool StoreSqlite::Open()
{
	// This case should never happen, but if it does we shouldn't leak handles.
	assert(!db_);

	if (sqlite3_open(filename_.c_str(), &db_) != SQLITE_OK)
	{
		sqlite3_close(db_);
		db_ = NULL;
		return false;
	}

	// Run the database in exclusive mode. Nobody else should be accessing the
	// database while we're running, and this will give somewhat improved perf.
	ExecSql("PRAGMA locking_mode = EXCLUSIVE");
	ExecSql("PRAGMA cache_size = 8000");
	ExecSql("PRAGMA synchronous = OFF");
	ExecSql("PRAGMA temp_store = MEMORY");

	statement_cache_.reset(new SqliteStatementCache(db_));

	if(NoTables())
	{
		return SetupDatabase();
	}		

	return CheckCompatibleVersion();
}

bool StoreSqlite::Close()
{
	if (!db_)
		return true;

	statement_cache_.reset();  // Must free statements before closing DB.
	bool result = sqlite3_close(db_) == SQLITE_OK;
	db_ = NULL;

	return result;
}

bool StoreSqlite::Delete()
{
	if (!Close())
	{
		return false;
	}

	// Just in case, delete the journal file, because associating the
	// wrong journal file with a database is very bad.
	const std::string journal_file = JournalFileForFilename(filename_);
	if (!FileUtils::getInstance()->removeFile(journal_file) &&
		FileUtils::getInstance()->isFileExist(journal_file))
	{
		return false;
	}

	if (!FileUtils::getInstance()->removeFile(filename_) &&
		FileUtils::getInstance()->isFileExist(filename_))
	{
		return false;
	}

	return true;
}

bool StoreSqlite::SetDBVersion( const int cur_version )
{
	if (cur_version < 0)
	{
		return false;
	}

	assert(db_);

	SQLTransaction transaction(db_);
	if (transaction.Begin() != SQLITE_OK)
		return false;

	std::stringstream ss;
	ss << "PRAGMA user_version = " << cur_version;
	const std::string version = ss.str();		
	if (!ExecSql(version.c_str()))
		return false;

	if (transaction.Commit() != SQLITE_OK)
		return false;

	return true;
}

const std::string StoreSqlite::JournalFileForFilename( const std::string& filename )
{
	return filename + "-journal";
}

bool StoreSqlite::ExecSql( const char* sql )
{
	assert(db_);

	int rv = sqlite3_exec(db_, sql, NULL, NULL, NULL);
	if (rv == SQLITE_CORRUPT)
		return OnCorruptDatabase();

	assert(rv == SQLITE_OK);
	return true;
}

bool StoreSqlite::SetupDatabase()
{
	assert(db_);

	SQLTransaction transaction(db_);
	if (transaction.Begin() != SQLITE_OK)
		return false;

	if (!CreateTables())
		return false;

	// PRAGMA does not support bind parameters...
	assert(db_version_ != 0);

	std::stringstream ss;
	ss << "PRAGMA user_version = " << db_version_;
	const std::string version = ss.str();		
	if (!ExecSql(version.c_str()))
		return false;

	if (transaction.Commit() != SQLITE_OK)
		return false;

	return true;
}

bool StoreSqlite::CheckCompatibleVersion()
{
	assert(db_);
//	assert(db_version_ != 0);// 只要到这一步就无法找到表

	SQLITE_UNIQUE_STATEMENT(statement, *statement_cache_,
		"PRAGMA user_version");
	if (!statement.is_valid())
		return false;

	int result = statement->step();
	if (result != SQLITE_ROW)
		return false;

	int cur_version = statement->column_int(0);
	if ( cur_version != db_version_)
	{
		return UpdateDB(cur_version);
	}
	return true;
}

bool StoreSqlite::OnCorruptDatabase()
{
	return false;
}
