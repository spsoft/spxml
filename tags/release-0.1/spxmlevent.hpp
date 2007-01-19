/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spxmlevent_hpp__
#define __spxmlevent_hpp__

class SP_XmlArrayList;

class SP_XmlPullEvent {
public:
	enum EventType { eStartDocument, eEndDocument,
			eDocDecl, eDocType, eStartTag, eEndTag, eCData, eComment };

	SP_XmlPullEvent( int eventType );
	virtual ~SP_XmlPullEvent();

	int getEventType();

private:
	/// Private copy constructor and copy assignment ensure classes derived from
	/// this cannot be copied.
	SP_XmlPullEvent( SP_XmlPullEvent & );
	SP_XmlPullEvent & operator=( SP_XmlPullEvent & );

protected:
	const int mEventType;
};

class SP_XmlPullEventList {
public:
	SP_XmlPullEventList();
	~SP_XmlPullEventList();

	int getCount() const;
	void append( SP_XmlPullEvent * event );
	const SP_XmlPullEvent * get( int index ) const;
	SP_XmlPullEvent * take( int index );

private:
	SP_XmlPullEventList( SP_XmlPullEventList & );
	SP_XmlPullEventList & operator=( SP_XmlPullEventList & );

	SP_XmlArrayList * mList;
};

class SP_XmlStartDocEvent : public SP_XmlPullEvent {
public:
	SP_XmlStartDocEvent();
	virtual ~SP_XmlStartDocEvent();
};

class SP_XmlEndDocEvent : public SP_XmlPullEvent {
public:
	SP_XmlEndDocEvent();
	virtual ~SP_XmlEndDocEvent();
};

class SP_XmlDocTypeEvent : public SP_XmlPullEvent {
public:
	SP_XmlDocTypeEvent();
	virtual ~SP_XmlDocTypeEvent();

	void setName( const char * name );
	const char * getName() const;
	void setSystemID( const char * systemID );
	const char * getSystemID() const;
	void setPublicID( const char * publicID );
	const char * getPublicID() const;
	void setDTD( const char * dtd );
	const char * getDTD() const;

private:
	char mName[ 128 ];
	char mSystemID[ 128 ];
	char mPublicID[ 128 ];
	char mDTD[ 256 ];	
};

class SP_XmlDocDeclEvent : public SP_XmlPullEvent {
public:
	SP_XmlDocDeclEvent();
	virtual ~SP_XmlDocDeclEvent();

	void setVersion( const char * version );
	const char * getVersion() const;
	void setEncoding( const char * encoding );
	const char * getEncoding() const;
	void setStandalone( int standalone );
	int getStandalone() const;

private:
	char mVersion[ 8 ];
	char mEncoding[ 32 ];
	int mStandalone;
};

class SP_XmlStartTagEvent : public SP_XmlPullEvent {
public:
	SP_XmlStartTagEvent();
	virtual ~SP_XmlStartTagEvent();

	void setName( const char * name );
	const char * getName() const;

	void addAttr( const char * name, const char * value );
	const char * getAttrValue( const char * name ) const;
	int getAttrCount() const;

	/// get attribute name and value by index, return attribute name
	const char * getAttr( int index, const char ** value ) const;

private:
	char * mName;
	SP_XmlArrayList * mAttrNameList;
	SP_XmlArrayList * mAttrValueList;
};

class SP_XmlTextEvent : public SP_XmlPullEvent {
public:
	SP_XmlTextEvent( int eventType );
	virtual ~SP_XmlTextEvent();

	void setText( const char * text, int len );
	const char * getText() const;

private:
	char * mText;
};

class SP_XmlEndTagEvent : public SP_XmlTextEvent {
public:
	SP_XmlEndTagEvent();
	virtual ~SP_XmlEndTagEvent();
};

class SP_XmlCDataEvent : public SP_XmlTextEvent {
public:
	SP_XmlCDataEvent();
	virtual ~SP_XmlCDataEvent();
};

class SP_XmlCommentEvent : public SP_XmlTextEvent {
public:
	SP_XmlCommentEvent();
	virtual ~SP_XmlCommentEvent();
};

#endif

