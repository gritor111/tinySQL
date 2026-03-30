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
        {TokenType::IDENTIFIER,  "VALUES"},
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
        {TokenType::KEYWORD,     "ORDER"},
        {TokenType::KEYWORD,     "BY"},
        {TokenType::IDENTIFIER,  "age"},
        {TokenType::KEYWORD,     "DESC"},
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
    ASSERT_TRUE(select.condition.has_value());
    EXPECT_EQ(select.condition->columnName, "age");
    EXPECT_EQ(select.condition->op, OpType::GTE);
    EXPECT_EQ(std::get<INTEGER>(select.condition->value), 18);
    ASSERT_TRUE(select.orderClause.has_value());
    EXPECT_EQ(select.orderClause->columnName, "age");
    EXPECT_FALSE(select.orderClause->isAsc);
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
    EXPECT_EQ(del.condition.columnName, "age");
    EXPECT_EQ(del.condition.op, OpType::NEQ);
    EXPECT_EQ(std::get<INTEGER>(del.condition.value), 18);
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