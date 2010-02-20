/*
 * Copyright 2010 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#include <string.h>

#include "spxmlrpc.hpp"
#include "spxmlutils.hpp"
#include "spxmlnode.hpp"
#include "spdomparser.hpp"

void testReq()
{
	const char * req = "<?xml version=\"1.0\"?>"
			"<methodCall>"
			"<methodName>examples.getStateName</methodName>"
			"<params><param><value><i4>41</i4></value></param></params>"
			"</methodCall>";

	SP_XmlRpcReqObject reqObject( req, strlen( req ) );

	SP_XmlStringBuffer buffer;

	SP_XmlRpcUtils::toReqBuffer( "examples.getStateName", "1", reqObject.getParams(), &buffer );

	printf( "req: %s\n", buffer.getBuffer() );
}

void testResp()
{
	const char * resp = "<?xml version=\"1.0\"?>"
			"<methodResponse>"
			"<params><param><value><string>South Dakota</string></value></param></params>"
			"</methodResponse>";

	SP_XmlRpcRespObject respObject( resp, strlen( resp ) );

	SP_XmlStringBuffer buffer;

	SP_XmlRpcUtils::toRespBuffer( respObject.getStrID(), respObject.getResult(),
			respObject.getError(), &buffer );

	printf( "resp: %s\n", buffer.getBuffer() );
}

void testError()
{
	SP_XmlElementNode error;
	error.setName( "struct" );

	SP_XmlRpcUtils::setError( &error, 4, "Too many parameters." );

	SP_XmlStringBuffer buffer;

	SP_XmlRpcUtils::toRespBuffer( "", NULL, &error, &buffer );

	printf( "error: %s\n", buffer.getBuffer() );

	SP_XmlRpcRespObject respObject( buffer.getBuffer(), buffer.getSize() );

	printf( "code %d, msg %s\n", respObject.getErrorCode(), respObject.getErrorMsg() );
}

int main( int argc, char * argv[] )
{
	testReq();

	testResp();

	testError();

	return 0;
}

