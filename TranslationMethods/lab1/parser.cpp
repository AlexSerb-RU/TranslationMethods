#include "parser.h"
#include <iostream>
#include <sstream>

Parser::Parser(const std::vector<Token>& tokens_, Emitter& emitter_) 
    : tokens(tokens_), pos(0), em(emitter_), lookahead(Token::END_OF_FILE, "", 0, 0), lookahead2(Token::END_OF_FILE, "", 0, 0) {
    // предварительный просмотр инициализируется двумя токенами
    if (pos < tokens.size()) {
        lookahead = tokens[pos++];
    }
    if (pos < tokens.size()) {
        lookahead2 = tokens[pos++];
    }
}

void Parser::nextToken() {
    // выполняется переход на один токен
    lookahead = lookahead2;
    if (pos < tokens.size()) {
        lookahead2 = tokens[pos++];
    } else {
        lookahead2 = Token(Token::END_OF_FILE, "", 0, 0);
    }
}

bool Parser::match(Token::Type type, const std::string& lexeme) {
    if (lookahead.type != type) return false;
    if (!lexeme.empty() && lookahead.lexeme != lexeme) return false;
    nextToken();
    return true;
}

Token Parser::expect(Token::Type type, const std::string& lexeme) {
    Token t = lookahead;
    if (t.type == type && (lexeme.empty() || t.lexeme == lexeme)) {
        nextToken();
        return t;
    }
    std::string expected = lexeme.empty() ? std::to_string(type) : lexeme;
    writeError(t, std::string("Unexpected token '") + t.lexeme + "' expected '" + expected + "'");
    return t;
}

void Parser::writeError(const Token& t, const std::string& msg) {
    std::string error_msg = std::to_string(t.line) + ":" + std::to_string(t.column) + ": ERROR: " + msg + " (near '" + t.lexeme + "')\n";

    // Вывод в консоль
    std::cerr << error_msg;

    // Вывод в файл ошибок
    std::ostream* err_stream = em.getErrorStream();
    if (err_stream) {
        (*err_stream) << error_msg;
    }

    // Базовое восстановление выполняется: если текущий предварительный просмотр равен токену, в котором была сообщена ошибка,
    // он используется, чтобы избежать повторения одной и той же ошибки.
    if (lookahead.line == t.line && lookahead.column == t.column && lookahead.lexeme == t.lexeme) {
        // один токен просматривается вперед для попытки восстановления
        try {
            nextToken();
        }
        catch (...) {
            // игнорируется
        }
    }
}

void Parser::parseTranslationUnit() {
    while (lookahead.type != Token::END_OF_FILE) {
        parseExternalDeclaration();
    }
}

void Parser::parseExternalDeclaration() {
    // объявления и главная функция поддерживаются на верхнем уровне
    if (lookahead.type == Token::KEYWORD && (lookahead.lexeme == "int" || lookahead.lexeme == "void" || lookahead.lexeme == "const")) {
        // спецификатор объявления разбирается
        // необязательный 'const' обрабатывается
        if (lookahead.lexeme == "const") nextToken();
        // ожидается тип
        if (lookahead.type == Token::KEYWORD && (lookahead.lexeme == "int" || lookahead.lexeme == "void")) {
            Token typeTok = lookahead;
            nextToken();

            // если следующим является идентификатор 'main', за которым следует '(', то определение функции для main разбирается
            if (lookahead.type == Token::IDENTIFIER && lookahead.lexeme == "main") {
                // идентификатор потребляется
                nextToken();
                // ожидается '('
                if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == "(") {
                    // остаток функции main (параметры и тело) разбирается
                    // метка для main генерируется
                    em.emitLabel("main");
                    // параметры и тело разбираются
                    // ожидается '('
                    expect(Token::SEPARATOR, "(");
                    // допускается 'void' или пустота
                    if (lookahead.type == Token::KEYWORD && lookahead.lexeme == "void") nextToken();
                    expect(Token::SEPARATOR, ")");
                    // составной оператор как тело функции разбирается
                    parseCompoundStatement();
                    return;
                }
                else {
                    // это не функция, поэтому обрабатывается как объявление, начинающееся с идентификатора (проваливание)
                    // продолжается разбор списка инициализаторов-деклараторов
                }
            }

            // в противном случае обрабатывается как объявление: разбирается список инициализаторов-деклараторов
            // первый декларатор разбирается
            while (true) {
                if (lookahead.type == Token::IDENTIFIER) {
                    Token id = lookahead; nextToken();
                    if (lookahead.type == Token::OPERATOR && lookahead.lexeme == "=") {
                        nextToken();
                        parseAssignmentExpression();
                        em.emitStore(id.lexeme);
                    }
                }
                else {
                    writeError(lookahead, "expected identifier in declaration");
                    // выполняется восстановление до следующей ',' или ';'
                    while (!(lookahead.type == Token::SEPARATOR && (lookahead.lexeme == "," || lookahead.lexeme == ";")) && lookahead.type != Token::END_OF_FILE) nextToken();
                }

                if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == ",") {
                    nextToken();
                    continue;
                }
                break;
            }

            // ожидается точка с запятой
            if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == ";") nextToken();
            else writeError(lookahead, "expected ';' after declaration");
            return;
        }
    }

    // в противном случае предпринимается попытка разобрать оператор (например, может поддерживаться switch верхнего уровня)
    parseStatement();
}

