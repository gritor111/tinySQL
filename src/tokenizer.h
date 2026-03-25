#pragma once

#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>

enum TokenType {
    KEYWORD,
    IDENTIFIER,
    OPERATOR,

    PUNCTUATION,

    LITERAL,

    DATA_TYPE,

    END_OF_QUERY,
    UNKOWN
};

struct Token
{
    TokenType type;
    std::string value;

    Token(TokenType type, const std::string& value) : type(type), value(value)
    {
    }
};

class Tokenizer
{
public:
    Tokenizer(const std::string& query);
    std::vector<Token> tokenize();

    static std::string getTokenTypeName(TokenType type);
    static void printTokens(const std::vector<Token>& tokens);

private:
    const std::string _query;
    size_t _pos;

    const std::unordered_set<std::string> KEYWORDS = {
        "CREATE", "TABLE", "SELECT", "INSERT", "INTO",
        "DELETE", "FROM", "WHERE", "DROP", "VALES"
    };
    
    std::string consumeWord();
    std::string consumeIntLiteral();
    std::string consumeStringLiteral(const char quoteType);
};