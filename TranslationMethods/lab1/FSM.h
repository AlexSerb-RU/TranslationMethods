#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <unordered_set>
#include <algorithm>

#include "StaticTable.h"
#include "DynamicTable.h"

using std::vector;

// 1. Токен - результат работы сканера
struct Token {
   enum Type {
      IDENTIFIER = 0,    // идентификатор
      KEYWORD,           // ключевое слово
      INTEGER,           // целая константа
      CONSTANT,          // именованая константа (только int)
      OPERATOR,          // оператор (+, -, *, /, =, ...)
      SEPARATOR,         // разделитель (;, ,, (, ), {, }, ...) + комментарий
      UNKNOWN,           // неизвестный символ
      END_OF_FILE      // конец файла
   };

   Type type;
   std::string lexeme;     // распознанная лексема
   int line;               // строка (для ошибок)
   int column;             // колонка (для ошибок)

   // Для чисел можно хранить значение
   union {
      int int_value;
      double float_value;
   };

   Token( Type t, const std::string &lex, int l, int c )
      : type( t ), lexeme( lex ), line( l ), column( c ) {
   }
};

// 2. ДКА для одного типа лексем
struct LexerDFA {
   using State = int;

   struct Transition {
      std::function<bool( char )> condition;
      State next_state;
   };

   State start_state;
   std::unordered_set<State> final_states;    // конечные состояния
   std::unordered_map<State, std::vector<Transition>> transitions;

   // Для каждого конечного состояния - какой токен он производит
   std::unordered_map<State, Token::Type> state_to_token_type;
};

// 3. Основной сканер, использующий набор ДКА
class Scanner {
private:
   // Входной текст
   std::string input;
   size_t position;
   int current_line;
   int current_column;

   // Таблица ключевых слов
   std::unordered_map<std::string, Token::Type> keywords;

   // Набор ДКА для разных классов лексем
   LexerDFA identifier_dfa;      // распознает [a-zA-Z_][a-zA-Z0-9_]*
   LexerDFA number_dfa;          // распознает целые и вещественные числа
   LexerDFA operator_dfa;        // распознает операторы (+, +=, ++, и т.д.)
   LexerDFA separator_dfa;       // распознает разделители

   // Не  распознаём - комментарии и пробелы исключаются на этапе сканера.
   //LexerDFA whitespace_dfa;      // распознает пробельные символы
   //LexerDFA comment_dfa;         // распознает комментарии

public:
   Scanner( const std::string &source );

   Token next_token( );

private:
   void init_keywords( );
   void init_dfas( );
   void skip_whitespace_and_comments( );
   std::string run_dfa( const LexerDFA &dfa );
   Token try_recognize_with_dfas( );
   void update_line_column( const std::string &lexeme );
};

class FSM
{
private:
   int current_state;

   vector<vector<int>> table;
   /// state | symbol1 | symbol2 | symbol3
   ///  st1  | next_st | -//-    | -//-
   ///  st2  | next_st | -//-    | -//-

   vector<char> symbols;  // список символов, по которым происходит переход
   vector<int> final_states;
   std::vector<std::unordered_map<char, int>> transitions_map;

   int get_symbol_index( char symbol );

   template <typename T>
   void build_table( const StaticTable<T> &st )
   {
      transitions_map.clear( );
      transitions_map.emplace_back( );

      int idx = 0;
      while ( true )
      {
         T word = st.get_element( idx++ );
         if ( word == T( ) ) break;

         int state = 0;
         for ( char ch : word )
         {
            auto &m = transitions_map[state];
            auto it = m.find( ch );
            if ( it == m.end( ) )
            {
               int new_state = static_cast<int>(transitions_map.size( ));
               transitions_map.emplace_back( );
               m[ch] = new_state;
               state = new_state;
            }
            else
            {
               state = it->second;
            }
         }
      }
   }

   template <typename T>
   void build_final_states( const StaticTable<T> &st )
   {
      final_states.clear( );

      int idx = 0;
      while ( true )
      {
         T word = st.get_element( idx++ );
         if ( word == T( ) ) break;

         int state = 0;
         bool ok = true;
         for ( char ch : word )
         {
            auto &m = transitions_map[state];
            auto it = m.find( ch );
            if ( it == m.end( ) )
            {
               ok = false;
               break;
            }
            state = it->second;
         }
         if ( ok )
            final_states.push_back( state );
      }

      std::sort( final_states.begin( ), final_states.end( ) );
      final_states.erase( std::unique( final_states.begin( ), final_states.end( ) ), final_states.end( ) );
   }

   template <typename T>
   void fill_up_symbols( const StaticTable<T> &table_static )
   {
      std::unordered_set<char> symset;
      for ( const auto &m : transitions_map )
      {
         for ( const auto &p : m )
            symset.insert( p.first );
      }

      symbols.assign( symset.begin( ), symset.end( ) );
      std::sort( symbols.begin( ), symbols.end( ) );

      size_t states_count = transitions_map.size( );
      size_t symbols_count = symbols.size( );

      table.assign( static_cast<int>(states_count), vector<int>( static_cast<int>(symbols_count), 0 ) );

      for ( size_t s = 0; s < states_count; ++s )
      {
         for ( const auto &p : transitions_map[s] )
         {
            char ch = p.first;
            int next_state = p.second;
            auto it = std::find( symbols.begin( ), symbols.end( ), ch );
            if ( it != symbols.end( ) )
            {
               int col = static_cast<int>( std::distance( symbols.begin( ), it ) );
               table[static_cast<int>( s )][col] = next_state;
            }
         }
      }
   }

public:
   template <typename T>
   FSM( StaticTable<T> table )
   {
      build_table( table );
      build_final_states( table );
      fill_up_symbols( table );
   }

   int process_symbol( char symbol );


};
