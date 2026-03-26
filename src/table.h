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
	void addRow(const std::vector<Data>& row);
	std::vector<std::vector<Data>> getRows() const;
	std::vector<Column> getColumns() const;

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
	Table& getTable(const std::string& tableName);
	void removeTable(const std::string& tableName);
};


ColumnType getColumnType(const std::string& typeStr);
std::string getTypeString(const ColumnType type);
