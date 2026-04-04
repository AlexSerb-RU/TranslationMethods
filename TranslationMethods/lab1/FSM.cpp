#include "FSM.h"
#include <algorithm>
#include <cctype>
#include <string>

// ============= Scanner Implementation =============

using namespace std;

Scanner::Scanner(const string& source)
   : input(source), position(0), current_line(1), current_column(1) {
   init_keywords();
   init_dfas();
}

Token Scanner::next_token() {
   skip_whitespace_and_comments();

   if (position >= input.length()) {
      return Token(Token::END_OF_FILE, "", current_line, current_column);
   }

   Token token = try_recognize_with_dfas();

   position += token.lexeme.length();
   update_line_column(token.lexeme);

   return token;
}

void Scanner::init_keywords() {
   keywords["if"] = Token::KEYWORD;
   keywords["else"] = Token::KEYWORD;
   keywords["while"] = Token::KEYWORD;
   keywords["int"] = Token::KEYWORD;
   keywords["float"] = Token::KEYWORD;
}

void Scanner::init_dfas() {
   identifier_dfa.start_state = 0;
   identifier_dfa.transitions[0].push_back({
      []( char c ) { return isalpha( c ) || c == '_'; },
      1
   });
   identifier_dfa.transitions[1].push_back({
      []( char c ) { return isalnum( c ) || c == '_'; },
      1
   });
   identifier_dfa.final_states.insert(1);
   identifier_dfa.state_to_token_type[1] = Token::IDENTIFIER;

   number_dfa.start_state = 0;
   number_dfa.transitions[0].push_back({
      []( char c ) { return isdigit( c ); },
      1
   });
   number_dfa.transitions[1].push_back({
      []( char c ) { return isdigit( c ); },
      1
   });
   number_dfa.transitions[1].push_back({
      []( char c ) { return c == '.'; },
      2
   });
   number_dfa.transitions[2].push_back({
      []( char c ) { return isdigit( c ); },
      3
   });
   number_dfa.transitions[3].push_back({
      []( char c ) { return isdigit( c ); },
      3
   });

   number_dfa.final_states.insert(1);
   number_dfa.final_states.insert(3);
   number_dfa.state_to_token_type[1] = Token::INTEGER;

   operator_dfa.start_state = 0;
   string operators = "+-*/=<>!";
   for (char op : operators) {
      operator_dfa.transitions[0].push_back({
         [op]( char c ) { return c == op; },
         1
      });
   }
   operator_dfa.final_states.insert(1);
   operator_dfa.state_to_token_type[1] = Token::OPERATOR;

   separator_dfa.start_state = 0;
   string separators = "(){}[];,";
   for (char sep : separators) {
      separator_dfa.transitions[0].push_back({
         [sep]( char c ) { return c == sep; },
         1
      });
   }
   separator_dfa.final_states.insert(1);
   separator_dfa.state_to_token_type[1] = Token::SEPARATOR;

   // Пробельные символы
   /*whitespace_dfa.start_state = 0;
   whitespace_dfa.transitions[0].push_back({
      []( char c ) { return isspace( c ); },
      1
   });
   whitespace_dfa.transitions[1].push_back({
      []( char c ) { return isspace( c ); },
      1
   });
   whitespace_dfa.final_states.insert(1);
   whitespace_dfa.state_to_token_type[1] = Token::WHITESPACE;*/

   // Комментарии
   /*comment_dfa.start_state = 0;
   comment_dfa.transitions[0].push_back({
      []( char c ) { return c == '/'; },
      1
   });
   comment_dfa.transitions[1].push_back({
      []( char c ) { return c == '/'; },
      2
   });
   comment_dfa.transitions[2].push_back({
      []( char c ) { return c != '\n'; },
      2
   });
   comment_dfa.final_states.insert(2);
   comment_dfa.state_to_token_type[2] = Token::COMMENT;*/
}

void Scanner::skip_whitespace_and_comments() {
   while (position < input.length()) {
      string whitespace = run_dfa(whitespace_dfa); // нужно убрать. Пробелы и комментарии пропускаем.
      if (!whitespace.empty()) {
         position += whitespace.length();
         update_line_column(whitespace);
         continue;
      }

      string comment = run_dfa(comment_dfa);
      if (!comment.empty()) {
         position += comment.length();
         update_line_column(comment);
         continue;
      }

      break;
   }
}

string Scanner::run_dfa(const LexerDFA& dfa) {
   LexerDFA::State current_state = dfa.start_state;
   size_t start_pos = position;
   size_t last_final_pos = start_pos;
   LexerDFA::State last_final_state = -1;

   for (size_t i = start_pos; i < input.length(); ++i) {
      char c = input[i];

      bool found = false;
      for (const auto& trans : dfa.transitions.at(current_state)) {
         if (trans.condition(c)) {
            current_state = trans.next_state;
            found = true;
            break;
         }
      }

      if (!found) break;

      if (dfa.final_states.count(current_state)) {
         last_final_pos = i + 1;
         last_final_state = current_state;
      }
   }

   if (last_final_state != -1) {
      return input.substr(start_pos, last_final_pos - start_pos);
   }
   return "";
}

Token Scanner::try_recognize_with_dfas() {
   string ident = run_dfa(identifier_dfa);
   if (!ident.empty()) {
      auto it = keywords.find(ident);
      if (it != keywords.end()) {
         return Token(it->second, ident, current_line, current_column);
      }
      else {
         return Token(Token::IDENTIFIER, ident, current_line, current_column);
      }
   }

   std::string num = run_dfa(number_dfa);
   if (!num.empty()) {
      Token t(Token::INTEGER, num, current_line, current_column);
      t.int_value = std::stoi(num);

      return t;
   }

   string op = run_dfa(operator_dfa);
   if (!op.empty()) {
      return Token(Token::OPERATOR, op, current_line, current_column);
   }

   string sep = run_dfa(separator_dfa);
   if (!sep.empty()) {
      return Token(Token::SEPARATOR, sep, current_line, current_column);
   }

   string unknown(1, input[position]);
   return Token(Token::UNKNOWN, unknown, current_line, current_column);
}

void Scanner::update_line_column(const string& lexeme) {
   for (char c : lexeme) {
      if (c == '\n') {
         current_line++;
         current_column = 1;
      }
      else {
         current_column++;
      }
   }
}

// ============= FSM Implementation =============

int FSM::get_symbol_index(char symbol) {
   auto it = find(symbols.begin(), symbols.end(), symbol);

   if (it == symbols.end())
      return -1;

   return static_cast<int>(distance(symbols.begin(), it));
}

int FSM::process_symbol(char symbol) {
   int symbol_index = get_symbol_index(symbol);

   if (symbol_index == -1) {
      if (find(final_states.begin(), final_states.end(), current_state) == final_states.end())
         return -1;

      return current_state;
   }

   current_state = table[current_state][symbol_index];
   return current_state;
}
