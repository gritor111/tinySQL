#include <gtest/gtest.h>

#include "tokenizer.h"

TEST(TokenizerTest, Select)
{
    Tokenizer tokenizer("SELECT id, name FROM users WHERE age >= 30 AND balance >= 100000 ORDER BY id DESC");
    std::vector<Token> tokens = tokenizer.tokenize();

    std::vector<Token> expected = {
        {TokenType::KEYWORD,     "SELECT"},
        {TokenType::IDENTIFIER,  "id"},
        {TokenType::PUNCTUATION, ","},
        {TokenType::IDENTIFIER,  "name"},
        {TokenType::KEYWORD,     "FROM"},
        {TokenType::IDENTIFIER,  "users"},
        {TokenType::KEYWORD,     "WHERE"},
        {TokenType::IDENTIFIER,  "age"},
        {TokenType::OPERATOR,    ">="},
        {TokenType::LITERAL,     "30"},
        {TokenType::KEYWORD,     "AND"},
        {TokenType::IDENTIFIER,  "balance"},
        {TokenType::OPERATOR,    ">="},
        {TokenType::LITERAL,    "100000"},
        {TokenType::KEYWORD,     "ORDER"},
        {TokenType::KEYWORD,     "BY"},
        {TokenType::IDENTIFIER,  "id"},
        {TokenType::KEYWORD,     "DESC"},
    };

    ASSERT_EQ(tokens.size(), expected.size());
    for (size_t i = 0; i < expected.size(); i++)
    {
        EXPECT_EQ(Tokenizer::getTokenTypeName(tokens[i].type), Tokenizer::getTokenTypeName(expected[i].type)) << "Type mismatch at index " << i;
        EXPECT_EQ(tokens[i].value, expected[i].value) << "Value mismatch at index " << i;
    }
}