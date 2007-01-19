/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __xmlparser_hpp__
#define __xmlparser_hpp__

class SP_XmlPullEvent;
class SP_XmlPullEventList;
class SP_XmlReader;
class SP_XmlReaderPool;
class SP_XmlArrayList;

class SP_XmlPullParser {
public:
	SP_XmlPullParser();
	~SP_XmlPullParser();

	/// append more input xml source
	void append( const char * source, int len );

	/// @return NOT NULL : the pull event
	/// @return NULL : error or need more input
	SP_XmlPullEvent * getNext();	

	/// @return NOT NULL : the detail error message
	/// @return NULL : no error
	const char * getError();

	int getLevel();

protected:
	void changeReader( SP_XmlReader * reader );

	SP_XmlReader * getReader( int type );

	void setError( const char * error );

	friend class SP_XmlReader;

private:
	SP_XmlPullEventList * mEventList;
	SP_XmlReader * mReader;
	SP_XmlReaderPool * mReaderPool;
	SP_XmlArrayList * mTagNameStack;

	enum { eRootNone, eRootStart, eRootEnd };
	int mRootTagState;

	int mLevel;

	char * mError;

	char mErrorSegment[ 32 ];
	int mErrorIndex;
	int mColIndex, mRowIndex;
};

#endif

