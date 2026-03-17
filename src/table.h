#pragma once

#include <vector>
#include <variant>
#include <unordered_map>
#include <iostream>
#include <exception>

using Data = std::variant<int, std::string>;

enum ColumnType { INTEGER, TEXT };

struct Column
{
	const std::string name;
	ColumnType type;
};

class Table
{
public:
	void addColumn(const std::string& name, ColumnType type);
	size_t getRowsCount() const;

private:
	const std::string _name;
	std::vector<Column> _columns;
	std::vector<std::vector<Data>> _rows;
};

class Database
{
public:
	const std::string name;
	std::unordered_map<std::string, Table> _tables;

	Database(const std::string& name) : name(name)
	{
	}
	void addTable(const std::string& tableName, const Table& table);
};


ColumnType getColumnType(const std::string& typeStr);
