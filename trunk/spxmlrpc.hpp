/*
 * Copyright 2010 Stephen Liu
 * For license terms, see the file COPYING along with this library.
 */

#ifndef __spxmlrpc_hpp__
#define __spxmlrpc_hpp__

class SP_XmlElementNode;
class SP_XmlCDataNode;
class SP_XmlDomParser;

class SP_XmlStringBuffer;

class SP_XmlRpcReqObject {
public:
	SP_XmlRpcReqObject( const char * buffer, int len );
	~SP_XmlRpcReqObject();

	const char * getVersion() const;
	const char * getMethod() const;
	SP_XmlElementNode * getParams() const;

	const char * getStrID() const;

	const char * getPacketError() const;

private:
	SP_XmlDomParser * mParser;

	SP_XmlCDataNode * mVersion;
	SP_XmlElementNode * mParams;
	SP_XmlCDataNode * mMethod;
	SP_XmlCDataNode * mID;

	const char * mPacketError;
};

class SP_XmlRpcRespObject {
public:
	SP_XmlRpcRespObject( const char * buffer, int len );
	~SP_XmlRpcRespObject();

	const char * getVersion() const;
	SP_XmlElementNode * getResult() const;

	const char * getStrID() const;

	SP_XmlElementNode * getError() const;

	int getErrorCode() const;

	const char * getErrorMsg() const;

	const char * getPacketError() const;

private:
	SP_XmlDomParser * mParser;

	SP_XmlCDataNode * mVersion;
	SP_XmlElementNode * mResult;
	SP_XmlElementNode * mError;
	SP_XmlCDataNode * mID;

	const char * mPacketError;
};

class SP_XmlRpcUtils {
public:

	enum {
		eParseError     = -32700,
		eInvalidRequest = -32600,
		eMethodNoFound  = -32601,
		eInvalidParams  = -32602,
		eInternalError  = -32603
	};

public:
	static int toReqBuffer( const char * method, const char * id,
			const SP_XmlElementNode * params, SP_XmlStringBuffer * buffer );

	static int toRespBuffer( const char * id, const SP_XmlElementNode * result,
			const SP_XmlElementNode * error, SP_XmlStringBuffer * buffer );

	static int setError( SP_XmlElementNode * error, int code, const char * msg );

private:
	SP_XmlRpcUtils();
};

#endif

