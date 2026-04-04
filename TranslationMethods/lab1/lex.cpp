#include "lex.h"

lex::lex( LEX_TYPE _type, int _value )
   : type( _type ), value( _value )
{
}

int lex::set_type( LEX_TYPE _type )
{
   type = _type;
   return 0;
}

int lex::set_value( int _value )
{
   value = _value;
   return 0;
}

int lex::get_type( LEX_TYPE &buf ) const
{
   buf = type;
   return 0;
}

int lex::get_value( int &buf ) const
{
   if ( value.has_value( ) )
   {
      buf = value.value( );
      return 0;
   }

   return 1;
}

lex::operator std::string( ) const
{
   LEX_TYPE buf1;
   int buf2;

   get_type( buf1 );
   get_value( buf2 );

   return "Type: " + std::to_string( buf1 ) +
      "\nValue: " + std::to_string( buf2 );
}