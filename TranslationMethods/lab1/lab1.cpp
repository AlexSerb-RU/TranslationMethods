#include "DynamicTable.h"
#include "StaticTable.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "FSM.h"


using namespace std;

enum OPTIONS {ADD_ELEM=1, ADD_ELEM_WIWTH_ARGS, GET_ELEM, IS_CONTAIN, SET_TYPE, SET_VALUE, PRINT_TABLE};

template<typename T>
class ConsoleApp
{
private:
   DynamicTable &table;

   ostream &user_stream;
   ostream &out_stream;

   int last_command;

   string elem_name_buf;
   T elem_buf;

public:
   ConsoleApp( ostream &_user_stream, ostream &_out_stream, DynamicTable &_table )
      : user_stream( _user_stream ), out_stream( _out_stream ), table( _table )
   {
   };

   ~ConsoleApp( )
   {
   };

private:
   void print_menu( )
   {
      user_stream << "\n\nDynamic Table options\n"
         << "1. Add element\n"
         << "2. Add element with ARGS\n"
         << "3. Get element\n"
         << "4. Does the element exist?\n"
         << "5. Set type of element\n"
         << "6. Set value of element\n"
         << "7. Print table\n";
   };

   void get_command( )
   {
      user_stream << "\nEnter the command number: ";
      cin >> last_command;
      user_stream << "\n";
   }

   void use_command( )
   {
      switch ( last_command )
      {
         case ADD_ELEM:
            add_elem( );
            break;
         case ADD_ELEM_WIWTH_ARGS:
            add_elem_with_args( );
            break;
         case GET_ELEM:
            get_elem( );
            break;
         case IS_CONTAIN:
            is_contain( );
            break;
         case SET_TYPE:
            set_type( );
            break;
         case SET_VALUE:
            set_value( );
            break;
         case PRINT_TABLE:
            print_all_table( );
            break;
      }
   }

   void enter_elem_name( )
   {
      user_stream << "\nEnter the element NAME: ";
      cin >> elem_name_buf;
      user_stream << "\n";
   }

   int enter_elem_value( )
   {
      string str;
      int buf;

      cin >> str;
      try
      {
         buf = stoi( str );
      }
      catch ( const invalid_argument& e )
      {
         user_stream << "\nThe VALUE not correct, entered is recognized as 0";
         buf = 0;
      }
      catch ( const out_of_range& e )
      {
         user_stream << "\nThe VALUE out of range of INT type, entered is recognized as 0";
         buf = 0;
      }
         

      return buf;
   }

   LEX_TYPE enter_elem_type( )
   {
      string str;
      int buf;

      cin >> str;
      try
      {
         buf = stoi( str );
      }
      catch ( const invalid_argument& e )
      {
         user_stream << "\nThe TYPE not correct, entered is recognized as UNDEFINED";
         buf = 0;
      }
      catch ( const out_of_range& e )
      {
         user_stream << "\nThe TYPE out of range, entered is recognized as UNDEFINED";
         buf = 0;
      }

      if ( 0 >= buf or buf >= 2 )
      {
         user_stream << "\nThe TYPE out of range, entered is recognized as UNDEFINED";
         buf = 0;
      }

      return LEX_TYPE(buf);
   }

   void add_elem( )
   {
      enter_elem_name( );

      if ( table.add_elem( elem_name_buf ) != 0 )
      {
         user_stream << "\nThe element is already contained in the table.\n";
         return;
      }

      user_stream << "\nThe element has been added.\n";

   }

   void add_elem_with_args( )
   {
      int value;
      LEX_TYPE type;

      user_stream << "\nEnter element NAME TYPE(UNDEFINED=0, CONSTANT, ID) VALUE(integer): \n";
      
      cin >> elem_name_buf;
      type = enter_elem_type( );
      value = enter_elem_value( );

      if ( table.add_elem( elem_name_buf ) != 0 )
      {
         user_stream << "\nThe element is already contained in the table.\n";
         return;
      }

      if ( table.set_attr( elem_name_buf, type ) != 0 ) // Маловероятно
      {
         user_stream << "\nThe TYPE setting element was not found!!!\n";
         return;
      }

      if ( table.set_attr( elem_name_buf, value ) != 0 ) // Маловероятно
      {
         user_stream << "\nThe VALUE setting element was not found!!!\n";
         return;
      }
   }

