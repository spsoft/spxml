/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spxmlutils_hpp__
#define __spxmlutils_hpp__

#include <stdio.h>

typedef struct tagSP_XmlArrayListNode SP_XmlArrayListNode_t;

class SP_XmlArrayList {
public:
	static const int LAST_INDEX;

	SP_XmlArrayList( int initCount = 2 );
	virtual ~SP_XmlArrayList();

	int getCount() const;
	int append( const void * value, int size );
	const void * getItem( int index, int * size = NULL ) const;
	void * takeItem( int index, int * size = NULL );

private:
	SP_XmlArrayList( SP_XmlArrayList & );
	SP_XmlArrayList & operator=( SP_XmlArrayList & );

	int mMaxCount;
	int mCount;
	SP_XmlArrayListNode_t * mFirst;
};

class SP_XmlQueue {
public:
	SP_XmlQueue();
	virtual ~SP_XmlQueue();

	void push( void * item );
	void * pop();
	void * top();

private:
	void ** mEntries;
	unsigned int mHead;
	unsigned int mTail;
	unsigned int mCount;
	unsigned int mMaxCount;
};

class SP_XmlStringBuffer {
public:
	SP_XmlStringBuffer();
	virtual ~SP_XmlStringBuffer();
	int append( char c );
	int append( const char * value, int size = 0 );
	int getSize() const;
	const char * getBuffer() const;
	char * takeBuffer();
	void clean();

private:
	SP_XmlStringBuffer( SP_XmlStringBuffer & );
	SP_XmlStringBuffer & operator=( SP_XmlStringBuffer & );

	void init();

	char * mBuffer;
	int mMaxSize;
	int mSize;	
};

class SP_XmlStringUtils {
public:
	static int decode(
			const char * encodeValue, SP_XmlStringBuffer * outBuffer );
	static int encode(
			const char * decodeValue, SP_XmlStringBuffer * outBuffer );
	static int isNameChar( char c );

private:
	static const char XML_CHARS [];
	static const char * ESC_CHARS [];

	SP_XmlStringUtils();
};

#ifdef WIN32
int snprintf(char *s,  size_t  n,  const  char  *format,  /*args*/ ...);

#define strncasecmp strnicmp
#define strcasecmp  stricmp
#endif

#endif

