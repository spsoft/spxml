/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spdomparser_hpp__
#define __spdomparser_hpp__

class SP_XmlNode;
class SP_XmlDocument;
class SP_XmlElementNode;
class SP_XmlDocDeclNode;
class SP_XmlDocTypeNode;
class SP_XmlPullParser;
class SP_XmlStringBuffer;

/// parse string to xml node tree
class SP_XmlDomParser {
public:
	SP_XmlDomParser();
	~SP_XmlDomParser();

	/// append more input xml source
	void append( const char * source, int len );

	/// @return NOT NULL : the detail error message
	/// @return NULL : no error
	const char * getError();
	
	/// get the parse result
	const SP_XmlDocument * getDocument() const;

private:
	SP_XmlDomParser( SP_XmlDomParser & );
	SP_XmlDomParser & operator=( SP_XmlDomParser & );

	SP_XmlPullParser * mParser;
	SP_XmlDocument * mDocument;
	SP_XmlElementNode * mCurrent;
};

/// serialize xml node tree to string
class SP_XmlDomBuffer {
public:
	SP_XmlDomBuffer( const SP_XmlNode * node );
	~SP_XmlDomBuffer();

	const char * getBuffer() const;
	int getSize() const;

private:
	SP_XmlDomBuffer( SP_XmlDomBuffer & );
	SP_XmlDomBuffer & operator=( SP_XmlDomBuffer & );

	static void dumpDocDecl( const SP_XmlDocDeclNode * docDecl,
			SP_XmlStringBuffer * buffer );
	static void dumpDocType( const SP_XmlDocTypeNode * docType,
			SP_XmlStringBuffer * buffer );
	static void dump( const SP_XmlNode * node,
			SP_XmlStringBuffer * buffer, int level );
	static void dumpElement( const SP_XmlNode * node,
			SP_XmlStringBuffer * buffer, int level );

	SP_XmlStringBuffer * mBuffer;
};

#endif

