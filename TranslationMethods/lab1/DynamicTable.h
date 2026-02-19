#pragma once

#include <unordered_set>

#include "lex.h"
#include <iterator>

class DynamicTable
{
private:
   std::unordered_set<lex> table;

public:
   DynamicTable( ) {};

   ~DynamicTable( )
   {
      table.clear( );
   }

   int find_lex( std::string name, std::unordered_set<lex>::iterator &buf )
   {
      auto _temp = lex( name );
      auto buf = table.find( _temp );  // Может вернуть end()

      return 0;
   }
   
   int find_lex( std::string name, LEX_TYPE type, std::unordered_set<lex>::iterator &buf )
   {
      auto _temp = lex( name, type );
      auto buf = table.find( _temp );  // Может вернуть end()

      return 0;
   }

   void set_attr( std::unordered_set<lex>::iterator  )
   {

   }


};

