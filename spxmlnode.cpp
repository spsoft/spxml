/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <string.h>
#include <stdlib.h>

#include "spxmlnode.hpp"
#include "spxmlutils.hpp"
#include "spxmlevent.hpp"

//=========================================================

SP_XmlNode :: SP_XmlNode( int type )
	: mType( type )
{
	mParent = NULL;
}

SP_XmlNode :: ~SP_XmlNode()
{
	mParent = NULL;
}

void SP_XmlNode :: setParent( SP_XmlNode * parent )
{
	mParent = parent;
}

const SP_XmlNode * SP_XmlNode :: getParent() const
{
	return mParent;
}

int SP_XmlNode :: getType() const
{
	return mType;
}

//=========================================================

SP_XmlNodeList :: SP_XmlNodeList()
{
	mList = new SP_XmlArrayList();
}

SP_XmlNodeList :: ~SP_XmlNodeList()
{
	for( int i = 0; i < mList->getCount(); i++ ) {
		SP_XmlNode * node = NULL;
		memcpy( &node, mList->getItem( i ), sizeof( void * ) );
		delete node;
	}

	delete mList;

	mList = NULL;
}

int SP_XmlNodeList :: getLength() const
{
	return mList->getCount();
}

void SP_XmlNodeList :: append( SP_XmlNode * node )
{
	mList->append( &node, sizeof( void * ) );
}

const SP_XmlNode * SP_XmlNodeList :: get( int index ) const
{
	SP_XmlNode * node = NULL;

	if( NULL != mList->getItem( index ) ) {
		memcpy( &node, mList->getItem( index ), sizeof( void * ) );
	}

	return node;
}

const SP_XmlNode * SP_XmlNodeList :: take( int index ) const
{
	SP_XmlNode * node = NULL;

	void * item = mList->takeItem( index );
	if( NULL != item ) {
		memcpy( &node, item, sizeof( void * ) );
		free( item );
	}

	return node;
}

//=========================================================

SP_XmlDocument :: SP_XmlDocument()
	: SP_XmlNode( eXMLDOC )
{
	mDocDecl = NULL;
	mDocType = NULL;
	mRootElement = NULL;
}

SP_XmlDocument :: ~SP_XmlDocument()
{
	if( NULL != mDocDecl ) delete mDocDecl;
	mDocDecl = NULL;

	if( NULL != mDocType ) delete mDocType;
	mDocType = NULL;

	if( NULL != mRootElement ) delete mRootElement;
	mRootElement = NULL;
}

void SP_XmlDocument :: setDocDecl( SP_XmlDocDeclNode * docDecl )
{
	if( NULL != mDocDecl ) delete mDocDecl;
	docDecl->setParent( this );
	mDocDecl = docDecl;
}

SP_XmlDocDeclNode * SP_XmlDocument :: getDocDecl() const
{
	return mDocDecl;
}

void SP_XmlDocument :: setDocType( SP_XmlDocTypeNode * docType )
{
	if( NULL != mDocType ) delete mDocType;
	docType->setParent( this );
	mDocType = docType;
}

SP_XmlDocTypeNode * SP_XmlDocument :: getDocType() const
{
	return mDocType;
}

void SP_XmlDocument :: setRootElement( SP_XmlElementNode * rootElement )
{
	if( NULL != mRootElement ) delete mRootElement;
	rootElement->setParent( this );
	mRootElement = rootElement;
}

SP_XmlElementNode * SP_XmlDocument :: getRootElement() const
{
	return mRootElement;
}

//=========================================================

SP_XmlDocDeclNode :: SP_XmlDocDeclNode()
	: SP_XmlNode( eDOCDECL )
{
	mEvent = new SP_XmlDocDeclEvent();
}

SP_XmlDocDeclNode :: SP_XmlDocDeclNode( SP_XmlDocDeclEvent * event )
	: SP_XmlNode( eDOCDECL )
{
	mEvent = event;
}

SP_XmlDocDeclNode :: ~SP_XmlDocDeclNode()
{
	if( NULL != mEvent ) delete mEvent;
	mEvent = NULL;
}

void SP_XmlDocDeclNode :: setVersion( const char * version )
{
	mEvent->setVersion( version );
}

const char * SP_XmlDocDeclNode :: getVersion() const
{
	return mEvent->getVersion();
}

void SP_XmlDocDeclNode :: setEncoding( const char * encoding )
{
	mEvent->setEncoding( encoding );
}

const char * SP_XmlDocDeclNode :: getEncoding() const
{
	return mEvent->getEncoding();
}

void SP_XmlDocDeclNode :: setStandalone( int standalone )
{
	mEvent->setStandalone( standalone );
}

