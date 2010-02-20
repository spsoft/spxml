/*
 * Copyright 2010 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spxmlrpc.hpp"

#include "spxmlnode.hpp"
#include "spdomparser.hpp"
#include "spxmlhandle.hpp"
#include "spxmlcodec.hpp"
#include "spxmlutils.hpp"

SP_XmlRpcReqObject :: SP_XmlRpcReqObject( const char * buffer, int len )
{
	mPacketError = NULL;

	mParser = new SP_XmlDomParser();
	mParser->append( buffer, len );

	SP_XmlHandle handle( mParser->getDocument()->getRootElement() );

	mVersion = NULL;

	mMethod = handle.getChild( "methodName" ).getChild( 0 ).toCData();

	mParams = handle.getChild( "params" ).getChild( "param" )
			.getChild( "value" ).getChild( 0 ).toElement();

	if( NULL == mMethod ) {
		mPacketError = "invalid xml stream, not methodName element";
	} else {
		if( NULL == mParams ) {
			mPacketError = "invalid xml stream, not params element";
		}
	}

	mID = NULL;
}

SP_XmlRpcReqObject :: ~SP_XmlRpcReqObject()
{
	delete mParser, mParser = NULL;
}

const char * SP_XmlRpcReqObject :: getVersion() const
{
	return NULL == mVersion ? "" : mVersion->getText();
}

const char * SP_XmlRpcReqObject :: getMethod() const
{
	return NULL == mMethod ? "" : mMethod->getText();
}

SP_XmlElementNode * SP_XmlRpcReqObject :: getParams() const
{
	return mParams;
}

const char * SP_XmlRpcReqObject :: getStrID() const
{
	return NULL == mID ? "" : mID->getText();
}

const char * SP_XmlRpcReqObject :: getPacketError() const
{
	return mPacketError;
}

//============================================================================

SP_XmlRpcRespObject :: SP_XmlRpcRespObject( const char * buffer, int len )
{
	mPacketError = NULL;

	mParser = new SP_XmlDomParser();
	mParser->append( buffer, len );

	SP_XmlHandle handle( mParser->getDocument()->getRootElement() );

	mVersion = NULL;
	mID = NULL;

	mResult = handle.getChild( "params" ).getChild( "param" )
			.getChild( "value" ).getChild( 0 ).toElement();

	mError = handle.getChild( "fault" ).getChild( "value" ).getChild( 0 ).toElement();

	if( NULL == mResult && NULL == mError ) {
		mPacketError = "invalid xml stream, not result element";
	}
}

SP_XmlRpcRespObject :: ~SP_XmlRpcRespObject()
{
	delete mParser, mParser = NULL;
}

const char * SP_XmlRpcRespObject :: getVersion() const
{
	return NULL == mVersion ? "" : mVersion->getText();
}

SP_XmlElementNode * SP_XmlRpcRespObject :: getResult() const
{
	return mResult;
}

const char * SP_XmlRpcRespObject :: getStrID() const
{
	return NULL == mID ? "" : mID->getText();
}

SP_XmlElementNode * SP_XmlRpcRespObject :: getError() const
{
	return mError;
}

int SP_XmlRpcRespObject :: getErrorCode() const
{
	SP_XmlHandle handle( mError );

	SP_XmlCDataNode * node = NULL;

	for( int i = 0; i < 2; i++ ) {
		SP_XmlCDataNode * name = handle.getChild( "member", i ).getChild( "name" )
				.getChild( 0 ).toCData();

		if( NULL == name ) break;

		if( 0 == strcmp( "faultCode", name->getText() ) ) {
			node = handle.getChild( "member", i ).getChild( "value" )
					.getChild( "int" ).getChild( 0 ).toCData();
		}
	}

	return NULL != node ? atoi( node->getText() ) : 0;
}

const char * SP_XmlRpcRespObject :: getErrorMsg() const
{
	SP_XmlHandle handle( mError );

	SP_XmlCDataNode * node = NULL;

	for( int i = 0; i < 2; i++ ) {
		SP_XmlCDataNode * name = handle.getChild( "member", i ).getChild( "name" )
				.getChild( 0 ).toCData();

		if( NULL == name ) break;

		if( 0 == strcmp( "faultString", name->getText() ) ) {
			node = handle.getChild( "member", i ).getChild( "value" )
					.getChild( "string" ).getChild( 0 ).toCData();
		}
	}

	return NULL != node ? node->getText() : NULL;
}

const char * SP_XmlRpcRespObject :: getPacketError() const
{
	return mPacketError;
}

//============================================================================

int SP_XmlRpcUtils :: toReqBuffer( const char * method, const char * id,
		const SP_XmlElementNode * params, SP_XmlStringBuffer * buffer )
{
	buffer->append( "<?xml version=\"1.0\"?>" );
	buffer->append( "<methodCall>" );

	buffer->append( "<methodName>" );
	SP_XmlStringCodec::encode( "utf-8", method, buffer );
	buffer->append( "</methodName>" );

	buffer->append( "<params><param><value>" );

	SP_XmlDomBuffer::dump( "utf-8", params, buffer, -1 );

	buffer->append( "</value></param></params>" );

	buffer->append( "</methodCall>" );

	return 0;
}

int SP_XmlRpcUtils :: toRespBuffer( const char * id, const SP_XmlElementNode * result,
		const SP_XmlElementNode * error, SP_XmlStringBuffer * buffer )
{
	buffer->append( "<?xml version=\"1.0\"?>" );
	buffer->append( "<methodResponse>" );

	if( NULL != error ) {
		buffer->append( "<fault>" );
		buffer->append( "<value>" );

		SP_XmlDomBuffer::dump( "utf-8", error, buffer, -1 );

		buffer->append( "</value>" );
		buffer->append( "</fault>" );
	} else {
		buffer->append( "<params><param><value>" );

		SP_XmlDomBuffer::dump( "utf-8", result, buffer, -1 );

		buffer->append( "</value></param></params>" );
	}

	buffer->append( "</methodResponse>" );

	return 0;
}

int SP_XmlRpcUtils :: setError( SP_XmlElementNode * error, int code, const char * msg )
{
	char strCode[ 32 ] = { 0 };
	snprintf( strCode, sizeof( strCode ), "%d", code );

	// faultCode
	{
		SP_XmlElementNode * mcode = new SP_XmlElementNode();
		mcode->setName( "member" );

		SP_XmlElementNode * name = new SP_XmlElementNode();
		name->setName( "name" );

		SP_XmlCDataNode * cname = new SP_XmlCDataNode();
		cname->setText( "faultCode" );

		name->addChild( cname );

		mcode->addChild( name );

		SP_XmlElementNode * value = new SP_XmlElementNode();
		value->setName( "value" );

		SP_XmlElementNode * intNode = new SP_XmlElementNode();
		intNode->setName( "int" );

		SP_XmlCDataNode * cvalue = new SP_XmlCDataNode();
		cvalue->setText( strCode );

		intNode->addChild( cvalue );

		value->addChild( intNode );

		mcode->addChild( value );

		error->addChild( mcode );
	}

	// faultString
	{
		SP_XmlElementNode * mcode = new SP_XmlElementNode();
		mcode->setName( "member" );

		SP_XmlElementNode * name = new SP_XmlElementNode();
		name->setName( "name" );

		SP_XmlCDataNode * cname = new SP_XmlCDataNode();
		cname->setText( "faultString" );

		name->addChild( cname );

		mcode->addChild( name );

		SP_XmlElementNode * value = new SP_XmlElementNode();
		value->setName( "value" );

		SP_XmlElementNode * strNode = new SP_XmlElementNode();
		strNode->setName( "string" );

		SP_XmlCDataNode * cvalue = new SP_XmlCDataNode();
		cvalue->setText( msg );

		strNode->addChild( cvalue );

		value->addChild( strNode );

		mcode->addChild( value );

		error->addChild( mcode );
	}

	return 0;
}

