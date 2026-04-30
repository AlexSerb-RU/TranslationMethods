#pragma once

#include "FSM.h"
#include "emitter.h"
#include <string>
#include <vector>

class Parser {
public:
	Parser(const std::vector<Token>& tokens, Emitter &emitter);
	void parseTranslationUnit();

private:
	std::vector<Token> tokens;
	size_t pos;
	Emitter &em;
	Token lookahead;
	Token lookahead2;

	void nextToken();
	bool match(Token::Type type, const std::string &lexeme = ""); 
	Token expect(Token::Type type, const std::string &lexeme = ""); 

	// грамматика
	void parseExternalDeclaration(); // поддерживаются объявления и главная функция
	void parseDeclaration(); // поддерживается только объявление переменных 
	void parseDeclarationSpecifier(); // поддерживается только 'int' и 'const'
	void parseInitDeclaratorList(); // поддерживается только список инициализаторов-деклараторов для переменных
	void parseInitDeclarator(); // поддерживается только инициализатор-декларатор для переменных (id [= присваивание])
	void parseMainFunctionDefinition(); // поддерживается только определение главной функции

	void parseStatement(); // поддерживаются выражение-оператор, составной оператор, switch, break и return
	void parseExpressionStatement(); // поддерживается выражение-оператор
	void parseCompoundStatement(); // поддерживается составной оператор 
	void parseSwitchStatement(); // поддерживается оператор switch
	void parseSwitchItems(std::vector<std::tuple<bool,int,std::string>> &clauses); // поддерживаются элементы switch
	void parseCaseClause(std::vector<std::tuple<bool,int,std::string>> &clauses); // поддерживается оператор case
	void parseDefaultClause(std::vector<std::tuple<bool,int,std::string>> &clauses); // поддерживается оператор default
	void parseBreakStatement(); // поддерживается оператор break
	void parseReturnStatement(); // поддерживается оператор return

	// выражения
	void parseExpression(); // поддерживается выражение-оператор (присваивание и ниже)
	void parseAssignmentExpression(); // поддерживается присваивание
	void parseEqualityExpression(); // поддерживается сравнение
	void parseRelationalExpression(); // поддерживается реляционное выражение
	void parseAdditiveExpression(); // поддерживается сложение и вычитание
	void parseMultiplicativeExpression(); // поддерживается умножение и деление
	void parsePrimaryExpression(); // поддерживается первичное выражение

	// утилиты
	void writeError(const Token &t, const std::string &msg);
};
