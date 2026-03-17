#pragma once

#include <iostream>
#include <vector>
#include <exception>
#include <optional>

#include "tokenizer.h"
#include "table.h"

enum StatementType {
	CREATE,
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

class Parser
{
public:
	Parser(const std::vector<Token>& tokens);
	std::unique_ptr<Statement> parse();

private:
	size_t _pos;
	std::vector<Token> _tokens;

	CreateStatement parseCreateStatement();

	// helpers
	Token expect(TokenType type, std::optional<std::string> value = std::nullopt);
};