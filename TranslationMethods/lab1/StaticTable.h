#pragma once

#include <set>
#include <string>
#include <fstream>

template <typename T>
class StaticTable
{
private:
   std::set<T> table;

public:
   StaticTable( ) {};

   ~StaticTable( ) 
   {
      table.clear( );
   }

   int add_element( T element ) {};

   int read_file( ifstream filepath ) {};

   T get_element( int idx ) {};

   int find( T element ) {};

   bool is_contain( T element ) {};
};