int SP_XmlDocDeclNode :: getStandalone() const
{
	return mEvent->getStandalone();
}

//=========================================================

SP_XmlDocTypeNode :: SP_XmlDocTypeNode()
	: SP_XmlNode( eDOCTYPE )
{
	mEvent = new SP_XmlDocTypeEvent();
}

SP_XmlDocTypeNode :: SP_XmlDocTypeNode( SP_XmlDocTypeEvent * event )
	: SP_XmlNode( eDOCTYPE )
{
	mEvent = event;
}

SP_XmlDocTypeNode :: ~SP_XmlDocTypeNode()
{
	if( NULL != mEvent ) delete mEvent;
	mEvent = NULL;
}

void SP_XmlDocTypeNode :: setName( const char * name )
{
	mEvent->setName( name );
}

const char * SP_XmlDocTypeNode :: getName() const
{
	return mEvent->getName();
}

void SP_XmlDocTypeNode :: setSystemID( const char * systemID )
{
	mEvent->setSystemID( systemID );
}

const char * SP_XmlDocTypeNode :: getSystemID() const
{
	return mEvent->getSystemID();
}

void SP_XmlDocTypeNode :: setPublicID( const char * publicID )
{
	mEvent->setPublicID( publicID );
}

const char * SP_XmlDocTypeNode :: getPublicID() const
{
	return mEvent->getPublicID();
}

void SP_XmlDocTypeNode :: setDTD( const char * dtd )
{
	mEvent->setDTD( dtd );
}

const char * SP_XmlDocTypeNode :: getDTD() const
{
	return mEvent->getDTD();
}

//=========================================================

SP_XmlElementNode :: SP_XmlElementNode()
	: SP_XmlNode( eELEMENT )
{
	mEvent = new SP_XmlStartTagEvent();
	mChildren = new SP_XmlNodeList();
}

SP_XmlElementNode :: SP_XmlElementNode( SP_XmlStartTagEvent * event )
	: SP_XmlNode( eELEMENT )
{
	mEvent = event;
	mChildren = new SP_XmlNodeList();
}

SP_XmlElementNode :: ~SP_XmlElementNode()
{
	if( NULL != mEvent ) delete mEvent;
	mEvent = NULL;

	if( NULL != mChildren ) delete mChildren;
	mChildren = NULL;
}

void SP_XmlElementNode :: setName( const char * name )
{
	mEvent->setName( name );
}

const char * SP_XmlElementNode :: getName() const
{
	return mEvent->getName();
}

void SP_XmlElementNode :: addChild( SP_XmlNode * node )
{
	node->setParent( this );
	mChildren->append( node );
}

const SP_XmlNodeList * SP_XmlElementNode :: getChildren() const
{
	return mChildren;
}

void SP_XmlElementNode :: addAttr( const char * name, const char * value )
{
	mEvent->addAttr( name, value );
}

const char * SP_XmlElementNode :: getAttrValue( const char * name ) const
{
	return mEvent->getAttrValue( name );
}

int SP_XmlElementNode :: getAttrCount() const
{
	return mEvent->getAttrCount();
}

const char * SP_XmlElementNode :: getAttr( int index, const char ** value ) const
{
	return mEvent->getAttr( index, value );
}

//=========================================================

SP_XmlCDataNode :: SP_XmlCDataNode()
	: SP_XmlNode( eCDATA )
{
	mEvent = new SP_XmlCDataEvent();
}

SP_XmlCDataNode :: SP_XmlCDataNode( SP_XmlCDataEvent * event )
	: SP_XmlNode( eCDATA )
{
	mEvent = event;
}

SP_XmlCDataNode :: ~SP_XmlCDataNode()
{
	if( NULL != mEvent ) delete mEvent;
	mEvent = NULL;
}

void SP_XmlCDataNode :: setText( const char * content )
{
	mEvent->setText( content, strlen( content ) );
}

const char * SP_XmlCDataNode :: getText() const
{
	return mEvent->getText();
}

//=========================================================

SP_XmlCommentNode :: SP_XmlCommentNode()
	: SP_XmlNode( eCOMMENT )
{
	mEvent = new SP_XmlCommentEvent();
}

SP_XmlCommentNode :: SP_XmlCommentNode( SP_XmlCommentEvent * event )
	: SP_XmlNode( eCOMMENT )
{
	mEvent = event;
}

SP_XmlCommentNode :: ~SP_XmlCommentNode()
{
	if( NULL != mEvent ) delete mEvent;
	mEvent = NULL;
}

void SP_XmlCommentNode :: setText( const char * comment )
{
	mEvent->setText( comment, strlen( comment ) );
}

const char * SP_XmlCommentNode :: getText() const
{
	return mEvent->getText();
}
