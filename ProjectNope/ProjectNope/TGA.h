#ifndef TGA_H
#define TGA_H

#include "Texture.h"

class TGA :
	public Texture
{
public:
	TGA(instream& is, std::set<std::string> options);
	~TGA(void);

	GLuint getGLTexID(void);
private:
	unsigned char * pRawData;
	unsigned char * pData;
	unsigned char * pFinalData;

	bool linear;

	void ReadRLE(void);
};

#endif
