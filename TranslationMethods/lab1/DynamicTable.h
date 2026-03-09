#pragma once

#include <unordered_map>
#include <iterator>
#include <string>
#include <fstream>
#include <ostream>
#include <iomanip>

#include "lex.h"


class DynamicTable
{
private:
   std::unordered_map<std::string, lex> table;

public:
   DynamicTable( ) {};

   ~DynamicTable( )
   {
      table.clear( );
   }

   bool is_contain( const std::string &name )
   {
       return table.find( name ) != table.end();
   }

   int add_elem( const std::string &name )
   {
      auto result = table.find( name );

      if ( result != table.end( ) )
         return 1; 

      table.emplace( name, lex() );

      return 0;
   }

   int find_lex( const std::string &name, lex &buf )
   {
      auto result = table.find( name );

      if ( result == table.end( ) )
         return 1;

      buf = result->second;

      return 0;
   }

   int get_type( const std::string &name, LEX_TYPE &buf ) {
   auto result = table.find( name );

      if ( result == table.end( ) )
         return 1;

      return result->second.get_type( buf );

   }


   int get_value( const std::string &name, int &buf ) {
      auto result = table.find( name );

      if ( result == table.end( ) )
         return 1;

      return result->second.get_value( buf );
   }

   int set_attr( const std::string &name, const LEX_TYPE &type )
   {
      auto result = table.find( name );

      if ( result == table.end( ) )
         return 1;

      result->second.set_type( type );

      return 0;
   }

   int set_attr( const std::string &name, const int &value )
   {
      auto result = table.find( name );

      if ( result == table.end( ) )
         return 1;

      result->second.set_value( value );

      return 0;
   }

   int print_table( std::ostream &stream )
   {
      stream << std::setw( 10 ) << "NAME" << "TYPE" << "VALUE";

      for ( auto &elem : table )
      {
         LEX_TYPE buf_type;
         std::optional<int> buf_value;

         elem.second.get_type( buf_type );

         std::string value = "NONE";

         // Try to get value if available
         int v;
         if ( elem.second.get_value( v ) == 0 )
            value = std::to_string( v );

         stream << std::setw( 10 ) << elem.first << std::to_string( buf_type ) << value;
      }

      return 0;
   }

};

