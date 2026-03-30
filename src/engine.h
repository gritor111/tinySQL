#pragma once

#include <iostream>
#include <variant>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <numeric>

#include "table.h"
#include "parser.h"
#include "printer.h"

class Engine
{
public:
	Engine(const std::string& dbName);
	void executeStatement(const Statement& statement);
	SelectResult executeSelectStatement(const SelectStatement& statement);
	std::unordered_map<std::string, Table> Engine::getTables() const;

private:
	Database _db;

	void executeCreateStatement(const CreateStatement& statement);
	void executeInsertStatement(const InsertStatement& statement);
	size_t executeDeleteStatement(const DeleteStatement& statement);
	void executeDropStatement(const DropStatement& statement);
};

static bool doTypesMatch(const ColumnType expectedType, const Data& value);
static bool evaluate(const Data& lhs, OpType op, const Data& rhs);