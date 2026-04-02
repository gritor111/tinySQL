#include "parser.h"

Parser::Parser(const std::vector<Token>& tokens) : _tokens(tokens), _pos(0)
{
}

/*
Recieves tokens and builds a command object to pass to the engine
input: vector of tokens
output: command for engine to act on
*/
std::unique_ptr<Statement> Parser::parse()
{
	//Tokenizer::printTokens(_tokens);

	// Get statement type

	if (match(TokenType::KEYWORD, "CREATE"))
	{
		return std::make_unique<CreateStatement>(parseCreateStatement());
	}

	if (match(TokenType::KEYWORD, "INSERT"))
	{
		return std::make_unique<InsertStatement>(parseInsertStatement());

	}

	if (match(TokenType::KEYWORD, "SELECT"))
	{
		return std::make_unique<SelectStatement>(parseSelectStatement());
	}

	if (match(TokenType::KEYWORD, "DELETE"))
	{
		return std::make_unique<DeleteStatement>(parseDeleteStatement());
	}

	if (match(TokenType::KEYWORD, "DROP"))
	{
		return std::make_unique<DropStatement>(parseDropStatement());
	}


	throw std::runtime_error("Syntax Error: Unexpected token " + peek().value + ".");

}


/*
Function that parses tokens into a create statement that contains the table name and string for column name and type
input: none
output: create statement
*/
CreateStatement Parser::parseCreateStatement()
{
	CreateStatement resultStatement;

	expect(TokenType::KEYWORD, "TABLE");
	
	resultStatement.tableName = expect(TokenType::IDENTIFIER).value;

	expect(TokenType::PUNCTUATION, "(");

	while (true)
	{
		std::string currColumnName = expect(TokenType::IDENTIFIER).value;
		std::string currTypeStr = expect(TokenType::IDENTIFIER).value;
		resultStatement.columnsData.emplace_back(currColumnName, currTypeStr);

		if (!match(TokenType::PUNCTUATION, ","))
		{
			break;
		}

		if (peek().value == ")")
		{
			throw std::runtime_error("Error syntax: Trailing comma found before ')', eg: (name TEXT, )");
		}
	}

	expect(TokenType::PUNCTUATION, ")");

	return resultStatement;
}


/*
Function to parse an insert query
input: none
output: insert statement
*/
InsertStatement Parser::parseInsertStatement()
{
	InsertStatement resultStatement;

	expect(TokenType::KEYWORD, "INTO");

	resultStatement.tableName = expect(TokenType::IDENTIFIER).value;

	expect(TokenType::KEYWORD, "VALUES");
	expect(TokenType::PUNCTUATION, "(");


	while (true)
	{
		resultStatement.values.emplace_back(resolveData(expect(TokenType::LITERAL)));

		if (!match(TokenType::PUNCTUATION, ","))
		{
			break;
		}

		if (peek().value == ")")
		{
			throw std::runtime_error("Error syntax: Trailing comma found before ')', eg: (name TEXT, )");
		}
	}

	expect(TokenType::PUNCTUATION, ")");

	return resultStatement;
}

/*
Function to parse an select query, with an optional where clause
input: none
output: select statement
*/
SelectStatement Parser::parseSelectStatement()
{
	SelectStatement resultStatement;

	while (true)
	{
		if (peek().value == "*")
		{
			resultStatement.columnNames.emplace_back(expect(TokenType::PUNCTUATION, "*").value);
		}
		else
		{
			resultStatement.columnNames.emplace_back(expect(TokenType::IDENTIFIER).value);
		}

		if (!match(TokenType::PUNCTUATION, ","))
		{
			break;
		}

		if (peek().value == ")")
		{
			throw std::runtime_error("Error syntax: Trailing comma found before ')', eg: (name TEXT, )");
		}
	}
	

	expect(TokenType::KEYWORD, "FROM");

	resultStatement.tableName = expect(TokenType::IDENTIFIER).value;

	if (match(TokenType::KEYWORD, "WHERE"))
	{
		resultStatement.where = parseWhereClause();
	}

	if (match(TokenType::KEYWORD, "ORDER"))
	{
		expect(TokenType::KEYWORD, "BY");
		
		OrderClause orderClause;
		orderClause.columnName = expect(TokenType::IDENTIFIER).value;
		const std::string sortDirection = expect(TokenType::KEYWORD).value;
		orderClause.isAsc = sortDirection == "ASC";

		resultStatement.order = orderClause;
	}

	return resultStatement;
}

