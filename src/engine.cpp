#include "engine.h"


Engine::Engine(const std::string& dbName) : _db(dbName)
{
}


/*
Recieves a statement object and dispatches it to the correct execute function based off of its type
input: statement object
output: none
*/
void Engine::executeStatement(const Statement& statement)
{
	switch (statement.type)
	{
	case StatementType::CREATE:
	{
		executeCreateStatement(static_cast<const CreateStatement&>(statement));
		break;
	}
		
	}
}

/*
Getter for database tables
input: none
output: tables of database
*/
std::unordered_map<std::string, Table> Engine::getTables() const
{
	return _db._tables;
}


/*
Executes the create statement, adds a new table to the database.
throws error if there is an unkown column type
input: create statement
output: none
*/
void Engine::executeCreateStatement(const CreateStatement& statement)
{
	Table newTable;

	for (const auto& [name, typeStr] : statement.columnsData)
	{
		ColumnType type = getColumnType(typeStr);

		newTable.addColumn(name, type);
	}
	
	_db.addTable(statement.tableName, newTable);
}
