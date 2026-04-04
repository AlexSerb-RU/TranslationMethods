#include "DynamicTable.h"

DynamicTable::DynamicTable( )
{
}

DynamicTable::~DynamicTable( )
{
   table.clear( );
}

bool DynamicTable::is_contain( const std::string &name )
{
   return table.find( name ) != table.end( );
}

int DynamicTable::add_elem( const std::string &name )
{
   auto result = table.find( name );

   if ( result != table.end( ) )
      return 1;

   table.emplace( name, lex( ) );

   return 0;
}

int DynamicTable::add_elem( const std::string &name, LEX_TYPE type )
{
   if ( add_elem( name ) != 0 )
      return 1;

   if ( set_attr( name, type ) )
      return -1;

   return 0;
}

int DynamicTable::add_elem( const std::string &name, int value )
{
   if ( add_elem( name ) != 0 )
      return 1;

   if ( set_attr( name, value ) )
      return -1;

   return 0;
}

int DynamicTable::add_elem( const std::string &name, int value, LEX_TYPE type )
{
   if ( add_elem( name ) != 0 )
      return 1;

   if ( set_attr( name, value, type ) )
      return -1;

   return 0;
}

int DynamicTable::find_lex( const std::string &name, lex &buf )
{
   auto result = table.find( name );

   if ( result == table.end( ) )
      return 1;

   buf = result->second;

   return 0;
}

int DynamicTable::get_type( const std::string &name, LEX_TYPE &buf )
{
   auto result = table.find( name );

   if ( result == table.end( ) )
      return 1;

   return result->second.get_type( buf );
}

int DynamicTable::get_value( const std::string &name, int &buf )
{
   auto result = table.find( name );

   if ( result == table.end( ) )
      return 1;

   return result->second.get_value( buf );
}

int DynamicTable::set_attr( const std::string &name, const LEX_TYPE &type )
{
   auto result = table.find( name );

   if ( result == table.end( ) )
      return 1;

   result->second.set_type( type );

   return 0;
}

int DynamicTable::set_attr( const std::string &name, const int &value )
{
   auto result = table.find( name );

   if ( result == table.end( ) )
      return 1;

   result->second.set_value( value );

   return 0;
}

int DynamicTable::set_attr( const std::string &name, const int &value, const LEX_TYPE type )
{
   auto result = table.find( name );

   if ( result == table.end( ) )
      return 1;

   result->second.set_value( value );
   result->second.set_type( type );

   return 0;
}

int DynamicTable::print_table( std::ostream &stream )
{
   stream << std::setw( 10 ) << "NAME\t" << "TYPE\t" << "VALUE\n";

   for ( auto &elem : table )
   {
      LEX_TYPE buf_type;
      std::optional<int> buf_value;

      elem.second.get_type( buf_type );

      std::string value = "NONE";

      int v;
      if ( elem.second.get_value( v ) == 0 )
         value = std::to_string( v );

      stream << std::setw( 10 ) << elem.first << "\t" << std::to_string( buf_type ) << "\t" << value << "\n";
   }

   return 0;
}