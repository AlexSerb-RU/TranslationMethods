#pragma once

#include <string>

enum LEX_TYPE {UNDEFINED, CONSTANT, ID, KEYWORD, SEP, OPERATION_SYMBOL};

struct lex
{
private:
   std::string name;

   int value;
   LEX_TYPE type;

public:
   lex( std::string _name, LEX_TYPE _type = UNDEFINED, int _value = 0 )
      : name( _name ), type( _type ), value( _value )
   {}

   int set_type( LEX_TYPE _type )
   {
      type = _type;

      return 0;
   }

   int set_value( int _value )
   {
      value = _value;

      return 0;
   }

   LEX_TYPE get_type( ) const
   {
      return type;
   }

   int get_value( int &buf ) const
   {
      if ( type == CONSTANT or type == ID )
      {
         buf = value;
         return 0;
      }

      return -1;
   }
};

