#include "FSM.h"

#include <algorithm>
#include <cctype>
#include <stdexcept>

using namespace std;

namespace {

   vector<char> make_identifier_symbols( ) {
      vector<char> s;
      for ( char c = 'a'; c <= 'z'; ++c ) s.push_back( c );
      for ( char c = 'A'; c <= 'Z'; ++c ) s.push_back( c );
      for ( char c = '0'; c <= '9'; ++c ) s.push_back( c );
      s.push_back( '_' );
      return s;
   }

   vector<char> make_integer_symbols( ) {
      vector<char> s;
      for ( char c = '0'; c <= '9'; ++c ) s.push_back( c );
      return s;
   }

   void fill_symbol_index( LexerDFA &dfa ) {
      dfa.symbol_to_index_map.clear( );
      for ( int i = 0; i < static_cast<int>( dfa.symbols.size( ) ); ++i ) {
         dfa.symbol_to_index_map[dfa.symbols[i]] = i;
      }
   }

   LexerDFA make_identifier_dfa( ) {
      LexerDFA dfa;
      dfa.start_state = 0;
      dfa.symbols = make_identifier_symbols( );
      fill_symbol_index( dfa );
      dfa.init_matrix( 2 );

      // 0 --[a-zA-Z_]--> 1
      for ( char c = 'a'; c <= 'z'; ++c ) dfa.matrix[0][dfa.symbol_to_index_map[ c ]] = 1;
      for ( char c = 'A'; c <= 'Z'; ++c ) dfa.matrix[0][dfa.symbol_to_index_map[ c ]] = 1;
      dfa.matrix[0][dfa.symbol_to_index_map[ '_' ]] = 1;

      // 1 --[a-zA-Z0-9_]--> 1
      for ( char c = 'a'; c <= 'z'; ++c ) dfa.matrix[1][dfa.symbol_to_index_map[ c ]] = 1;
      for ( char c = 'A'; c <= 'Z'; ++c ) dfa.matrix[1][dfa.symbol_to_index_map[ c ]] = 1;
      for ( char c = '0'; c <= '9'; ++c ) dfa.matrix[1][dfa.symbol_to_index_map[ c ]] = 1;
      dfa.matrix[1][dfa.symbol_to_index_map[ '_' ]] = 1;

      dfa.final_states.insert( 1 );
      dfa.state_to_token_type[1] = Token::IDENTIFIER;
      return dfa;
   }

   LexerDFA make_integer_dfa( ) {
      LexerDFA dfa;
      dfa.start_state = 0;
      dfa.symbols = make_integer_symbols( );
      fill_symbol_index( dfa );
      dfa.init_matrix( 2 );

      for ( char c = '0'; c <= '9'; ++c ) {
         dfa.matrix[0][dfa.symbol_to_index_map[ c ]] = 1;
         dfa.matrix[1][dfa.symbol_to_index_map[ c ]] = 1;
      }

      dfa.final_states.insert( 1 );
      dfa.state_to_token_type[1] = Token::INTEGER_CONSTANT;
      return dfa;
   }

   LexerDFA make_keyword_from_table( StaticTable<string> &table, Token::Type type ) {
      // trie-like DFA from a finite set of strings
      LexerDFA dfa;
      dfa.start_state = 0;

      unordered_set<char> symbol_set;
      for ( int i = 0; i < table.size( ); ++i ) {
         const string word = table.get_element( i );
         for ( char c : word ) symbol_set.insert( c );
      }

      dfa.symbols.assign( symbol_set.begin( ), symbol_set.end( ) );
      sort( dfa.symbols.begin( ), dfa.symbols.end( ) );
      fill_symbol_index( dfa );

      vector<unordered_map<char, int>> temp( 1 );

      for ( int i = 0; i < table.size( ); ++i ) {
         const string word = table.get_element( i );
         int state = 0;

         for ( char c : word ) {
            auto it = temp[state].find( c );
            if ( it == temp[state].end( ) ) {
               int new_state = static_cast<int>( temp.size( ) );
               temp.emplace_back( );
               temp[state][c] = new_state;
               state = new_state;
            }
            else {
               state = it->second;
            }
         }

         dfa.final_states.insert( state );
         dfa.state_to_token_type[state] = type;
      }

      dfa.init_matrix( static_cast<int>(temp.size( )) );
      for ( int s = 0; s < static_cast<int>( temp.size( ) ); ++s ) {
         for ( const auto &[c, next] : temp[s] ) {
            dfa.matrix[s][dfa.symbol_to_index_map[ c ]] = next;
         }
      }

      return dfa;
   }

} // namespace

Scanner::Scanner(
   const string &source,
   const StaticTable<string> &keywords,
   const StaticTable<string> &operators,
   const StaticTable<string> &separators
)
   : input( source ),
   keywords_table( keywords ),
   operators_table( operators ),
   separators_table( separators ) {
   init_dfas( );
}

