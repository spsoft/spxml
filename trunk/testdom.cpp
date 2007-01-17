/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include "spdomparser.hpp"
#include "spdomiterator.hpp"
#include "spxmlnode.hpp"

int main( int argc, char * argv[] )
{
	if( argc < 2 ) {
		printf( "Usage: %s <xml_file>\n", argv[0] );
		exit( -1 );
	}

	FILE * fp = fopen ( argv[1], "r" );
	if( NULL == fp ) {
		printf( "cannot not open %s\n", argv[1] );
		exit( -1 );
	}

	struct stat aStat;
	char * source = NULL;
	stat( argv[1], &aStat );
	source = ( char * ) malloc ( aStat.st_size + 1 );
	fread ( source, aStat.st_size, sizeof ( char ), fp );
	fclose ( fp );
	source[ aStat.st_size ] = 0;

	SP_XmlDomParser parser;

	for( int i = 0; i < (int)strlen( source ); i++ ) {
		parser.append( source + i, 1 );
	}

	free( source );

	SP_XmlDomBuffer buffer( parser.getDocument() );
	puts( buffer.getBuffer() );

	SP_DomIterator iter( parser.getDocument() );
	for( const SP_XmlNode * node = iter.getNext();
			NULL != node;
			node = iter.getNext() ) {
		//printf( "=============================== %p\n", node );
		SP_XmlDomBuffer buffer2( node );
		//puts( buffer2.getBuffer() );
	}

	if( NULL != parser.getError() ) {
		printf( "\n\nerror: %s\n", parser.getError() );
	}

#ifdef WIN32
	getchar();
#endif

	return 0;
}

