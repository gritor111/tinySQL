#pragma once

#include <iostream>
#include <vector>
#include <exception>
#include <optional>
#include <string>
#include <functional>

#include "tokenizer.h"
#include "table.h"

enum StatementType 
{
	CREATE,
	INSERT,
	SELECT,
	DELETE,
	DROP
};

enum OpType 
{
	EQ,
	GT,
	LT,
	GTE,
	LTE,
	NEQ
};

struct Statement
{
	const StatementType type;

	Statement(StatementType type) : type(type)
	{
	}

	virtual ~Statement() = default;
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

struct Condition
{
	std::string columnName;
	OpType op;
	Data value;
};

struct OrderClause
{
	std::string columnName;
	bool isAsc;
};

struct SelectStatement : Statement
{
	std::vector<std::string> columnNames;
	std::string tableName;
	std::optional<Condition> condition;
	std::optional<OrderClause> orderClause;

	SelectStatement() : Statement(StatementType::SELECT)
	{
	}
};

struct DeleteStatement : Statement
{
	std::string tableName;
	Condition condition;

	DeleteStatement() : Statement(StatementType::DELETE)
	{
	}
};

struct DropStatement : Statement
{
	std::string tableName;

	DropStatement() : Statement(StatementType::DROP)
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
	SelectStatement parseSelectStatement();
	DropStatement parseDropStatement();
	DeleteStatement parseDeleteStatement();

	// helpers
	Token peek();
	Token consume();
	std::optional<Token> match(TokenType type, std::string value = "");
	Token expect(TokenType type, std::string value = "");
};

static Data resolveData(const Token& literalToken);
static OpType getOperator(const std::string& op);