void Parser::parseDeclaration() {
    // объявление разбирается: [const] (int|void) список_инициализаторов_деклараторов ';'
    // необязательный 'const' обрабатывается
    if (lookahead.type == Token::KEYWORD && lookahead.lexeme == "const") {
        nextToken();
    }

    // спецификатор типа разбирается
    if (lookahead.type == Token::KEYWORD && (lookahead.lexeme == "int" || lookahead.lexeme == "void")) {
        nextToken();
    }
    else {
        writeError(lookahead, "expected type specifier in declaration");
        // выполняется восстановление до точки с запятой
        while (!(lookahead.type == Token::SEPARATOR && lookahead.lexeme == ";") && lookahead.type != Token::END_OF_FILE) nextToken();
        if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == ";") nextToken();
        return;
    }

    // список инициализаторов-деклараторов разбирается: id [= присваивание] {, id [= присваивание] }
    while (true) {
        if (lookahead.type == Token::IDENTIFIER) {
            Token id = lookahead;
            nextToken();

            if (lookahead.type == Token::OPERATOR && lookahead.lexeme == "=") {
                nextToken();
                parseAssignmentExpression();
                // сохранение для инициализированной переменной генерируется
                em.emitStore(id.lexeme);
            }
        }
        else {
            writeError(lookahead, "expected identifier in declaration");
            // выполняется восстановление до следующей ',' или ';'
            while (!(lookahead.type == Token::SEPARATOR && (lookahead.lexeme == "," || lookahead.lexeme == ";")) && lookahead.type != Token::END_OF_FILE) nextToken();
        }

        if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == ",") {
            nextToken();
            continue;
        }
        break;
    }

    if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == ";") nextToken();
    else writeError(lookahead, "expected ';' after declaration");
}

void Parser::parseDeclarationSpecifier() {}
void Parser::parseInitDeclaratorList() {}
void Parser::parseInitDeclarator() {}
void Parser::parseMainFunctionDefinition() {}

void Parser::parseStatement() {
    if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == ";") {
        nextToken();
        return;
    }
    if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == "{") {
        parseCompoundStatement();
        return;
    }
    if (lookahead.type == Token::KEYWORD && lookahead.lexeme == "switch") {
        parseSwitchStatement();
        return;
    }
    if (lookahead.type == Token::KEYWORD && lookahead.lexeme == "break") {
        parseBreakStatement();
        return;
    }
    if (lookahead.type == Token::KEYWORD && lookahead.lexeme == "return") {
        parseReturnStatement();
        return;
    }
    // объявление внутри блока разбирается
    if (lookahead.type == Token::KEYWORD && (lookahead.lexeme == "int" || lookahead.lexeme == "const" || lookahead.lexeme == "void")) {
        parseDeclaration();
        return;
    }
    // по умолчанию: выражение-оператор разбирается
    parseExpressionStatement();
}

