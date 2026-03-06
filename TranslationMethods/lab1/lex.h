#pragma once

#include <string>
#include <optional>

enum LEX_TYPE {UNDEFINED, CONSTANT, ID};  //, KEYWORD, SEP, OPERATION_SYMBOL

struct lex
{
private:
   //std::string name;

   LEX_TYPE type;
   std::optional<int> value;

public:
   lex( LEX_TYPE _type = UNDEFINED, int _value = INT_MAX )
      :  type( _type ), value( _value )  // name( _name )  // std::string _name,
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

   int get_type( LEX_TYPE &buf ) const
   {
      buf = type;

      return 0;
   }

   int get_value( int &buf ) const
   {
      // value.has_value( )
   }

   operator std::string( ) const {
      LEX_TYPE buf1;
      int buf2;

      get_type( buf1 );
      get_value( buf2 );

      return "Type: " + std::to_string( buf1 ) +
         "\nValue: " + std::to_string( buf2 );
   }
};

