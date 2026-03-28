#include "table.h"

/*
Function to get enum type of column from str of type. throws on unkown type
input: string of column type
output: enum of type
*/
ColumnType getColumnType(const std::string& typeStr)
{
	if (typeStr == "INTEGER")
	{
		return ColumnType::INTEGER;
	}
	else if (typeStr == "TEXT")
	{
		return ColumnType::TEXT;
	}

	throw std::runtime_error("Unkown column type " + typeStr + ".");
}

/*
Return string representation of a column type
input: column type
output: string of type
*/
std::string getTypeString(const ColumnType type)
{
	switch (type)
	{
	case ColumnType::INTEGER:
		return "INTEGER";
	case ColumnType::TEXT:
		return "TEXT";
	default:
		return "UNKOWN TYPE";
	}
}


/*
Function that adds a column to a table.
input: column name and type
output: none
*/
void Table::addColumn(const std::string& name, ColumnType type)
{
	_columns.emplace_back(Column{ name, type });
}

/*
Adds a row to the table.
input: vector of data
output: none
*/
void Table::addRow(const std::vector<Data>& row)
{
	_rows.emplace_back(row);
}

/*
Getter for rows
input: none
output: rows in table
*/
std::vector<std::vector<Data>> Table::getRows() const
{
	return _rows;
}

std::vector<Column> Table::getColumns() const
{
	return _columns;
}

/*
Function that removes a row if it matches the given lambda.
input: lambda to validate rows against
output: how many rows were removed
*/
size_t Table::removeRows(std::function<bool(const std::vector<Data>&)> predicate)
{
	auto it	= std::remove_if(_rows.begin(), _rows.end(), predicate);

	const size_t deletedCount = std::distance(it, _rows.end());

	_rows.erase(it, _rows.end());

	return deletedCount;
}


/*
Adds a table to the database if name does not exist
*/
void Database::addTable(const std::string& tableName, const Table& table)
{
	if (!_tables.insert({ tableName, table }).second)
	{
		throw std::runtime_error("Error: table with name '" + tableName + "' already exists.");
	}
}


/*
Returns table struct given a name, nullopt if doesnt exist
input: name of table
output: optional table
*/
Table& Database::getTable(const std::string& tableName)
{
	auto it = _tables.find(tableName);
	if (it == _tables.end())
	{
		throw std::runtime_error("Table not found: " + tableName + '.');
	}

	return it->second;
}

/*
Removes a table from the database, throws error if table does not exist
input: table name to remove
output: none
*/
void Database::removeTable(const std::string& tableName)
{
	if (!_tables.erase(tableName))
	{
		throw std::runtime_error("Drop Error: Table '" + tableName + "' does not exist.");
	}
}