void Parser::parseExpressionStatement() {
    if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == ";") {
        nextToken();
        return;
    }
    parseExpression();
    if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == ";") nextToken();
}

void Parser::parseCompoundStatement() {
    // ожидается '{'
    if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == "{") nextToken();
    else {
        writeError(lookahead, "expected '{' to begin compound statement");
        return;
    }

    while (!(lookahead.type == Token::SEPARATOR && lookahead.lexeme == "}") && lookahead.type != Token::END_OF_FILE) {
        if (lookahead.type == Token::KEYWORD && (lookahead.lexeme == "int" || lookahead.lexeme == "const" || lookahead.lexeme == "void")) {
            parseDeclaration();
        }
        else {
            parseStatement();
        }
    }

    if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == "}") nextToken();
}

void Parser::parseSwitchStatement() {
    // ожидается 'switch'
    expect(Token::KEYWORD, "switch");
    expect(Token::SEPARATOR, "(");

    // выражение разбирается во временный эмиттер
    Emitter tmpEm;
    // стек switch копируется
    tmpEm.setInitialSwitchStack(std::vector<std::string>());
    // временное использование tmpEm выполняется путем создания подпарсера? Проще: токены выражения генерируются в tmpEm через вызов методов разбора выражений, которые записывают в tmpEm.
    // Для достижения этого без рефакторинга: указатель на текущий эмиттер сохраняется, создается новый парсер, использующий тот же сканер? Это считается сложным.
    // Более простой подход: выражение разбирается и генерируется непосредственно в главный эмиттер, затем значение сохраняется
    parseExpression();
    expect(Token::SEPARATOR, ")");

    // идентификатор switch и метки создаются
    int sid = em.nextSwitchId();
    std::string tmpVar = "sw" + std::to_string(sid);
    em.emitInstr("store-switch-temp", ""); // маркер
    em.emitStore(tmpVar);

    // конечная метка для этого switch создается и помещается в стек, чтобы 'break' мог на нее ссылаться
    std::string endLabel = "sw" + std::to_string(sid) + "_end";
    em.pushSwitchEnd(endLabel);

    expect(Token::SEPARATOR, "{");

    // сборка ветвей: для простоты, ветви разбираются, а метки и тела сразу генерируются
    // Этот наивный подход не создает таблицу диспетчеризации, но позволяет использовать вложенные switch и break
    // Итерация по case/default выполняется до '}'
    while (!(lookahead.type == Token::SEPARATOR && lookahead.lexeme == "}") && lookahead.type != Token::END_OF_FILE) {
        if (lookahead.type == Token::KEYWORD && lookahead.lexeme == "case") {
            // метка для case генерируется
            nextToken();
            if (lookahead.type == Token::INTEGER_CONSTANT) {
                std::string caseLabel = "sw" + std::to_string(sid) + "_case" + lookahead.lexeme;
                // число потребляется
                std::string num = lookahead.lexeme;
                nextToken();
                // двоеточие обязательно после выражения case
                expect(Token::SEPARATOR, ":");
                em.emitLabel(caseLabel);
                // операторы разбираются до следующего case/default/}
                while (!(lookahead.type == Token::KEYWORD && (lookahead.lexeme == "case" || lookahead.lexeme == "default")) && !(lookahead.type == Token::SEPARATOR && lookahead.lexeme == "}") && lookahead.type != Token::END_OF_FILE) {
                    parseStatement();
                }
            }
            else {
                writeError(lookahead, "expected constant after 'case'");
                // выполняется попытка восстановления
                while (!(lookahead.type == Token::SEPARATOR && lookahead.lexeme == ";") && lookahead.type != Token::END_OF_FILE) nextToken();
                if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == ";") nextToken();
            }
        }
        else if (lookahead.type == Token::KEYWORD && lookahead.lexeme == "default") {
            nextToken();
            // двоеточие обязательно после default
            expect(Token::SEPARATOR, ":");
            em.emitLabel("sw" + std::to_string(sid) + "_default");
            while (!(lookahead.type == Token::KEYWORD && (lookahead.lexeme == "case" || lookahead.lexeme == "default")) && !(lookahead.type == Token::SEPARATOR && lookahead.lexeme == "}") && lookahead.type != Token::END_OF_FILE) {
                parseStatement();
            }
        }
        else {
            // неожиданный токен внутри switch: пропускается
            writeError(lookahead, "unexpected token in switch");
            nextToken();
        }
    }

    if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == "}") nextToken();
    if (!em.currentSwitchEnd().empty()) {
        // конечная метка из вершины стека генерируется (должна быть endLabel)
        em.emitLabel(em.currentSwitchEnd());
    }
    else {
        em.emitLabel("sw" + std::to_string(sid) + "_end");
    }

    em.popSwitchEnd();
}

