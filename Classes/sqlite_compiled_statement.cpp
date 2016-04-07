#include "sqlite_compiled_statement.h"
#include <assert.h>
#include "stl_util-inl.h"
#include "sqlite_utils.h"
#include "sqlite3.h"


// SqliteStatementCache -------------------------------------------------------

SqliteStatementCache::SqliteStatementCache() : db_(NULL)
{
}

SqliteStatementCache::SqliteStatementCache(sqlite3* db) : db_(db)
{
}

SqliteStatementCache::~SqliteStatementCache()
{
	STLDeleteContainerPairSecondPointers(statements_.begin(), statements_.end());
	statements_.clear();
	db_ = NULL;
}

void SqliteStatementCache::set_db(sqlite3* db)
{
	assert(!db_);
	db_ = db;
}

SQLStatement* SqliteStatementCache::InternalGetStatement(const char* func_name,
														 int func_number, const char* sql)
{
	FuncID id;
	id.name = func_name;
	id.number = func_number;

	StatementMap::const_iterator found = statements_.find(id);
	if (found != statements_.end())
		return found->second;

	if (!sql)
		return NULL;  // Don't create a new statement when we were not given SQL.

	// Create a new statement.
	SQLStatement* statement = new SQLStatement();
	if (statement->prepare(db_, sql) != SQLITE_OK)
	{
		const char* err_msg = sqlite3_errmsg(db_);		
		return NULL;
	}

	statements_[id] = statement;
	return statement;
}

bool SqliteStatementCache::FuncID::operator<(const FuncID& other) const {
	// Compare numbers first since they are faster than strings and they are
	// almost always unique.
	if (number != other.number)
		return number < other.number;
	return name < other.name;
}

// SqliteCompiledStatement ----------------------------------------------------

SqliteCompiledStatement::SqliteCompiledStatement(const char* func_name, int func_number,
												 SqliteStatementCache& cache, const char* sql)
{
	statement_ = cache.GetStatement(func_name, func_number, sql);
}

SqliteCompiledStatement::~SqliteCompiledStatement()
{
	if (statement_)
		statement_->reset();
}

SQLStatement& SqliteCompiledStatement::operator*()
{
	assert(statement_);
	return *statement_;
}

SQLStatement* SqliteCompiledStatement::operator->()
{
	assert(statement_);
	return statement_;
}

SQLStatement* SqliteCompiledStatement::statement()
{
	assert(statement_);
	return statement_;
}
