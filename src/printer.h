#pragma once

#include <iostream>
#include <string>
#include <algorithm>
#include <iomanip>

#include "table.h"

void printTables(const std::unordered_map<std::string, Table>& tables);
void printSelectResult(const std::vector<std::string>& columnNames, const std::vector<std::vector<Data>>& rows);

static std::string dataToString(const Data& value);