#include <gtest/gtest.h>

#include "parser.h"

TEST(ParserTest, Create)
{
    std::vector<Token> tokens = {
        {TokenType::KEYWORD,     "CREATE"},
        {TokenType::KEYWORD,     "TABLE"},
        {TokenType::IDENTIFIER,  "users"},
        {TokenType::PUNCTUATION, "("},
        {TokenType::IDENTIFIER,  "id"},
        {TokenType::IDENTIFIER,  "INTEGER"},
        {TokenType::PUNCTUATION, ","},
        {TokenType::IDENTIFIER,  "name"},
        {TokenType::IDENTIFIER,  "TEXT"},
        {TokenType::PUNCTUATION, ","},
        {TokenType::IDENTIFIER,  "age"},
        {TokenType::IDENTIFIER,  "INTEGER"},
        {TokenType::PUNCTUATION, ")"},
    };

    Parser parser(tokens);
    std::unique_ptr<Statement> base = parser.parse();
    ASSERT_NE(dynamic_cast<CreateStatement*>(base.get()), nullptr);
    CreateStatement& create = static_cast<CreateStatement&>(*base);

    EXPECT_EQ(create.tableName, "users");
    ASSERT_EQ(create.columnsData.size(), 3);
    EXPECT_EQ(create.columnsData[0].first, "id");   EXPECT_EQ(create.columnsData[0].second, "INTEGER");
    EXPECT_EQ(create.columnsData[1].first, "name"); EXPECT_EQ(create.columnsData[1].second, "TEXT");
    EXPECT_EQ(create.columnsData[2].first, "age");  EXPECT_EQ(create.columnsData[2].second, "INTEGER");
}

TEST(ParserTest, Insert)
{
    std::vector<Token> tokens = {
        {TokenType::KEYWORD,     "INSERT"},
        {TokenType::KEYWORD,     "INTO"},
        {TokenType::IDENTIFIER,  "users"},
        {TokenType::KEYWORD,     "VALUES"},
        {TokenType::PUNCTUATION, "("},
        {TokenType::LITERAL,     "1"},
        {TokenType::PUNCTUATION, ","},
        {TokenType::LITERAL,     "'Ayellet'"},
        {TokenType::PUNCTUATION, ","},
        {TokenType::LITERAL,     "30"},
        {TokenType::PUNCTUATION, ")"},
    };

    Parser parser(tokens);
    std::unique_ptr<Statement> base = parser.parse();
    ASSERT_NE(dynamic_cast<InsertStatement*>(base.get()), nullptr);
    InsertStatement& insert = static_cast<InsertStatement&>(*base);

    EXPECT_EQ(insert.tableName, "users");
    ASSERT_EQ(insert.values.size(), 3);
    EXPECT_EQ(std::get<INTEGER>(insert.values[0]), 1);
    EXPECT_EQ(std::get<std::string>(insert.values[1]), "Ayellet");
    EXPECT_EQ(std::get<INTEGER>(insert.values[2]), 30);
}

