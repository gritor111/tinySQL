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
	Tokenizer::printTokens(_tokens);

	// Get statement type
	const std::string keyword = expect(TokenType::KEYWORD).value;
	if (keyword == "CREATE")
	{
		return std::make_unique<CreateStatement>(parseCreateStatement());
	}
	else
	{
		throw std::invalid_argument("Syntax Error Near " + keyword);
	}
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

	std::string currColumnName = expect(TokenType::IDENTIFIER).value;
	std::string currTypeStr = expect(TokenType::IDENTIFIER).value;
	resultStatement.columnsData.emplace_back(currColumnName, currTypeStr);

	bool rightParenthesessFound = false;
	while (_pos < _tokens.size())
	{
		const std::string nextPunct = expect(TokenType::PUNCTUATION).value;
		if (nextPunct == ")")
		{
			rightParenthesessFound = true;
			break;
		}
		else if (nextPunct != ",")
		{
			throw std::runtime_error("Syntax error near " + nextPunct + ".");
		}

		std::string currColumnName = expect(TokenType::IDENTIFIER).value;
		std::string currTypeStr = expect(TokenType::IDENTIFIER).value;
		resultStatement.columnsData.emplace_back(currColumnName, currTypeStr);
	}

	if (!rightParenthesessFound)
	{
		throw std::runtime_error("Syntax error, no right ) found.");
	}

	return resultStatement;
}

/*
Helper function, checks if token at current position matches a type and value.
returns token if matches, raises error if it doesnt
input: type and value of matched token
output: token if matched
*/
Token Parser::expect(TokenType type, std::optional<std::string> value)
{
	const Token& currToken = _tokens[_pos];

	if (type != currToken.type)
	{
		throw std::runtime_error("Unexpected token type, expected " + Tokenizer::getTokenTypeName(type)
			+ " got " + Tokenizer::getTokenTypeName(currToken.type) + " instead.");
	}

	if (value && *value != currToken.value)
	{
		throw std::runtime_error("Unexpected token value, expected " + *value + " got " + currToken.value + " instead.");
	}

	_pos++;
	return currToken;
}


