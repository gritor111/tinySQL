#include <gtest/gtest.h>

#include "engine.h"

TEST(EngineTest, FullEngine)
{
	Engine engine;

	CreateStatement create{};
	create.tableName = "users";
	create.columnsData = {
		{"name",    "TEXT"},
		{"age",     "INTEGER"},
		{"balance", "INTEGER"}
	};

	engine.executeStatement(create);
	ASSERT_EQ(engine.getTables().size(), 1);

	const std::vector<std::vector<Data>> toInsert = {
		{"Ayellet", 21, 1000},
		{"Bar", 18, 20},
		{"Gargamel", 73, 25000}
	};

	InsertStatement insert{};
	insert.tableName = "users";

	for (const std::vector<Data>& row : toInsert)
	{
		insert.values = row;
		engine.executeStatement(insert);
	}
	
	WhereClause where{};

	std::vector<Condition> currGroup{};

	Condition con{};
	con.columnName = "age";
	con.op = OpType::GT;
	con.value = 18;
	currGroup.emplace_back(con);

	con.columnName = "balance";
	con.op = OpType::LTE;
	con.value = 1000;
	currGroup.emplace_back(con);
	where.conditionGroups.emplace_back(currGroup);
	currGroup.clear();

	con.columnName = "name";
	con.op = OpType::EQ;
	con.value = "Gargamel";
	currGroup.emplace_back(con);
	where.conditionGroups.emplace_back(currGroup);

	OrderClause order{};
	order.columnName = "balance";
	order.isAsc = false;

	SelectStatement select{};
	select.tableName = "users";
	select.columnNames = {
		"*",
		"balance"
	};
	select.where = where;
	select.order = order;

	const SelectResult& result = engine.executeSelectStatement(select);

	ASSERT_EQ(result.columnNames.size(), 4);
	const std::vector<std::string> expectedColumnNames = {
		"name",
		"age",
		"balance",
		"balance"
	};

	for (size_t i = 0; i < expectedColumnNames.size(); i++)
	{
		EXPECT_EQ(result.columnNames[i], expectedColumnNames[i]) << "Column mismatch at index " << i;
	}

	ASSERT_EQ(result.rows.size(), 2);
	const std::vector<std::vector<Data>> expectedRows = {
		{"Gargamel", 73, 25000, 25000},
		{"Ayellet", 21, 1000, 1000}
	};

	for (size_t i = 0; i < expectedRows.size(); i++)
	{
		std::vector<Data> row = result.rows[i];
		ASSERT_EQ(row.size(), 4);
		std::vector<Data> expectedRow = expectedRows[i];

		for (size_t j = 0; j < expectedRow.size(); j++)
		{
			EXPECT_EQ(row[j], expectedRow[j]) << "Value mismatch at row " << i << " column " << j;
		}
	}
}