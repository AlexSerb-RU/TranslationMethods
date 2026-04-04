#pragma once

#include <string>
#include <optional>

enum LEX_TYPE { UNDEFINED, CONSTANT, ID };

struct lex
{
private:
   LEX_TYPE type;
   std::optional<int> value;

public:
   lex( LEX_TYPE _type = UNDEFINED, int _value = INT_MAX );

   int set_type( LEX_TYPE _type );
   int set_value( int _value );
   int get_type( LEX_TYPE &buf ) const;
   int get_value( int &buf ) const;

   operator std::string( ) const;
};

