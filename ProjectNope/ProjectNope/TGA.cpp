#include "TGA.h"
#include <memory>
#include <iostream>

TGA::TGA(instream& is, std::set<std::string> options)
{
	if (options.find("!sRGB") != options.end())
		sRGB = false;

	linear = options.find("linear") != options.end();

	size_t file_size;
	is.seekg(0, std::ios::end);
	file_size = is.tellg();
	is.seekg(0, std::ios::beg);

	void * data = malloc(file_size);

	is.read((char*)data, file_size);

	pRawData = (unsigned char*)data;
	w = ((unsigned short*)&pRawData[12])[0];
	h = ((unsigned short*)&pRawData[12])[1];
	FinalData = malloc(w*h*4);
	pFinalData = (unsigned char*)FinalData;
	pData = &pRawData[18+pRawData[0]];
	switch(pRawData[2])
	{
	case 1:
		{
			int reader = 18+pRawData[0]+pRawData[7]*((unsigned short*)&pRawData[5])[0];
			int writer = 0;
			int pbs = pRawData[16]==16 ? 2 : 1;
			int cmes = pRawData[7]==32 ? 4 : 3;

			int img_size = w*h;
			while (writer < img_size) {

				if (pbs==2) {
					unsigned short index = ((unsigned short*)&pRawData[reader])[0];
					pFinalData[writer*4] = pData[index*cmes];
					pFinalData[writer*4+1] = pData[index*cmes+1];
					pFinalData[writer*4+2] = pData[index*cmes+2];
					pFinalData[writer*4+3] = cmes==4 ? pData[index*cmes+3] : 255;
				} else {
					unsigned char index = pRawData[reader+1];
					pFinalData[writer*4] = pData[index*cmes];
					pFinalData[writer*4+1] = pData[index*cmes+1];
					pFinalData[writer*4+2] = pData[index*cmes+2];
					pFinalData[writer*4+3] = cmes==4 ? pData[index*cmes+3] : 255;
				}
				reader += pbs;
			}

			break;
		}
	case 2:
		{
			if (pRawData[16]==32) {
				memcpy(pData, pFinalData, w*h*4);
			} else {
				for (int i=0;i<w*h;++i) {
					pFinalData[i*4] = pData[i*3];
					pFinalData[i*4+1] = pData[i*3+1];
					pFinalData[i*4+2] = pData[i*3+2];
					pFinalData[i*4+3] = 255;
				}
			}
			break;
		}
	case 9:
		{
			ReadRLE();
			break;
		}
	case 10:
		{
			ReadRLE();
			break;
		}
	}
	free(data);

	texid = 0;
}

TGA::~TGA(void)
{
	if (FinalData != nullptr) {
		free((void*)FinalData);
	}
}

void TGA::ReadRLE(void)
{
	int reader = 18+pRawData[0];

	int writer = 0;
	int pbs = pRawData[16]==32 ? 4 : 3;

	int cmes = pRawData[7]==32 ? 4 : 3;

	if (pRawData[2]==9){
		reader += pRawData[7]*((unsigned short*)&pRawData[5])[0];
		pbs = pRawData[16]==16 ? 2 : 1;
	}
	int img_size = w*h;
	while (writer < img_size) {
		if (pRawData[reader]>127) {
			for (int i=0;i<pRawData[reader]-127;++i) {
				switch(pbs)
				{
				case 4:
					{
						pFinalData[(writer+i)*4] = pRawData[(reader)+1];
						pFinalData[(writer+i)*4+1] = pRawData[(reader)+2];
						pFinalData[(writer+i)*4+2] = pRawData[(reader)+3];
						pFinalData[(writer+i)*4+3] = pRawData[(reader)+4];
						break;
					}
				case 3:
					{
						pFinalData[(writer+i)*4] = pRawData[(reader)+1];
						pFinalData[(writer+i)*4+1] = pRawData[(reader)+2];
						pFinalData[(writer+i)*4+2] = pRawData[(reader)+3];
						pFinalData[(writer+i)*4+3] = 255;
						break;
					}
				case 2:
					{
						unsigned short index = ((unsigned short*)&pRawData[reader+1])[0];
						pFinalData[(writer+i)*4] = pData[index*cmes];
						pFinalData[(writer+i)*4+1] = pData[index*cmes+1];
						pFinalData[(writer+i)*4+2] = pData[index*cmes+2];
						pFinalData[(writer+i)*4+3] = cmes==4 ? pData[index*cmes+3] : 255;
						break;
					}
				case 1:
					{
						unsigned char index = pRawData[reader+1];
						pFinalData[(writer+i)*4] = pData[index*cmes];
						pFinalData[(writer+i)*4+1] = pData[index*cmes+1];
						pFinalData[(writer+i)*4+2] = pData[index*cmes+2];
						pFinalData[(writer+i)*4+3] = cmes==4 ? pData[index*cmes+3] : 255;
						break;
					}
				}
			}
			writer += pRawData[reader]-127;
			reader += pbs+1;
		} else {
			for (int i=0;i<pRawData[reader]+1;++i) {
				switch(pbs)
				{
				case 4:
					{
						pFinalData[(writer+i)*4] = pRawData[reader+i*4+1];
						pFinalData[(writer+i)*4+1] = pRawData[reader+i*4+2];
						pFinalData[(writer+i)*4+2] = pRawData[reader+i*4+3];
						pFinalData[(writer+i)*4+3] = pRawData[reader+i*4+4];
						break;
					}
				case 3:
					{
						pFinalData[(writer+i)*4] = pRawData[reader+i*3+1];
						pFinalData[(writer+i)*4+1] = pRawData[reader+i*3+2];
						pFinalData[(writer+i)*4+2] = pRawData[reader+i*3+3];
						pFinalData[(writer+i)*4+3] = 255;
						break;
					}
				case 2:
					{
						unsigned short index = ((unsigned short*)&pRawData[reader+1])[i];
						pFinalData[(writer+i)*4] = pData[index*cmes];
						pFinalData[(writer+i)*4+1] = pData[index*cmes+1];
						pFinalData[(writer+i)*4+2] = pData[index*cmes+2];
						pFinalData[(writer+i)*4+3] = cmes==4 ? pData[index*cmes+3] : 255;
						break;
					}
				case 1:
					{
						unsigned char index = pRawData[reader+i+1];
						pFinalData[(writer+i)*4] = pData[index*cmes];
						pFinalData[(writer+i)*4+1] = pData[index*cmes+1];
						pFinalData[(writer+i)*4+2] = pData[index*cmes+2];
						pFinalData[(writer+i)*4+3] = cmes==4 ? pData[index*cmes+3] : 255;
						break;
					}
				}
			}
			writer += pRawData[reader]+1;
			reader += (pRawData[reader]+1)*pbs+1;
		}
	}
}

GLuint TGA::getGLTexID(void)
{
	if (texid==0)
	{
		glGenTextures(1, &texid);
		glBindTexture(GL_TEXTURE_2D, texid);
		if (fAnisotropicFiltering < 0.5f ? fGeneralAnisotropicFiltering : fAnisotropicFiltering > 1.0f)
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, fAnisotropicFiltering < 0.5f ? fGeneralAnisotropicFiltering : fAnisotropicFiltering);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		if (linear)
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		else
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		if (sRGB && useGammaCorrection)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, FinalData);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, FinalData);
	}
	return texid;
}