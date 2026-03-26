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
		std::cout << "  -" << name << " (" << table.getRows().size() << " rows)" << std::endl;
	}
}

/*
Prints slecet result given column names and rows
*/
void printSelectResult(const std::vector<std::string>& columnNames, const std::vector<std::vector<Data>>& rows)
{
	if (rows.empty())
	{
		std::cout << "0 rows were found." << std::endl;
		return;
	}

	std::vector<size_t> columnWidths(columnNames.size());
	std::transform(columnNames.begin(), columnNames.end(), columnWidths.begin(), [](const std::string& name) { return name.size(); });

	for (const std::vector<Data>& row : rows)
	{
		for (size_t i = 0; i < row.size(); i++)
		{
			columnWidths[i] = std::max(columnWidths[i], dataToString(row[i]).size());
		}
	}

	// print column header
	auto printLineSeperator = [&]() {
		std::cout << "+";
		std::for_each(columnWidths.begin(), columnWidths.end(), [](const size_t width) {
			std::cout << std::string(width + 2, '-') << "+";
		});
		std::cout << std::endl;
	};

	printLineSeperator();
	std::cout << "|";
	for (size_t i = 0; i < columnNames.size(); i++) 
	{
		std::cout << " " << std::left << std::setw(columnWidths[i]) << columnNames[i] << " |";
	}
	std::cout << std::endl;
	printLineSeperator();

	// print rows
	for (const std::vector<Data>& row : rows)
	{
		std::cout << "|";
		for (size_t i = 0; i < row.size(); i++)
		{
			std::cout << " " << std::left << std::setw(columnWidths[i]) << dataToString(row[i]) << " |";
		}
		std::cout << std::endl;
	}
	printLineSeperator();
}

/*
Helper to convert data to string
input: data
output: string of data
*/
static std::string dataToString(const Data& value)
{
	if (std::holds_alternative<int>(value))
	{
		return std::to_string(std::get<int>(value));
	}

	return std::get<std::string>(value);
}