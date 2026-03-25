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
	case StatementType::INSERT:
		executeInsertStatement(static_cast<const InsertStatement&>(statement));
		break;
		
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

	std::cout << "Succesfully created table '" << statement.tableName << "'!" << std::endl;
}

/*
Executes the insert statement, inserts a row into a table.
input: insert statement
output: none
*/
void Engine::executeInsertStatement(const InsertStatement& statement)
{
	Table& table = _db.getTable(statement.tableName);

	std::vector<Column> cols = table.getColumns();

	// validate column count matches
	if (statement.values.size() != cols.size())
	{
		throw std::runtime_error("Insert Error: Column count mismatch, expected " + std::to_string(cols.size()) + " got " + std::to_string(statement.values.size()) + ".");
	}

	// validate type for each column
	for (size_t i = 0; i < cols.size(); i++)
	{
		const ColumnType expectedType = cols[i].type;
		const Data& value = statement.values[i];

		if (expectedType == ColumnType::INTEGER && !std::holds_alternative<int>(value))
		{
			throw std::runtime_error("Type Error: Column '" + cols[i].name + "' expects INTEGER.");
		}
		else if (expectedType == ColumnType::TEXT && !std::holds_alternative<std::string>(value))
		{
			throw std::runtime_error("Type Error: Column '" + cols[i].name + "' expects TEXT.");
		}
	}

	table.addRow(statement.values);

	std::cout << "Succesfully added 1 row to table '" << statement.tableName << "'!" << std::endl;
}
