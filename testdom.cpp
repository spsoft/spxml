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
	char * filename = NULL;

	if( argc < 2 ) {
#		ifdef WIN32
		filename = "..\\test.xml";
#		else
		printf( "Usage: %s <xml_file>\n", argv[0] );
		exit( -1 );
#		endif
	} else {
		filename = argv[1];
	}

	FILE * fp = fopen ( filename, "r" );
	if( NULL == fp ) {
		printf( "cannot not open %s\n", filename );
		exit( -1 );
	}

	struct stat aStat;
	char * source = NULL;
	stat( filename, &aStat );
	source = ( char * ) malloc ( aStat.st_size + 1 );
	fread ( source, aStat.st_size, sizeof ( char ), fp );
	fclose ( fp );
	source[ aStat.st_size ] = 0;

	SP_XmlDomParser parser;
	parser.append( source, strlen( source ) );
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