void Scanner::init_dfas( ) {
   identifier_dfa = make_identifier_dfa( );
   number_dfa = make_integer_dfa( );

   // finite-set DFA
   operator_dfa = make_keyword_from_table( operators_table, Token::OPERATOR );
   separator_dfa = make_keyword_from_table( separators_table, Token::SEPARATOR );
}

void Scanner::skip_whitespace( ) {
   while ( position < input.size( ) ) {
      unsigned char c = static_cast<unsigned char>( input[position] );
      if ( !std::isspace( c ) ) break;

      if ( input[position] == '\n' ) {
         ++current_line;
         current_column = 1;
      }
      else {
         ++current_column;
      }
      ++position;
   }
}

bool Scanner::skip_line_comment( ) {
   if ( position + 1 >= input.size( ) ) return false;
   if ( input[position] != '/' || input[position + 1] != '/' ) return false;

   while ( position < input.size( ) && input[position] != '\n' ) {
      ++position;
      ++current_column;
   }
   return true;
}

bool Scanner::skip_block_comment( ) {
   if ( position + 1 >= input.size( ) ) return false;
   if ( input[position] != '/' || input[position + 1] != '*' ) return false;

   position += 2;
   current_column += 2;

   while ( position + 1 < input.size( ) ) {
      if ( input[position] == '*' && input[position + 1] == '/' ) {
         position += 2;
         current_column += 2;
         return true;
      }

      if ( input[position] == '\n' ) {
         ++current_line;
         current_column = 1;
         ++position;
      }
      else {
         ++position;
         ++current_column;
      }
   }

   // unterminated block comment: consume to end
   position = input.size( );
   return true;
}

void Scanner::skip_whitespace_and_comments( ) {
   while ( true ) {
      size_t old_pos = position;
      int old_line = current_line;
      int old_col = current_column;

      skip_whitespace( );
      if ( skip_line_comment( ) ) continue;
      if ( skip_block_comment( ) ) continue;

      if ( position == old_pos && current_line == old_line && current_column == old_col ) {
         break;
      }
   }
}

Scanner::MatchResult Scanner::run_dfa( const LexerDFA &dfa ) const {
   MatchResult result;
   LexerDFA::State state = dfa.start_state;

   size_t i = position;
   size_t last_final_pos = position;
   LexerDFA::State last_final_state = LexerDFA::DEAD;

   while ( i < input.size( ) ) {
      LexerDFA::State next = dfa.move( state, input[i] );
      if ( next == LexerDFA::DEAD ) break;

      state = next;
      ++i;

      if ( dfa.is_final( state ) ) {
         last_final_state = state;
         last_final_pos = i;
      }
   }

   if ( last_final_state != LexerDFA::DEAD ) {
      result.matched = true;
      result.final_state = last_final_state;
      result.type = dfa.state_to_token_type.at( last_final_state );
      result.lexeme = input.substr( position, last_final_pos - position );
   }

   return result;
}

Token Scanner::try_recognize_with_dfas( ) {
   const int token_line = current_line;
   const int token_col = current_column;

   MatchResult best;

   auto choose_better = [&best]( const MatchResult &candidate ) {
      if ( !candidate.matched ) return;
      if ( !best.matched || candidate.lexeme.size( ) > best.lexeme.size( ) ) {
         best = candidate;
      }
      };

   choose_better( run_dfa( identifier_dfa ) );
   choose_better( run_dfa( number_dfa ) );
   choose_better( run_dfa( operator_dfa ) );
   choose_better( run_dfa( separator_dfa ) );

   if ( !best.matched ) {
      return Token( Token::UNKNOWN, string( 1, input[position] ), token_line, token_col );
   }

   Token token( best.type, best.lexeme, token_line, token_col );

   if ( best.type == Token::IDENTIFIER ) {
      if ( keywords_table.is_contain( best.lexeme ) ) {
         token.type = Token::KEYWORD;
      }
      else {
         identifiers_table.add_elem( best.lexeme );
      }
   }
   else if ( best.type == Token::INTEGER ) {
      token.int_value = stoi( best.lexeme );
      constants_table.add_elem( best.lexeme, token.int_value );
   }

   return token;
}

Token Scanner::next_token( ) {
   skip_whitespace_and_comments( );

   if ( position >= input.size( ) ) {
      return Token( Token::END_OF_FILE, "", current_line, current_column );
   }

   Token token = try_recognize_with_dfas( );
   position += token.lexeme.size( );
   update_line_column( token.lexeme );
   return token;
}

void Scanner::update_line_column( const string &lexeme ) {
   for ( char c : lexeme ) {
      if ( c == '\n' ) {
         ++current_line;
         current_column = 1;
      }
      else {
         ++current_column;
      }
   }
}