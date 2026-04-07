#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <algorithm>

#include "StaticTable.h"
#include "DynamicTable.h"

//using std::vector;
using namespace std;

// Токен
struct Token {
   enum Type {
      ALPHABET = 0,
      IDENTIFIER,    // идентификатор
      KEYWORD,           // ключевое слово
      INTEGER_CONSTANT,           // целая константа
      CUSTOM_CONSTANT,          // именованая константа (только int)
      OPERATOR,          // оператор (+, -, *, /, =, ...)
      SEPARATOR,         // разделитель (;, ,, (, ), {, }, ...) + комментарий
      END_OF_FILE      // конец файла
   };

   Type type;
   string lexeme;     // распознанная лексема
   int line;               // строка
   int column;             // колонка начала лексемы

   int idx_in_table = -1; // индекс в таблице (для идентификаторов и констант)

   // Для чисел можно хранить значение
   std::optional<int> int_value;

   Token( Type t, const string &lex, int l, int c )
      : type( t ), lexeme( lex ), line( l ), column( c ) {
   }
};

// ДКА для одного типа лексем
struct LexerDFA {
   using State = int;
   
   static constexpr State DEAD = -1;
   State start_state = 0;

   vector<std::vector<State>> matrix;   // Матрица переходов
   vector<char> symbols;
   std::unordered_map<char, int> symbol_to_index_map;  // Соответствие символов к номерам
   std::unordered_set<State> final_states;    // конечные состояния
   // Для каждого конечного состояния - какой токен он производит
   std::unordered_map<State, Token::Type> state_to_token_type;

   bool is_final( State s )
   {
      return final_states.find( s ) != final_states.end( );
   }

   void init_matrix( int states_count ) 
   {
      matrix.assign( states_count, std::vector<State>( symbols.size( ), DEAD ) );
   }

   int get_symbol_index( char symbol ) 
   {
      auto it = symbol_to_index_map.find( symbol );
      if ( it == symbol_to_index_map.end( ) )
         return -1;

      return it->second;
   }

   State move( State state, char symbol )
   {
      int idx = get_symbol_index( symbol );
      if ( idx == -1 )
         return -1;

      State new_state = matrix[state][idx];

      return new_state;
   }
};

// Сканер, использующий набор ДКА
class Scanner {
private:
   struct MatchResult {
      bool matched = false;
      Token::Type type = Token::END_OF_FILE;
      string lexeme;
      LexerDFA::State final_state = LexerDFA::DEAD;
   };

   std::ofstream error_stream;

   string input;
   size_t position = 0;
   int current_line = 1;
   int current_column = 1;

   StaticTable<string> keywords_table;
   StaticTable<string> operators_table;
   StaticTable<string> separators_table;

   DynamicTable identifiers_table;
   DynamicTable constants_table;

   LexerDFA identifier_dfa;
   LexerDFA number_dfa;
   LexerDFA operator_dfa;
   LexerDFA separator_dfa;

public:
   Scanner(
      const string &source,
      const StaticTable<string> &keywords,
      const StaticTable<string> &operators,
      const StaticTable<string> &separators,
      const string &error_log_path = "errors.log"
   );

   Token next_token( );

private:
   void init_dfas( );

   void write_error( int line, int column, const string lexeme, const string msg );
   void skip_whitespace( );
   bool skip_line_comment( );
   bool skip_block_comment( );
   void skip_whitespace_and_comments( );

   MatchResult run_dfa( LexerDFA &dfa ) const;
   Token try_recognize_with_dfas( );

   void update_line_column( const string &lexeme );
};
