/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdarg.h>
#include <ctype.h>

#include "spxmlutils.hpp"

//=========================================================

const int SP_XmlArrayList::LAST_INDEX = -1;

SP_XmlArrayList :: SP_XmlArrayList( int initCount )
{
	mMaxCount = initCount <= 0 ? 2 : initCount;
	mCount = 0;
	mFirst = (void**)malloc( sizeof( void * ) * mMaxCount );
}

SP_XmlArrayList :: ~SP_XmlArrayList()
{
	free( mFirst );
	mFirst = NULL;
}

int SP_XmlArrayList :: getCount() const
{
	return mCount;
}

int SP_XmlArrayList :: append( void * value )
{
	if( NULL == value ) return -1;

	if( mCount >= mMaxCount ) {
		mMaxCount = ( mMaxCount * 3 ) / 2 + 1;
		mFirst = (void**)realloc( mFirst, sizeof( void * ) * mMaxCount );
		assert( NULL != mFirst );
		memset( mFirst + mCount, 0, ( mMaxCount - mCount ) * sizeof( void * ) );
	}

	mFirst[ mCount++ ] = value;

	return 0;
}

void * SP_XmlArrayList :: takeItem( int index )
{
	void * ret = NULL;

	if( LAST_INDEX == index ) index = mCount -1;
	if( index < 0 || index >= mCount ) return ret;

	ret = mFirst[ index ];

	mCount--;

	if( ( index + 1 ) < mMaxCount ) {
		memmove( mFirst + index, mFirst + index + 1,
			( mMaxCount - index - 1 ) * sizeof( void * ) );
	} else {
		mFirst[ index ] = NULL;
	}

	return ret;
}

const void * SP_XmlArrayList :: getItem( int index ) const
{
	const void * ret = NULL;

	if( LAST_INDEX == index ) index = mCount - 1;
	if( index < 0 || index >= mCount ) return ret;

	ret = mFirst[ index ];

	return ret;
}

//=========================================================

SP_XmlQueue :: SP_XmlQueue()
{
	mMaxCount = 8;
	mEntries = (void**)malloc( sizeof( void * ) * mMaxCount );

	mHead = mTail = mCount = 0;
}

SP_XmlQueue :: ~SP_XmlQueue()
{
	free( mEntries );
	mEntries = NULL;
}

void SP_XmlQueue :: push( void * item )
{
	if( mCount >= mMaxCount ) {
		mMaxCount = ( mMaxCount * 3 ) / 2 + 1;
		void ** newEntries = (void**)malloc( sizeof( void * ) * mMaxCount );

		unsigned int headLen = 0, tailLen = 0;
		if( mHead < mTail ) {
			headLen = mTail - mHead;
		} else {
			headLen = mCount - mTail;
			tailLen = mTail;
		}

		memcpy( newEntries, &( mEntries[ mHead ] ), sizeof( void * ) * headLen );
		if( tailLen ) {
			memcpy( &( newEntries[ headLen ] ), mEntries, sizeof( void * ) * tailLen );
		}

		mHead = 0;
		mTail = headLen + tailLen;

		free( mEntries );
		mEntries = newEntries;
	}

	mEntries[ mTail++ ] = item;
	mTail = mTail % mMaxCount;
	mCount++;
}

void * SP_XmlQueue :: pop()
{
	void * ret = NULL;

	if( mCount > 0 ) {
		ret = mEntries[ mHead++ ];
		mHead = mHead % mMaxCount;
		mCount--;
	}

	return ret;
}

void * SP_XmlQueue :: top()
{
	return mCount > 0 ? mEntries[ mHead ] : NULL;
}

//=========================================================

SP_XmlStringBuffer :: SP_XmlStringBuffer()
{
	init();
}

void SP_XmlStringBuffer :: init()
{
	mSize = 0;
	mMaxSize = 8;
	mBuffer = (char*)malloc( mMaxSize );
	memset( mBuffer, 0, mMaxSize );
}

SP_XmlStringBuffer :: ~SP_XmlStringBuffer()
{
	free( mBuffer );
}

int SP_XmlStringBuffer :: append( char c )
{
	if( mSize >= ( mMaxSize - 1 ) ) {
		mMaxSize += ( mMaxSize * 3 ) / 2 + 1;
		mBuffer = (char*)realloc( mBuffer, mMaxSize );
		assert( NULL != mBuffer );
		memset( mBuffer + mSize, 0, mMaxSize - mSize );
	}
	mBuffer[ mSize++ ] = c;

	return 0;
}

int SP_XmlStringBuffer :: append( const char * value, int size )
{
	if( NULL == value ) return -1;

	size = ( size <= 0 ? strlen( value ) : size );
	if( size <= 0 ) return -1;

	if( ( size + mSize ) > ( mMaxSize - 1 ) ) {
		mMaxSize += size;
		mBuffer = (char*)realloc( mBuffer, mMaxSize );
		assert( NULL != mBuffer );
		memset( mBuffer + mSize, 0, mMaxSize - mSize );
	}

	memcpy( mBuffer + mSize, value, size );
	mSize += size;

	return 0;
}

int SP_XmlStringBuffer :: getSize() const
{
	return mSize;
}

const char * SP_XmlStringBuffer :: getBuffer() const
{
	return mBuffer;
}

char * SP_XmlStringBuffer :: takeBuffer()
{
	char * ret = mBuffer;

	mBuffer = NULL;
	init();

	return ret;
}

void SP_XmlStringBuffer :: clean()
{
	memset( mBuffer, 0, mMaxSize );
	mSize = 0;
}

//=========================================================

const char SP_XmlStringUtils :: XML_CHARS [] =
		{ '<', '>', '&', '\'', '"' };
const char * SP_XmlStringUtils :: ESC_CHARS [] =
		{ "&lt;", "&gt;", "&amp;", "&apos;", "&quot;" };

int SP_XmlStringUtils :: decode( const char * encodeValue,
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
				outBuffer->append( *pos++ );
			}
		} else {
			outBuffer->append( *pos++ );
		}
	}

	return 0;
}

int SP_XmlStringUtils :: encode( const char * decodeValue,
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

int SP_XmlStringUtils :: isNameChar( char c )
{
	return isalnum(c) || c == ':' || c == '-' || c == '.' || c == '_';
}

#ifdef WIN32
int snprintf(char *s,  size_t  n,  const  char  *format,  /*args*/ ...)
{
	int ret = 0;

	va_list vaList;
	va_start( vaList, format );
	ret = vsprintf( s, format, vaList );
	va_end( vaList );

	return ret;
}
#endif

//=========================================================

