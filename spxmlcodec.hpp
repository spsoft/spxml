
#ifndef __spxmlcodec_hpp__
#define __spxmlcodec_hpp__

class SP_XmlStringBuffer;

class SP_XmlStringCodec {
public:
	static int decode(
			const char * encodeValue, SP_XmlStringBuffer * outBuffer );
	static int encode(
			const char * decodeValue, SP_XmlStringBuffer * outBuffer );
	static int isNameChar( char c );

private:
	static const char XML_CHARS [];
	static const char * ESC_CHARS [];

	SP_XmlStringCodec();
};

#endif

