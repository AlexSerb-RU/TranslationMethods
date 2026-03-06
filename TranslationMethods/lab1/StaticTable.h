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

   int read_file( std::string filepath ) {};

   bool is_contain( T element ) {};

   int add_element( T element ) {};

   T get_element( int idx ) {};

   int find( T element ) {}; // АЛгоритм ппоиска в упорядоченной таблице

   T operator []( int idx )
   {
      return get_element( idx );
   }
};

