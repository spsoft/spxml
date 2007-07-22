
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "spxmlcodec.hpp"
#include "spxmlutils.hpp"

const char SP_XmlStringCodec :: XML_CHARS [] =
		{ '<', '>', '&', '\'', '"' };
const char * SP_XmlStringCodec :: ESC_CHARS [] =
		{ "&lt;", "&gt;", "&amp;", "&apos;", "&quot;" };

int SP_XmlStringCodec :: decode( const char * encodeValue,
		SP_XmlStringBuffer * outBuffer )
{
	const char * pos = encodeValue;
	for( ; '\0' != *pos; ) {
		if( '&' == *pos ) {
			int index = -1;
			int len = 0;
			for( int i = 0; i < (int)( sizeof( ESC_CHARS ) / sizeof( ESC_CHARS[0] ) ); i++ ) {
				len = strlen( ESC_CHARS[ i ] );
				if( 0 == strncmp( pos, ESC_CHARS[i], len ) ) {
					index = i;
					break;
				}
			}
			if( index >= 0 ) {
				outBuffer->append( XML_CHARS[ index ] );
				pos += len;
			} else {
				char * next = "";
				int ch = 0;
				if( '#' == *( pos + 1 ) ) {
					if( 'x' == *( pos + 2 ) ) {
						ch = strtol( pos + 3, &next, 16 );
					} else {
						ch = strtol( pos + 2, &next, 10 );
					}
				}

				// TODO: fully support xml entity, currently only support printable and space char
				if( ';' == *next && isascii( ch ) && ( isprint( ch ) || isspace( ch ) ) ) {
					outBuffer->append( ch );
					pos = next + 1;
				} else {
					outBuffer->append( *pos++ );
				}
			}
		} else {
			outBuffer->append( *pos++ );
		}
	}

	return 0;
}

int SP_XmlStringCodec :: encode( const char * decodeValue,
		SP_XmlStringBuffer * outBuffer )
{
	const char * pos = decodeValue;
	for( ; '\0' != *pos; pos++ ) {
		int index = -1;
		for( int i = 0; i < (int)( sizeof( XML_CHARS ) / sizeof( XML_CHARS[0] ) ); i++ ) {
			if( XML_CHARS[i] == *pos ) {
				index = i;
				break;
			}
		}
		if( index >= 0 ) {
			outBuffer->append( ESC_CHARS[ index ] );
		} else {
			outBuffer->append( *pos );
		}	
	}

	return 0;
}

int SP_XmlStringCodec :: isNameChar( char c )
{
	return isalnum(c) || c == ':' || c == '-' || c == '.' || c == '_';
}

//=========================================================

