#ifndef _Store_Sqlite_H_
#define _Store_Sqlite_H_

#include "sqlite3.h"
#include <string>
#include "sqlite_compiled_statement.h"
#include "sqlite_utils.h"

class StoreSqlite
{
public:
	StoreSqlite();
	virtual	~StoreSqlite();

	virtual void Init(const std::string& filename);
	virtual bool Open();
	virtual bool Close();
	virtual bool Delete();
	
	bool SetDBVersion(const int cur_version);

	// Returns the name of the SQLite journal file for |filename|.
	static const std::string JournalFileForFilename(const std::string& filename);

protected:
	// Execute all statements in sql, returning true if every one of
	// them returns SQLITE_OK.
	virtual bool ExecSql(const char* sql);
	virtual bool SetupDatabase();	

	virtual bool CheckCompatibleVersion();
	virtual bool NoTables() = 0;
	virtual bool CreateTables() = 0;
	virtual bool ResetTables() = 0;
	virtual bool UpdateDB(int cur_version) = 0;

	// Calls |corruption_callback_| if non-NULL, always returns false as
	// a convenience to the caller.
	virtual bool OnCorruptDatabase();

	std::string filename_;
	sqlite3 *db_;
	int	db_version_;
	scoped_ptr<SqliteStatementCache> statement_cache_;
};

#endif