void Parser::parseSwitchItems(std::vector<std::tuple<bool, int, std::string>>& clauses) {
    // не используется в этой упрощенной реализации
}

void Parser::parseCaseClause(std::vector<std::tuple<bool, int, std::string>>& clauses) {}
void Parser::parseDefaultClause(std::vector<std::tuple<bool, int, std::string>>& clauses) {}

void Parser::parseBreakStatement() {
    expect(Token::KEYWORD, "break");
    if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == ";") nextToken();
    std::string end = em.currentSwitchEnd();
    if (end.empty()) {
        writeError(lookahead, "'break' outside of switch");
    }
    else {
        em.emitInstr("JMP", end);
    }
}

void Parser::parseReturnStatement() {
    expect(Token::KEYWORD, "return");
    if (!(lookahead.type == Token::SEPARATOR && lookahead.lexeme == ";")) {
        parseExpression();
    }
    if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == ";") nextToken();
    em.emitInstr("RETURN", "");
}

void Parser::parseExpression() {
    parseAssignmentExpression();
}

void Parser::parseAssignmentExpression() {
    // если IDENTIFIER '=' присваивание_выражение (используется lookahead2 для заглядывания)
    if (lookahead.type == Token::IDENTIFIER && lookahead2.type == Token::OPERATOR && lookahead2.lexeme == "=") {
        Token id = lookahead;
        // идентификатор и '=' потребляются
        nextToken(); // now lookahead is '='
        nextToken(); // выполняется переход к токену после '='
        parseAssignmentExpression();
        em.emitStore(id.lexeme);
        return;
    }
    // в противном случае выражения с низшим приоритетом разбираются обычным образом
    parseEqualityExpression();
}

void Parser::parseEqualityExpression() {
    parseRelationalExpression();
    while (lookahead.type == Token::OPERATOR && (lookahead.lexeme == "==" || lookahead.lexeme == "!=")) {
        std::string op = lookahead.lexeme;
        nextToken();
        parseRelationalExpression();
        if (op == "==") em.emitOperator("=="); else em.emitOperator("!=");
    }
}

void Parser::parseRelationalExpression() {
    parseAdditiveExpression();
    while (lookahead.type == Token::OPERATOR && lookahead.lexeme == "<") {
        nextToken();
        parseAdditiveExpression();
        em.emitOperator("<");
    }
}

void Parser::parseAdditiveExpression() {
    parseMultiplicativeExpression();
    while (lookahead.type == Token::OPERATOR && (lookahead.lexeme == "+" || lookahead.lexeme == "-")) {
        std::string op = lookahead.lexeme;
        nextToken();
        parseMultiplicativeExpression();
        em.emitOperator(op);
    }
}

void Parser::parseMultiplicativeExpression() {
    parsePrimaryExpression();
    while (lookahead.type == Token::OPERATOR && lookahead.lexeme == "*") {
        nextToken();
        parsePrimaryExpression();
        em.emitOperator("*");
    }
}

void Parser::parsePrimaryExpression() {
    if (lookahead.type == Token::IDENTIFIER) {
        em.emitOperand(lookahead.lexeme);
        nextToken();
        return;
    }
    if (lookahead.type == Token::INTEGER_CONSTANT) {
        em.emitOperand(lookahead.lexeme);
        nextToken();
        return;
    }
    if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == "(") {
        nextToken();
        parseExpression();
        if (lookahead.type == Token::SEPARATOR && lookahead.lexeme == ")") nextToken();
        else writeError(lookahead, "expected ')'");
        return;
    }
    writeError(lookahead, "unexpected token in primary expression");
}