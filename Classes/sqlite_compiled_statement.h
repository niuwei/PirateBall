#ifndef _BASE_SQLITE_COMPILED_STATEMENT_H_
#define _BASE_SQLITE_COMPILED_STATEMENT_H_

#include <map>
#include <string>


struct sqlite3;
class SQLStatement;

class SqliteStatementCache
{
public:
  SqliteStatementCache();
  explicit SqliteStatementCache(sqlite3* db);

  ~SqliteStatementCache();

  void set_db(sqlite3* db);
  
  SQLStatement* GetStatement(const char* func_name, int func_number, const char* sql)
  {
    return InternalGetStatement(func_name, func_number, sql);
  }

  SQLStatement* GetExistingStatement(const char* func_name, int func_number)
  {
    return InternalGetStatement(func_name, func_number, NULL);
  }

private:
  struct FuncID
  {
    int number;
    std::string name;

    bool operator<(const FuncID& other) const;
  };

  SQLStatement* InternalGetStatement(const char* func_name, int func_number, const char* sql);

  sqlite3* db_;

  typedef std::map<FuncID, SQLStatement*> StatementMap;
  StatementMap statements_;

  SqliteStatementCache(const SqliteStatementCache&);
  void operator=(const SqliteStatementCache&);
};


class SqliteCompiledStatement
{
public:
  SqliteCompiledStatement(const char* func_name, int func_number, SqliteStatementCache& cache, const char* sql);
  ~SqliteCompiledStatement();

  bool is_valid() const { return !!statement_; }

  SQLStatement& operator*();
  SQLStatement* operator->();
  SQLStatement* statement();

private:
  SQLStatement* statement_;

  SqliteCompiledStatement(const SqliteCompiledStatement&);
  void operator=(const SqliteCompiledStatement&);
};

// Creates a compiled statement that has a unique name based on the file and
// line number. Example:
//
//     SQLITE_UNIQUE_STATEMENT(var_name, cache, "SELECT * FROM foo");
//     if (!var_name.is_valid())
//       return oops;
//     var_name->bind_XXX(
//     var_name->step();
//     ...
#define SQLITE_UNIQUE_STATEMENT(var_name, cache, sql) \
    SqliteCompiledStatement var_name(__FILE__, __LINE__, cache, sql)

#endif  // _BASE_SQLITE_COMPILED_STATEMENT_H_
