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
   DynamicTable( );
   ~DynamicTable( );

   bool is_contain( const std::string &name );

   int add_elem( const std::string &name );
   int add_elem( const std::string &name, LEX_TYPE type );
   int add_elem( const std::string &name, int value );
   int add_elem( const std::string &name, int value, LEX_TYPE type );

   int find_lex( const std::string &name, lex &buf );
   int get_type( const std::string &name, LEX_TYPE &buf );
   int get_value( const std::string &name, int &buf );

   int set_attr( const std::string &name, const LEX_TYPE &type );
   int set_attr( const std::string &name, const int &value );
   int set_attr( const std::string &name, const int &value, const LEX_TYPE type );

   int print_table( std::ostream &stream );
};

