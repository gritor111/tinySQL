#include "printer.h"

/*
Prints all table names in a database.
input: vector of tables
output: none
*/
void printTables(const std::unordered_map<std::string, Table>& tables)
{
	for (const auto& [name, table] : tables)
	{
		std::cout << "  -" << name << " (" << table.getRowsCount() << " rows)" << std::endl;
	}
}
