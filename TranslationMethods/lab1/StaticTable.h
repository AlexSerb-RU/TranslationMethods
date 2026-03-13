#pragma once

#include <set>
#include <string>
#include <fstream>
#include <iterator>

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

   int size( )
   {
      return static_cast<int>( table.size( ) );
   };

   int read_file( std::string filepath )
   {
      std::ifstream stream( filepath );

      if ( !stream )
         return 1;

      T tmp;

      while ( stream >> tmp )
      {
         add_element( tmp );
      }

      return 0;
   };

   bool is_contain( T element )
   {
      return table.find( element ) != table.end( );
   };

   int add_element( T element )
   {
      auto result = table.insert( element );

      if ( result.second )
         return 0; // inserted

      return 1; // already existed
   };

   T get_element( int idx )
   {
      if ( idx < 0 || static_cast<size_t>( idx ) >= table.size( ) )
         return T( );

      auto it = table.begin( );

      std::advance( it, idx );

      return *it;
   };

   int find( T element )
   {
      auto it = table.find( element );

      if ( it == table.end( ) )
         return -1;

      return static_cast<int>( std::distance( table.begin( ), it ) );
   }; // АЛгоритм ппоиска в упорядоченной таблице

   T operator []( int idx )
   {
      return get_element( idx );
   }
};

