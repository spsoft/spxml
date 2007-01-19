/*
 * Copyright 2007 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <assert.h>

#include "spdomparser.hpp"
#include "spxmlparser.hpp"
#include "spxmlevent.hpp"
#include "spxmlutils.hpp"
#include "spxmlnode.hpp"

//=========================================================

SP_XmlDomParser :: SP_XmlDomParser()
{
	mParser = new SP_XmlPullParser();
	mDocument = new SP_XmlDocument();
	mCurrent = NULL;
}

SP_XmlDomParser :: ~SP_XmlDomParser()
{
	if( NULL != mDocument ) delete mDocument;
	mDocument = NULL;

	if( NULL != mParser ) delete mParser;
	mParser = NULL;
}

void SP_XmlDomParser :: append( const char * source, int len )
{
	for( int pos = 0; pos < len; pos += 64 ) {
		int realLen = ( len - pos ) > 64 ? 64 : ( len - pos );
		mParser->append( source + pos, realLen );
		buildTree();
	}
}

void SP_XmlDomParser :: buildTree()
{
	for( SP_XmlPullEvent * event = mParser->getNext();
			NULL != event; event = mParser->getNext() ) {

		switch( event->getEventType() ) {
			case SP_XmlPullEvent::eStartDocument:
				// ignore
				delete event;
				break;
			case SP_XmlPullEvent::eEndDocument:
				// ignore
				delete event;
				break;
			case SP_XmlPullEvent::eDocDecl:
				{
					mDocument->setDocDecl(
							new SP_XmlDocDeclNode( (SP_XmlDocDeclEvent*)event ) );
					break;
				}
			case SP_XmlPullEvent::eDocType:
				{
					mDocument->setDocType(
							new SP_XmlDocTypeNode( (SP_XmlDocTypeEvent*)event ) );
					break;
				}
			case SP_XmlPullEvent::eStartTag:
				{
					SP_XmlElementNode * element =
							new SP_XmlElementNode( (SP_XmlStartTagEvent*)event );
					if( NULL == mCurrent ) {
						mCurrent = element;
						mDocument->setRootElement( element );
					} else {
						mCurrent->addChild( element );
						mCurrent = element;
					}
					break;
				}
			case SP_XmlPullEvent::eEndTag:
				{
					SP_XmlNode * parent = (SP_XmlNode*)mCurrent->getParent();
					if( NULL != parent && SP_XmlNode::eELEMENT == parent->getType() ) {
						mCurrent = static_cast<SP_XmlElementNode*>((SP_XmlNode*)parent);
					} else {
						mCurrent = NULL;
					}

					delete event;
					break;
				}
			case SP_XmlPullEvent::eCData:
				{
					if( NULL != mCurrent ) {
						mCurrent->addChild( new SP_XmlCDataNode( (SP_XmlCDataEvent*)event ) );
					} else {
						delete event;
					}
					break;
				}
			case SP_XmlPullEvent::eComment:
				{
					if( NULL != mCurrent ) {
						mCurrent->addChild( new SP_XmlCommentNode( (SP_XmlCommentEvent*)event ) );
					} else {
						delete event;
					}
					break;
				}
			default:
				{
					assert( 0 );
					break;
				}
		}
	}
}

const char * SP_XmlDomParser :: getError()
{
	return mParser->getError();
}

const SP_XmlDocument * SP_XmlDomParser :: getDocument() const
{
	return mDocument;
}

//=========================================================

SP_XmlDomBuffer :: SP_XmlDomBuffer( const SP_XmlNode * node )
{
	mBuffer = new SP_XmlStringBuffer();
	dump( node, mBuffer, 0 );
}

SP_XmlDomBuffer :: ~SP_XmlDomBuffer()
{
	if( NULL != mBuffer ) delete mBuffer;
	mBuffer = NULL;
}

const char * SP_XmlDomBuffer :: getBuffer() const
{
	return mBuffer->getBuffer();
}

int SP_XmlDomBuffer :: getSize() const
{
	return mBuffer->getSize();
}

void SP_XmlDomBuffer :: dump(
		const SP_XmlNode * node, SP_XmlStringBuffer * buffer, int level )
{
	if( SP_XmlNode::eXMLDOC == node->getType() ) {
		SP_XmlDocument * document = static_cast<SP_XmlDocument*>((SP_XmlNode*)node);
		dumpDocDecl( document->getDocDecl(), buffer );
		dumpDocType( document->getDocType(), buffer );
		dumpElement( document->getRootElement(), buffer, level );
	} else if( SP_XmlNode::eCDATA == node->getType() ) {
		SP_XmlCDataNode * cdata = static_cast<SP_XmlCDataNode*>((SP_XmlNode*)node);
		SP_XmlStringUtils::encode( cdata->getText(), buffer );
	} else if( SP_XmlNode::eCOMMENT == node->getType() ) {
		SP_XmlCommentNode * comment = static_cast<SP_XmlCommentNode*>((SP_XmlNode*)node);
		buffer->append( '\n' );
		for( int i = 0; i < level; i++ ) buffer->append( '\t' );
		buffer->append( "<!--" );
		buffer->append( comment->getText() );
		buffer->append( "-->\n" );
	} else if( SP_XmlNode::eELEMENT == node->getType() ) {
		dumpElement( node, buffer, level );
	} else if( SP_XmlNode::eDOCDECL == node->getType() ) {
		dumpDocDecl( (SP_XmlDocDeclNode*)node, buffer );
	} else if( SP_XmlNode::eDOCTYPE == node->getType() ) {
		dumpDocType( (SP_XmlDocTypeNode*)node, buffer );
	} else {
		// ignore
	}
}

void SP_XmlDomBuffer :: dumpDocDecl( const SP_XmlDocDeclNode * docDecl,
		SP_XmlStringBuffer * buffer )
{
	if( NULL == docDecl ) return;

	buffer->append( "<?xml version=\"" );
	if( '\0' != * ( docDecl->getVersion() ) ) {
		buffer->append( docDecl->getVersion() );
	} else {
		buffer->append( "1.0" );
	}
	buffer->append( "\" " );

	if( '\0' != * ( docDecl->getEncoding() ) ) {
		buffer->append( "encoding=\"" );
		buffer->append( docDecl->getEncoding() );
		buffer->append( "\" " );
	}

	if( -1 != docDecl->getStandalone() ) {
		char standalone[ 32 ];
		snprintf( standalone, sizeof( standalone ), "standalone=\"%s\" ",
				0 == docDecl->getStandalone() ? "no" : "yes" );
		buffer->append( standalone );
	}

	buffer->append( "?>\n" );
}

void SP_XmlDomBuffer :: dumpDocType( const SP_XmlDocTypeNode * docType,
		SP_XmlStringBuffer * buffer )
{
	if( NULL == docType ) return;

	buffer->append( "<!DOCTYPE " );
	buffer->append( docType->getName() );

	if( '\0' != * ( docType->getPublicID() ) ) {
		buffer->append( " PUBLIC " );
		buffer->append( '"' );
		buffer->append( docType->getPublicID() );
		buffer->append( '"' );
	}

	if( '\0' != * ( docType->getSystemID() ) ) {
		buffer->append( " SYSTEM " );
		buffer->append( '"' );
		buffer->append( docType->getSystemID() );
		buffer->append( '"' );
	}

	if( '\0' != * ( docType->getDTD() ) ) {
		buffer->append( " \"" );
		buffer->append( docType->getDTD() );
		buffer->append( '"' );
	}

	buffer->append( ">\n" );
}

void SP_XmlDomBuffer :: dumpElement(
		const SP_XmlNode * node, SP_XmlStringBuffer * buffer, int level )
{
	if( SP_XmlNode::eELEMENT == node->getType() ) {
		for( int i = 0; i < level; i++ ) buffer->append( '\t' );

		SP_XmlElementNode * element = static_cast<SP_XmlElementNode*>((SP_XmlNode*)node);
		buffer->append( "<" );
		buffer->append( element->getName() );

		const char * name = NULL, * value = NULL;
		for( int i = 0; i < element->getAttrCount(); i++ ) {
			name = element->getAttr( i, &value );
			if( NULL != name && NULL != value ) {
				buffer->append( ' ' );
				buffer->append( name );
				buffer->append( "=\"" );
				SP_XmlStringUtils::encode( value, buffer );
				buffer->append( "\"" );
			}
		}

		const SP_XmlNodeList * children = element->getChildren();

		if( children->getLength() > 0 ) {
			if( SP_XmlNode::eCDATA != children->get( 0 )->getType() ) {
				buffer->append( ">\n" );
			} else {
				buffer->append( ">" );
			}

			for( int j = 0; j < children->getLength(); j++ ) {
				dump( children->get( j ), buffer, level + 1 );
			}

			if( SP_XmlNode::eCDATA != children->get( 0 )->getType() ) {
				for( int i = 0; i < level; i++ ) buffer->append( '\t' );
			}
			buffer->append( "</" );
			buffer->append( element->getName() );
			buffer->append( ">\n" );
		} else {
			buffer->append( "/>\n" );
		}
	} else {
		dump( node, buffer, level );
	}
}

