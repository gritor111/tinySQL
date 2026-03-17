#pragma once

#include <iostream>
#include <variant>
#include <vector>
#include <unordered_map>
#include <stdexcept>

#include "table.h"
#include "parser.h"
#include "printer.h"

class Engine
{
public:
	Engine(const std::string& dbName);
	void executeStatement(const Statement& statement);
	std::unordered_map<std::string, Table> Engine::getTables() const;

private:
	Database _db;

	void executeCreateStatement(const CreateStatement& statement);
};