/*
Function to parse an drop query
input: none
output: drop statement
*/
DropStatement Parser::parseDropStatement()
{
	DropStatement resultStatement;

	expect(TokenType::KEYWORD, "TABLE");

	resultStatement.tableName = expect(TokenType::IDENTIFIER).value;

	return resultStatement;
}

/*
Function to parse delete query
input: none
output: delete statement
*/
DeleteStatement Parser::parseDeleteStatement()
{
	DeleteStatement resultStatement;

	expect(TokenType::KEYWORD, "FROM");

	resultStatement.tableName = expect(TokenType::IDENTIFIER).value;

	expect(TokenType::KEYWORD, "WHERE");

	resultStatement.where = parseWhereClause();

	return resultStatement;
}

/*
Helper to parse where clause if exists.
input: none
output: where clause
*/
WhereClause Parser::parseWhereClause()
{
	WhereClause result{};
	std::vector<Condition> currGroup{};

	while (true)
	{
		Condition con;
		con.columnName = expect(TokenType::IDENTIFIER).value;
		con.op = getOperator(expect(TokenType::OPERATOR).value);
		con.value = resolveData(expect(TokenType::LITERAL));

		currGroup.emplace_back(con);

		if (peek().type == TokenType::END_OF_QUERY || peek().value == "ORDER")
		{
			if (currGroup.size() > 0)
			{
				result.conditionGroups.emplace_back(currGroup);
			}
			break;
		}

		const std::string& logicGate = expect(TokenType::KEYWORD).value;
		if (logicGate == "OR")
		{
			result.conditionGroups.emplace_back(currGroup);
			currGroup.clear();
		}
		else if (logicGate != "AND")
		{
			throw std::runtime_error("Syntax Error: Expected AND/OR, got '" + logicGate + "'.");
		}
	}

	return result;
}


/*
Returns current token without consuming it
input: none
output: tail of tokens (current token)
*/
Token Parser::peek()
{
	if (_pos >= _tokens.size())
	{
		return Token(TokenType::END_OF_QUERY, "");
	}

	return _tokens[_pos];
}

/*
Advances position and returns token at old position
input: none
output: token at next position
*/
Token Parser::consume()
{
	Token token = peek();

	if (token.type != TokenType::END_OF_QUERY)
	{
		_pos++;
	}

	return token;
}

/*
Returns optional of token at next position
input: type of token, value of token (not required)
output: optional of token
*/
std::optional<Token> Parser::match(TokenType type, std::string value)
{
	Token token = peek();

	if (token.type == type && (value.empty() || token.value == value))
	{
		return consume();
	}

	return std::nullopt;
}

/*
Helper function, consumes token and matches a type and value.
returns token if matches, raises error if it doesnt
input: type and value of matched token
output: token if matched
*/
Token Parser::expect(TokenType type, std::string value)
{
	auto token = match(type, value);

	if (token)
	{
		return *token;
	}

	throw std::runtime_error("Syntax Error: expected " + value + " got " + peek().value + " .");
}

/*
Returns Data (int or std::string) from str representation.
input: str literal
*/
static Data resolveData(const Token& literalToken)
{
	if (literalToken.value.empty())
	{
		return "";
	}

	char firstChar = literalToken.value[0];

	if (firstChar == '-' || std::isdigit(firstChar))
	{
		return std::stoi(literalToken.value);
	}

	// after we validate type we can remove quotes
	return literalToken.value.substr(1, literalToken.value.size() - 2);
}

/*
Converts operator string to operator enum.
input: string of operator
output: Operator type enum
*/
static OpType getOperator(const std::string& op)
{
	if (op == "=") return OpType::EQ;
	if (op == ">") return OpType::GT;
	if (op == "<") return OpType::LT;
	if (op == ">=") return OpType::GTE;
	if (op == "<=") return OpType::LTE;
	if (op == "!=") return OpType::NEQ;

	throw std::runtime_error("Unkown operator" + op + ".");
}