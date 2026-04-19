#pragma once

#include "FSM.h"
#include "emitter.h"
#include <string>
#include <vector>

class Parser {
public:
    Parser(Scanner &scanner, Emitter &emitter);
    void parseTranslationUnit();

private:
    Scanner &scanner;
    Emitter &em;
    Token lookahead;
    Token lookahead2;

    void nextToken();
    bool match(Token::Type type, const std::string &lexeme = "");
    Token expect(Token::Type type, const std::string &lexeme = "");

    // грамматика
    void parseExternalDeclaration();
    void parseDeclaration();
    void parseDeclarationSpecifier();
    void parseInitDeclaratorList();
    void parseInitDeclarator();
    void parseMainFunctionDefinition();

    void parseStatement();
    void parseExpressionStatement();
    void parseCompoundStatement();
    void parseSwitchStatement();
    void parseSwitchItems(std::vector<std::tuple<bool,int,std::string>> &clauses); // (isDefault, value, body)
    void parseCaseClause(std::vector<std::tuple<bool,int,std::string>> &clauses);
    void parseDefaultClause(std::vector<std::tuple<bool,int,std::string>> &clauses);
    void parseBreakStatement();
    void parseReturnStatement();

    // выражения
    void parseExpression();
    void parseAssignmentExpression();
    void parseEqualityExpression();
    void parseRelationalExpression();
    void parseAdditiveExpression();
    void parseMultiplicativeExpression();
    void parsePrimaryExpression();

    // утилиты
    void writeError(const Token &t, const std::string &msg);
};
