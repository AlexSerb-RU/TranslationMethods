#pragma once

#include <unordered_map>
#include <iterator>
#include <string>
#include <fstream>
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
      auto result = table.find( name );
      
      if ( result == table.end( ) )
         return false;

      return true;
   }

   int add_elem( const std::string &name ) {}

   int find_lex( const std::string &name, lex &buf )
   {
      auto result = table.find( name );

      if ( result == table.end( ) )
         return 1;

      buf = result->second;

      return 0;
   }

   int get_type( const std::string &name, LEX_TYPE buf ) {}

   int get_vallue( const std::string &name, int &buf ) {}

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
      // ...
   }

   int print_table( std::ofstream stream )
   {
      stream << std::setw( 10 ) << "NAME" << "TYPE" << "VALUE";

      for ( auto elem : table )
      {
         LEX_TYPE buf_type;
         std::optional<int> buf_value;
         
         elem.second.get_value( *buf_value );
         elem.second.get_type( buf_type );

         std::string value;
         if ( buf_value.has_value( ) )
            value = std::to_string( buf_value.value( ) );
         else
            value = "NONE";

         stream << std::setw( 10 ) << elem.first << std::to_string( buf_type ) << value;
      }
   }

};

