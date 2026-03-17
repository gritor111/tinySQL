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
Function that adds a column to a table.
input: column name and type
output: none
*/
void Table::addColumn(const std::string& name, ColumnType type)
{
	_columns.emplace_back(Column{ name, type });
}


/*
Function that gets how many rows the table has.
input: none
output: count of rows in table
*/
size_t Table::getRowsCount() const
{
	return _rows.size();
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