   void get_elem( )
   {
      enter_elem_name( );

      if ( table.find_lex( elem_name_buf, elem_buf ) != 0 )
      {
         user_stream << "\nThe item was not found in the table ( Name: " << elem_name_buf << " )\n";
         return;
      }

      user_stream << "\nthe element is received: \n"
         << "Name: " << elem_name_buf << "\n"
         << string( elem_buf ) << "\n";
   }

   void is_contain( )
   {
      enter_elem_name( );

      if ( table.is_contain( elem_name_buf ) )
         user_stream << "\nYes, the element is contained in the table\n";
      else
         user_stream << "\nNo, the element is not contained in the table.\n";
   }

   void set_type( )
   {
      enter_elem_name( );

      user_stream << "\nEnter TYPE (UNDEFINED=0, CONSTANT=1, ID=2): ";
      LEX_TYPE type = enter_elem_type( );

      user_stream << "\n";
      
      if ( table.set_attr( elem_name_buf, type ) != 0 )
      {
         user_stream << "\nThe TYPE setting element was not found!!!\n";
         return;
      }

      user_stream << "\nThe token TYPE is set\n";
   }

   void set_value( )
   {
      enter_elem_name( );

      

      user_stream << "\nEnter VALUE (e.g. 4631): ";
      int value = enter_elem_value( );

      user_stream << "\n";

      if ( table.set_attr( elem_name_buf, value ) != 0 )
      {
         user_stream << "\nThe VALUE setting element was not found!!!\n";
         return;
      }

      user_stream << "\nThe token VALUE is set\n";
   }

   void print_all_table( )
   {
      table.print_table( user_stream );

      table.print_table( out_stream );
   }

public:
   void loop( )
   {
      while ( true )
      {
         print_menu( );

         get_command( );

         use_command( );
      }
   }

   
};


//int main( )
//{
//   string out_filename = "out.txt";
//   string words_fp = "input/words.txt";
//
//   auto words = StaticTable<string>( );
//   words.read_file( words_fp );
//   cout << "\nwords: find switch: id: " << words.find( "switch" ) << "\n";
//   cout << "\nwords: find abcde: id: " << words.find( "abcde" ) << "\n";
//
//   auto table = DynamicTable();
//
//   ofstream output( out_filename );
//
//   ConsoleApp<lex> App( cout, output, table );
//
//   App.loop( );
//
//}

std::string read_file( const std::string &path ) {
   std::ifstream in( path );
   if ( !in ) {
      throw std::runtime_error( "Cannot open file: " + path );
   }

   std::ostringstream buffer;
   buffer << in.rdbuf( );
   return buffer.str( );
}

std::string token_type_to_string( Token::Type type ) {
   switch ( type ) {
      case Token::IDENTIFIER: return "IDENTIFIER";
      case Token::KEYWORD: return "KEYWORD";
      case Token::INTEGER_CONSTANT: return "INTEGER";
      case Token::CUSTOM_CONSTANT: return "CONSTANT";
      case Token::OPERATOR: return "OPERATOR";
      case Token::SEPARATOR: return "SEPARATOR";
      case Token::END_OF_FILE: return "END_OF_FILE";
      default: return "UNDEFINED_TOKEN_TYPE";
   }
}


int main( )
{
   try {
      const std::string test_name = "../tests/1.cpp";

      const std::string source = read_file( test_name );

      string keywords_path = "../tables/keywords.txt";
      string operators_path = "../tables/operators.txt";
      string separators_path = "../tables/separators.txt";
      StaticTable<std::string> keywords;
      StaticTable<std::string> operators;
      StaticTable<std::string> separators;
      keywords.read_file( keywords_path );
      operators.read_file( operators_path );
      separators.read_file( separators_path );

      Scanner scanner( source, keywords, operators, separators );

      while ( true ) {
         Token token = scanner.next_token( );

         std::cout
            << token_type_to_string( token.type )
            << " | \"" << token.lexeme << "\""
            << " | line = " << token.line
            << ", column = " << token.column
            << '\n';

         if ( token.type == Token::END_OF_FILE ) {
            break;
         }
      }
   }
   catch ( const std::exception &ex ) {
      std::cerr << "Error: " << ex.what( ) << '\n';
      return 1;
   }

   return 0;
}
