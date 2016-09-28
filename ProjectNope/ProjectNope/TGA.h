#include "Texture.h"

#ifndef TGA_H
#define TGA_H

class TGA
	: public Texture
{
public:
	TGA(instream& is);
	~TGA(void);

	GLuint getGLTexID(void);
private:
	unsigned char * pRawData;
	unsigned char * pData;
	unsigned char * pFinalData;

	void ReadRLE(void);
};

#endif
