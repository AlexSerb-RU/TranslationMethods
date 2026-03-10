#include "DynamicTable.h"
#include "StaticTable.h"
#include <iostream>

using namespace std;

enum OPTIONS {ADD_ELEM=1, GET_ELEM, IS_CONTAIN, SET_TYPE, SET_VALUE, PRINT_TABLE};

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
         << "2. Get element\n"
         << "3. Does the element exist?\n"
         << "4. Set type of element\n"
         << "5. Set value of element\n"
         << "6. Print table\n";
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

   void add_elem( )
   {
      enter_elem_name( );

      if ( table.add_elem( elem_name_buf ) != 0 )
      {
         user_stream << "\nCouldn't add element\n";
         return;
      }

      user_stream << "\nThe element has been added\n";

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

      LEX_TYPE type;
      int buf;

      user_stream << "\nEnter TYPE (UNDEFINED=0, CONSTANT=1, ID=2): ";
      cin >> buf;
      user_stream << "\n";
      
      type = LEX_TYPE( buf );
      
      if ( table.set_attr( elem_name_buf, type ) != 0 )
      {
         user_stream << "\nCouldn't set the token TYPE\n";
         return;
      }

      user_stream << "\nThe token TYPE is set\n";
   }

   void set_value( )
   {
      enter_elem_name( );

      int value;

      user_stream << "\nEnter VALUE (e.g. 4631): ";
      cin >> value;
      user_stream << "\n";

      if ( table.set_attr( elem_name_buf, value ) != 0 )
      {
         user_stream << "\nCouldn't set the token VALUE\n";
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


int main( )
{
   string out_filename = "out.txt";
   string words_fp = "input/words.txt";

   auto words = StaticTable<string>( );
   words.read_file( words_fp );
   cout << "\nwords: find switch: id: " << words.find( "switch" ) << "\n";
   cout << "\nwords: find abcde: id: " << words.find( "abcde" ) << "\n";

   auto table = DynamicTable();

   ofstream output( out_filename );

   ConsoleApp<lex> App( cout, output, table );

   App.loop( );

}