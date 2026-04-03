#include "engine.h"


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
		const CreateStatement& createStatement = static_cast<const CreateStatement&>(statement);
		executeCreateStatement(createStatement);
		std::cout << "Succesfully created table '" << createStatement.tableName << "'!" << std::endl;
		break;
	}
	case StatementType::INSERT:
	{
		const InsertStatement insertStatement = static_cast<const InsertStatement&>(statement);
		executeInsertStatement(insertStatement);
		std::cout << "Succesfully added 1 row to table '" << insertStatement.tableName << "'!" << std::endl;
		break;
	}
	case StatementType::SELECT:
	{
		const SelectResult& result = executeSelectStatement(static_cast<const SelectStatement&>(statement));
		printSelectResult(result.columnNames, result.rows);
		break;
	}
	case StatementType::DELETE:
	{
		const DeleteStatement& deleteStatement = static_cast<const DeleteStatement&>(statement);
		const size_t deletedCount = executeDeleteStatement(deleteStatement);
		std::cout << "Delete succesfull, " << deletedCount << " rows deleted!" << std::endl;
		break;
	}
	case StatementType::DROP:
	{
		const DropStatement& dropStatement = static_cast<const DropStatement&>(statement);
		executeDropStatement(dropStatement);
		std::cout << "Succesfully dropped table '" << dropStatement.tableName << "'!" << std::endl;
		break;
	}
	default:
	{
		throw std::runtime_error("Unkown statement type.");
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
}


/*
Executes the select statement, prints retrieved rows.
input: select statement
output: none
*/
SelectResult Engine::executeSelectStatement(const SelectStatement& statement)
{
	const Table& table = _db.getTable(statement.tableName);

	// validate all columns actually exist
	std::vector<Column> tableColumns = table.getColumns();
	std::vector<size_t> keepIndexes{}; // for later
	std::vector<std::string> colNames{};

	for (const std::string& queryColumnName: statement.columnNames)
	{
		// allow for queries such as SELECT *, name FROM users
		if (queryColumnName == "*")
		{
			for (size_t i = 0; i < tableColumns.size(); i++)
			{
				keepIndexes.emplace_back(i);
				colNames.emplace_back(tableColumns[i].name);
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
		colNames.emplace_back(it->name);
	}

	std::vector<std::vector<Data>> rows = table.getRows();
	std::vector<std::vector<Data>> selectedRows;
	if (statement.where.has_value())
	{
		std::copy_if(rows.begin(), rows.end(), std::back_inserter(selectedRows), [&](const std::vector<Data>& row) {
			return evaluateWhereClause(table, row, statement.where.value());
		});
	}
	else
	{
		selectedRows = rows;
	}

	// handle order by if exists
	if (statement.order.has_value())
	{
		const std::string& columnName = statement.order->columnName;
		const size_t orderColumnIdx = table.getColumnIdx(columnName);

		std::sort(selectedRows.begin(), selectedRows.end(), [&](const std::vector<Data>& row1, const std::vector<Data>& row2) {
			return statement.order->isAsc ? row1[orderColumnIdx] < row2[orderColumnIdx] : row1[orderColumnIdx] > row2[orderColumnIdx];
		});
	}

	// filter for only wanted columns
	std::vector<std::vector<Data>> filteredRows{};
	for (const std::vector<Data>& row : selectedRows)
	{
		std::vector<Data> filteredRow{};
		std::for_each(keepIndexes.begin(), keepIndexes.end(), [&](const size_t columnIdx) { filteredRow.emplace_back(row[columnIdx]); });
		filteredRows.emplace_back(filteredRow);
	}

	return { colNames, filteredRows };
}

/*
Executes the delete statement, deletes rows where condition applies.
input: drop statement
output: amount of rows deleted
*/
size_t Engine::executeDeleteStatement(const DeleteStatement& statement)
{
	Table& table = _db.getTable(statement.tableName);
	const std::vector<Column>& tableColumns = table.getColumns();
	
	const WhereClause& where = statement.where;


	return table.removeRows([&](const std::vector<Data>& row) {
		return evaluateWhereClause(table, row, where);
	});
}

/*
Drops a table from the database.
input: drop statement
output: none
*/
void Engine::executeDropStatement(const DropStatement& statement)
{
	_db.removeTable(statement.tableName);
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
Evaluates a where clause (condition groups) against a row and returns result.
input: row to check and where clause
output: if all conditions are true
*/
static bool evaluateWhereClause(const Table& table, const std::vector<Data>& row, const WhereClause& where)
{
	const std::vector<Column> columns = table.getColumns();
	for (const std::vector<Condition> group : where.conditionGroups)
	{
		bool isAllTrue = true;
		for (const Condition& con : group)
		{
			const size_t columnIdx = table.getColumnIdx(con.columnName);
			const Data& rowValue = row[columnIdx];

			if (!doTypesMatch(columns[columnIdx].type, con.value))
			{
				throw std::runtime_error("Where Error: Column '" + con.columnName + "' expects " + getTypeString(columns[columnIdx].type) + ".");
			}

			if (!evaluate(row[columnIdx], con.op, con.value))
			{
				isAllTrue = false;
				break;
			}
		}

		if (isAllTrue)
		{
			return true;
		}
	}

	return false;
}

/*
Function to evaluate condition, expects lhs and rhs to be the same type, returns false if they are not
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