TEST(ParserTest, Select)
{
    std::vector<Token> tokens = {
        {TokenType::KEYWORD,     "SELECT"},
        {TokenType::IDENTIFIER,  "id"},
        {TokenType::PUNCTUATION, ","},
        {TokenType::IDENTIFIER,  "name"},
        {TokenType::PUNCTUATION, ","},
        {TokenType::IDENTIFIER,  "age"},
        {TokenType::KEYWORD,     "FROM"},
        {TokenType::IDENTIFIER,  "users"},
        {TokenType::KEYWORD,     "WHERE"},
        {TokenType::IDENTIFIER,  "age"},
        {TokenType::OPERATOR,    ">="},
        {TokenType::LITERAL,     "18"},
        {TokenType::KEYWORD,     "AND"},
        {TokenType::IDENTIFIER,  "name"},
        {TokenType::OPERATOR,    ">"},
        {TokenType::LITERAL,     "'b'"},
        {TokenType::KEYWORD,     "OR"},
        {TokenType::IDENTIFIER,  "name"},
        {TokenType::OPERATOR,    "="},
        {TokenType::LITERAL,     "'Alon'"},
        {TokenType::KEYWORD,     "ORDER"},
        {TokenType::KEYWORD,     "BY"},
        {TokenType::IDENTIFIER,  "age"},
        {TokenType::KEYWORD,     "DESC"}
    };

    Parser parser(tokens);
    std::unique_ptr<Statement> base = parser.parse();
    ASSERT_NE(dynamic_cast<SelectStatement*>(base.get()), nullptr);
    SelectStatement& select = static_cast<SelectStatement&>(*base);

    EXPECT_EQ(select.tableName, "users");
    ASSERT_EQ(select.columnNames.size(), 3);
    EXPECT_EQ(select.columnNames[0], "id");
    EXPECT_EQ(select.columnNames[1], "name");
    EXPECT_EQ(select.columnNames[2], "age");
    ASSERT_TRUE(select.where.has_value());
    ASSERT_EQ(select.where->conditionGroups.size(), 2);
    ASSERT_EQ(select.where->conditionGroups[0].size(), 2);
    ASSERT_EQ(select.where->conditionGroups[1].size(), 1);
    EXPECT_EQ(select.where->conditionGroups[0][0].columnName, "age");
    EXPECT_EQ(select.where->conditionGroups[0][0].op, OpType::GTE);
    EXPECT_EQ(std::get<INTEGER>(select.where->conditionGroups[0][0].value), 18);
    EXPECT_EQ(select.where->conditionGroups[0][1].columnName, "name");
    EXPECT_EQ(select.where->conditionGroups[0][1].op, OpType::GT);
    EXPECT_EQ(std::get<TEXT>(select.where->conditionGroups[0][1].value), "b");
    EXPECT_EQ(select.where->conditionGroups[1][0].columnName, "name");
    EXPECT_EQ(select.where->conditionGroups[1][0].op, OpType::EQ);
    EXPECT_EQ(std::get<TEXT>(select.where->conditionGroups[1][0].value), "Alon");
    ASSERT_TRUE(select.order.has_value());
    EXPECT_EQ(select.order->columnName, "age");
    EXPECT_FALSE(select.order->isAsc);
}

TEST(ParserTest, Delete)
{
    std::vector<Token> tokens = {
        {TokenType::KEYWORD,    "DELETE"},
        {TokenType::KEYWORD,    "FROM"},
        {TokenType::IDENTIFIER, "users"},
        {TokenType::KEYWORD,    "WHERE"},
        {TokenType::IDENTIFIER, "age"},
        {TokenType::OPERATOR,   "!="},
        {TokenType::LITERAL,    "18"},
    };

    Parser parser(tokens);
    std::unique_ptr<Statement> base = parser.parse();
    ASSERT_NE(dynamic_cast<DeleteStatement*>(base.get()), nullptr);
    DeleteStatement& del = static_cast<DeleteStatement&>(*base);

    EXPECT_EQ(del.tableName, "users");
    ASSERT_EQ(del.where.conditionGroups.size(), 1);
    ASSERT_EQ(del.where.conditionGroups[0].size(), 1);
    EXPECT_EQ(del.where.conditionGroups[0][0].columnName, "age");
    EXPECT_EQ(del.where.conditionGroups[0][0].op, OpType::NEQ);
    EXPECT_EQ(std::get<INTEGER>(del.where.conditionGroups[0][0].value), 18);
}

TEST(ParserTest, Drop)
{
    std::vector<Token> tokens = {
        {TokenType::KEYWORD,    "DROP"},
        {TokenType::KEYWORD,    "TABLE"},
        {TokenType::IDENTIFIER, "users"},
    };

    Parser parser(tokens);
    std::unique_ptr<Statement> base = parser.parse();
    ASSERT_NE(dynamic_cast<DropStatement*>(base.get()), nullptr);
    DropStatement& drop = static_cast<DropStatement&>(*base);

    EXPECT_EQ(drop.tableName, "users");
}