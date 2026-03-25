#pragma once

#include <iostream>
#include <vector>
#include <exception>
#include <optional>
#include <string>

#include "tokenizer.h"
#include "table.h"

enum StatementType {
	CREATE,
	INSERT
};

struct Statement
{
	const StatementType type;

	Statement(StatementType type) : type(type)
	{
	}
};

struct CreateStatement : Statement
{
	std::string tableName;
	std::vector<std::pair<std::string, std::string>> columnsData;

	CreateStatement() : Statement(StatementType::CREATE)
	{
	}
};

struct InsertStatement : Statement
{
	std::string tableName;
	std::vector<Data> values;

	InsertStatement() : Statement(StatementType::INSERT)
	{
	}
};

class Parser
{
public:
	Parser(const std::vector<Token>& tokens);
	std::unique_ptr<Statement> parse();

private:
	size_t _pos;
	std::vector<Token> _tokens;

	CreateStatement parseCreateStatement();
	InsertStatement parseInsertStatement();

	// helpers
	Token peek();
	Token consume();
	std::optional<Token> match(TokenType type, std::string value = "");
	Token expect(TokenType type, std::string value = "");
};

static Data resolveData(const Token& literalToken);