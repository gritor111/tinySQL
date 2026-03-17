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

	while (_pos < _query.length())
	{
		char currChar = _query[_pos];

		// Skip whitespaces
		if (std::isblank(currChar))
		{
			_pos++;
			continue;
		}

		// Check for keywords and identifiers
		if (std::isalpha(currChar))
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
		else
		{
			tokens.emplace_back(TokenType::SYMBOL, std::string(1, currChar));
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
	size_t start = _pos;

	while (_pos < _query.length() && std::isalpha(_query[_pos]))
	{
		_pos++;
	}

	return _query.substr(start, _pos - start);
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
    case TokenType::NUMBER_LITERAL:
        return "NUMBER";
    case TokenType::OPERATOR:
        return "OPERATOR";
    case TokenType::PUNCTUATION:
        return "PUNCTUATION";
	case TokenType::SYMBOL:
		return "SYMBOL";
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