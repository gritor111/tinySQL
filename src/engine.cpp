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
	{
		executeInsertStatement(static_cast<const InsertStatement&>(statement));
		break;
	}
	case StatementType::SELECT:
	{
		executeSelectStatement(static_cast<const SelectStatement&>(statement));
		break;
	}
	case StatementType::DELETE:
	{
		executeDeleteStatement(static_cast<const DeleteStatement&>(statement));
		break;
	}
	case StatementType::DROP:
		executeDropStatement(static_cast<const DropStatement&>(statement));
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
		if (!doTypesMatch(cols[i].type, statement.values[i]))
		{
			throw std::runtime_error("Type Error: Column '" + cols[i].name + "' expects type " + getTypeString(cols[i].type) + ".");
		}
	}

	table.addRow(statement.values);

	std::cout << "Succesfully added 1 row to table '" << statement.tableName << "'!" << std::endl;
}


/*
Executes the select statement, prints retrieved rows.
input: select statement
output: none
*/
void Engine::executeSelectStatement(const SelectStatement& statement)
{
	const Table& table = _db.getTable(statement.tableName);

	// validate all columns actually exist
	std::vector<Column> tableColumns = table.getColumns();
	std::vector<size_t> keepIndexes{}; // for later

	for (const std::string& queryColumnName: statement.columnNames)
	{
		// allow for queries such as SELECT *, name FROM users
		if (queryColumnName == "*")
		{
			for (size_t i = 0; i < tableColumns.size(); i++)
			{
				keepIndexes.emplace_back(i);
			}
			continue;
		}

		auto it = std::find_if(tableColumns.begin(), tableColumns.end(), [queryColumnName](const Column& column) {
			return column.name == queryColumnName;
		});

		if (it == tableColumns.end())
		{
			throw std::runtime_error("Select Error: Table '" + statement.tableName + "' does not have a column named '" + queryColumnName + "'.");
		}

		keepIndexes.emplace_back(std::distance(tableColumns.begin(), it));
	}

	std::vector<std::vector<Data>> selectedRows = table.getRows();
	if (statement.condition.has_value())
	{
		size_t conditionColumnIdx = 0;
		Condition con = statement.condition.value();
		const std::string& columnName = con.columnName;
		auto it = std::find_if(tableColumns.begin(), tableColumns.end(), [columnName](const Column& column) {
			return column.name == columnName;
		});
		
		if (it == tableColumns.end())
		{
			throw std::runtime_error("Where Error: Column '" + columnName + "' does not exist in table '" + statement.tableName + "'.");
		}

		conditionColumnIdx = std::distance(tableColumns.begin(), it);
		if (!doTypesMatch(tableColumns[conditionColumnIdx].type, con.value))
		{
			throw std::runtime_error("Where Error: Column '" + columnName + "' expects " + getTypeString(tableColumns[conditionColumnIdx].type) + ".");
		}

		selectedRows.clear();
		for (auto& row : table.getRows())
		{
			// row match
			if (evaluate(row[conditionColumnIdx], con.op, con.value))
			{
				selectedRows.emplace_back(row);
			}
		}
	}

	std::vector<std::string> colNames{};

	// filter for only wanted columns
	std::for_each(keepIndexes.begin(), keepIndexes.end(), [&](const size_t idx) { colNames.emplace_back(tableColumns[idx].name); });
	std::vector<std::vector<Data>> filteredRows{};
	for (const std::vector<Data>& row : selectedRows)
	{
		std::vector<Data> filteredRow{};
		std::for_each(keepIndexes.begin(), keepIndexes.end(), [&](const size_t columnIdx) { filteredRow.emplace_back(row[columnIdx]); });
		filteredRows.emplace_back(filteredRow);
	}
	printSelectResult(colNames, filteredRows);
}

/*
Executes the delete statement, deletes rows where condition applies.
input: drop statement
output: none
*/
void Engine::executeDeleteStatement(const DeleteStatement& statement)
{
	Table& table = _db.getTable(statement.tableName);
	const std::vector<Column>& tableColumns = table.getColumns();
	
	const Condition con = statement.condition;
	const std::string& columnName = con.columnName;
	auto it = std::find_if(tableColumns.begin(), tableColumns.end(), [columnName](const Column& column) {
		return column.name == columnName;
		});

	if (it == tableColumns.end())
	{
		throw std::runtime_error("Where Error: Column '" + columnName + "' does not exist in table '" + statement.tableName + "'.");
	}

	size_t conditionColumnIdx = std::distance(tableColumns.begin(), it);
	if (!doTypesMatch(tableColumns[conditionColumnIdx].type, con.value))
	{
		throw std::runtime_error("Where Error: Column '" + columnName + "' expects " + getTypeString(tableColumns[conditionColumnIdx].type) + ".");
	}

	size_t deletedCount = table.removeRows([&](const std::vector<Data>& row) {
		return evaluate(row[conditionColumnIdx], con.op, con.value);
	});

	std::cout << "Delete succesfull, " << deletedCount << " rows deleted!" << std::endl;
}

/*
Drops a table from the database.
input: drop statement
output: none
*/
void Engine::executeDropStatement(const DropStatement& statement)
{
	_db.removeTable(statement.tableName);
	std::cout << "Succesfully dropped table '" << statement.tableName << "'!" << std::endl;
}

/*
Helper function to match a value type (int or string) versus an sql type (INTEGER or TEXT)
input: expected type and value to check
output: if they match, int and INTEGER & string and TEXT
*/
static bool doTypesMatch(const ColumnType expectedType, const Data& value)
{
	if (expectedType == ColumnType::INTEGER && !std::holds_alternative<int>(value))
	{
		return false;
	}
	else if (expectedType == ColumnType::TEXT && !std::holds_alternative<std::string>(value))
	{
		return false;
	}

	return true;
}

/*
Function to evaluate conditions in select, expects lhs and rhs to be the same type, returns false if they are not
input: lhs and rhs of the same type, operator type
output: evaluation of the comparasion
*/
static bool evaluate(const Data& lhs, OpType op, const Data& rhs)
{
	switch (op)
	{
	case OpType::EQ:
		return lhs == rhs;
	case OpType::GT:
		return lhs > rhs;
	case OpType::LT:
		return lhs < rhs;
	case OpType::GTE:
		return lhs >= rhs;
	case OpType::LTE:
		return lhs <= rhs;
	case OpType::NEQ:
		return lhs != rhs;
	default:
		return false;
	}
}