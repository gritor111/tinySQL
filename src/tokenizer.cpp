#include "tokenizer.h"

Tokenizer::Tokenizer(const std::string& query) : _query(query), _pos(0)
{
}

/*
Recieves a sql query and splits it into a vector of tokens.
input: sql query
output: vector of tokens
*/
std::vector<Token> Tokenizer::tokenize()
{
	std::vector<Token> tokens = {};

	while (_pos < _query.size())
	{
		char currChar = _query[_pos];

		// Skip whitespaces
		if (std::isblank(currChar))
		{
			_pos++;
			continue;
		}

		// check for literals (int and string)
		if (currChar == '-')
		{
			if (_pos + 1 < _query.size() && std::isdigit(_query[_pos + 1]))
			{
				tokens.emplace_back(TokenType::LITERAL, consumeIntLiteral());
			}
		}
		else if (std::isdigit(currChar))
		{
			tokens.emplace_back(TokenType::LITERAL, consumeIntLiteral());
		}
		else if (currChar == '\'' || currChar == '"')
		{
			tokens.emplace_back(TokenType::LITERAL, consumeStringLiteral(currChar));
		}
		else if (std::isalpha(currChar)) // Check for keywords and identifierss
		{
			std::string word = consumeWord();
			std::string upperWord = word;
			std::transform(word.begin(), word.end(), upperWord.begin(), std::toupper);

			if (KEYWORDS.find((upperWord)) != KEYWORDS.end())
			{
				tokens.emplace_back(TokenType::KEYWORD, upperWord);
			}
			else
			{
				tokens.emplace_back(TokenType::IDENTIFIER, word);
			}
		}
		else if (currChar == ',' || currChar == '(' || currChar == ')' || currChar == '*')
		{
			tokens.emplace_back(TokenType::PUNCTUATION, std::string(1, currChar));
			_pos++;
		}
		else if (currChar == '<' || currChar == '>' || currChar == '!')
		{
			if (_pos + 1 < _query.size() && _query[_pos + 1] == '=')
			{
				tokens.emplace_back(TokenType::OPERATOR, _query.substr(_pos, 2));
				_pos += 2;
				continue;
			}
			else if (currChar != '!')
			{
				tokens.emplace_back(TokenType::OPERATOR, std::string(1, currChar));
			}
			else
			{
				tokens.emplace_back(TokenType::UNKOWN, std::string(1, currChar));
			}

			_pos++;
		}
		else if (currChar == '=')
		{
			tokens.emplace_back(TokenType::OPERATOR, std::string(1, currChar));
			_pos++;
		}
		else
		{
			tokens.emplace_back(TokenType::UNKOWN, std::string(1, currChar));
			_pos++;
		}
	}

	return tokens;
}

/*
Moves position to end of next word in query and returns the word.
input: none
output: string of word
*/
std::string Tokenizer::consumeWord()
{
	const size_t start = _pos;

	while (_pos < _query.size() && std::isalpha(_query[_pos]))
	{
		_pos++;
	}

	return _query.substr(start, _pos - start);
}

/*
Consumes a signed int literal.
input: none
output: string of integer
*/
std::string Tokenizer::consumeIntLiteral()
{
	const size_t start = _pos;

	if (_query[start] == '-')
	{
		_pos++;
	}

	while (_pos < _query.size() && std::isdigit(_query[_pos]))
	{
		_pos++;
	}

	return _query.substr(start, _pos - start);
}


/*
Consumes a string literal ('example123' or "example123")
input: quote of the literal, either single or double
output: string of literal including quotes
*/
std::string Tokenizer::consumeStringLiteral(const char quoteType)
{
	const size_t start = _pos;
	_pos++; // skip opening quote

	while (_pos < _query.size() && _query[_pos] != quoteType)
	{
		_pos++;
	}

	if (_pos >= _query.size())
	{
		throw std::runtime_error("Unterminated string literal.");
	}

	_pos++; // skip closing quote
	std::string literal = _query.substr(start, _pos - start);

	return literal;
}


/*
Helper function to get TokenType string from type.
input: Token type
output: string representation of token type
*/
std::string Tokenizer::getTokenTypeName(TokenType type)
{
    switch (type) 
	{
    case TokenType::KEYWORD:
        return "KEYWORD";
    case TokenType::IDENTIFIER:
        return "IDENTIFIER";
    case TokenType::LITERAL:
        return "LITERAL";
    case TokenType::OPERATOR:
        return "OPERATOR";
    case TokenType::PUNCTUATION:
        return "PUNCTUATION";
    default:
        return "UNDEFINED";
    }
}

/*
Helper function to print all tokens
*/
void Tokenizer::printTokens(const std::vector<Token>& tokens)
{
    for (const auto& token : tokens) 
	{
        std::cout << getTokenTypeName(token.type) << ": " << token.value << std::endl;
    }
}