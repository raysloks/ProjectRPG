#include "RenderContext.h"
#include <iostream>

PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = 0;
PFNGLCREATESHADERPROC glCreateShader = 0;
PFNGLSHADERSOURCEPROC glShaderSource = 0;
PFNGLCOMPILESHADERPROC glCompileShader = 0;
PFNGLATTACHSHADERPROC glAttachShader = 0;
PFNGLDELETESHADERPROC glDeleteShader = 0;
PFNGLGETSHADERIVPROC glGetShaderiv = 0;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = 0;
PFNGLCREATEPROGRAMPROC glCreateProgram = 0;
PFNGLLINKPROGRAMPROC glLinkProgram = 0;
PFNGLVALIDATEPROGRAMPROC glValidateProgram = 0;
PFNGLUSEPROGRAMPROC glUseProgram = 0;
PFNGLDELETEPROGRAMPROC glDeleteProgram = 0;
PFNGLGETPROGRAMIVPROC glGetProgramiv = 0;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = 0;
PFNGLUNIFORM1FPROC glUniform1f = 0;
PFNGLUNIFORM2FPROC glUniform2f = 0;
PFNGLUNIFORM3FPROC glUniform3f = 0;
PFNGLUNIFORM4FPROC glUniform4f = 0;
PFNGLUNIFORM1IPROC glUniform1i = 0;
PFNGLUNIFORM2IPROC glUniform2i = 0;
PFNGLUNIFORM3IPROC glUniform3i = 0;
PFNGLUNIFORM4IPROC glUniform4i = 0;
PFNGLUNIFORM1FVPROC glUniform1fv = 0;
PFNGLUNIFORM2FVPROC glUniform2fv = 0;
PFNGLUNIFORM3FVPROC glUniform3fv = 0;
PFNGLUNIFORM4FVPROC glUniform4fv = 0;
PFNGLUNIFORMMATRIX3FVPROC glUniformMatrix3fv = 0;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv = 0;
PFNGLUNIFORM1IVPROC glUniform1iv = 0;
PFNGLUNIFORM2IVPROC glUniform2iv = 0;
PFNGLUNIFORM3IVPROC glUniform3iv = 0;
PFNGLUNIFORM4IVPROC glUniform4iv = 0;
PFNGLVERTEXATTRIB1FPROC glVertexAttrib1f = 0;
PFNGLVERTEXATTRIB2FPROC glVertexAttrib2f = 0;
PFNGLVERTEXATTRIB3FPROC glVertexAttrib3f = 0;
PFNGLVERTEXATTRIB4FPROC glVertexAttrib4f = 0;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = 0;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation = 0;
PFNGLTEXIMAGE2DMULTISAMPLEPROC glTexImage2DMultisample = 0;
PFNGLTEXSTORAGE2DPROC glTexStorage2D = 0;
PFNGLTEXIMAGE3DPROC glTexImage3D = 0;
PFNGLTEXSTORAGE3DPROC glTexStorage3D = 0;
PFNGLTEXSUBIMAGE3DPROC glTexSubImage3D = 0;
PFNGLGENERATETEXTUREMIPMAPPROC glGenerateTextureMipmap = 0;
PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers = 0;
PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer = 0;
PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer = 0;
PFNGLDRAWBUFFERSPROC glDrawBuffers = 0;
PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus = 0;
PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers = 0;
PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D = 0;
PFNGLFRAMEBUFFERTEXTURELAYERPROC glFramebufferTextureLayer = 0;
PFNGLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer = 0;
PFNGLGENRENDERBUFFERSPROC glGenRenderbuffers = 0;
PFNGLDELETERENDERBUFFERSPROC glDeleteRenderbuffers = 0;
PFNGLBINDRENDERBUFFERPROC glBindRenderbuffer = 0;
PFNGLRENDERBUFFERSTORAGEPROC glRenderbufferStorage = 0;
PFNGLACTIVETEXTUREPROC glActiveTexture = 0;
PFNGLGENVERTEXARRAYSPROC glGenVertexArrays = 0;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays = 0;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray = 0;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer = 0;
PFNGLVERTEXATTRIBIPOINTERPROC glVertexAttribIPointer = 0;
PFNGLVERTEXATTRIBLPOINTERPROC glVertexAttribLPointer = 0;
PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisor = 0;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray = 0;
PFNGLDISABLEVERTEXATTRIBARRAYPROC glDisableVertexAttribArray = 0;
PFNGLBINDBUFFERPROC glBindBuffer = 0;
PFNGLDELETEBUFFERSPROC glDeleteBuffers = 0;
PFNGLGENBUFFERSPROC glGenBuffers = 0;
PFNGLBUFFERDATAPROC glBufferData = 0;
PFNGLCLEARBUFFERIVPROC glClearBufferiv = 0;
PFNGLCLEARBUFFERUIVPROC glClearBufferuiv = 0;
PFNGLCLEARBUFFERFVPROC glClearBufferfv = 0;
PFNGLCLEARBUFFERFIPROC glClearBufferfi = 0;
PFNGLSTENCILFUNCSEPARATEPROC glStencilFuncSeparate = 0;
PFNGLSTENCILOPSEPARATEPROC glStencilOpSeparate = 0;
PFNGLSTENCILMASKSEPARATEPROC glStencilMaskSeparate = 0;

#define DEBUG_GL

#include <string>

namespace Platform
{

	void reportFunc(const std::string& name, void * func)
	{
#ifdef DEBUG_GL
		auto error = glGetError();
		if (error!=GL_NO_ERROR || func==0)
		{
			std::cout << name << std::endl;
			if (error!=GL_NO_ERROR)
				std::cout << error << std::endl;
		}
#endif
	}

	RenderContext::RenderContext(HWND wnd, int depth)
	{
		PIXELFORMATDESCRIPTOR pfd= {
			sizeof(PIXELFORMATDESCRIPTOR),
			1,
			PFD_DRAW_TO_WINDOW |
			PFD_SUPPORT_OPENGL |
			PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA,
			32,
			0, 0, 0, 0, 0, 0,
			0,
			0,
			0,
			0, 0, 0, 0,
			depth,
			0,
			0,
			PFD_MAIN_PLANE,
			0,
			0, 0, 0
		};
		hWnd = wnd;
		hDC = GetDC(hWnd);
		PixelFormat = ChoosePixelFormat(hDC, &pfd);
		SetPixelFormat(hDC, PixelFormat, &pfd);
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
		wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
		reportFunc("wglSwapIntervalEXT", wglSwapIntervalEXT);
		glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
		reportFunc("glCreateShader", glCreateShader);
		glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
		reportFunc("glShaderSource", glShaderSource);
		glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
		reportFunc("glCompileShader", glCompileShader);
		glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
		reportFunc("glAttachShader", glAttachShader);
		glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
		reportFunc("glDeleteShader", glDeleteShader);
		glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
		reportFunc("glGetShaderiv", glGetShaderiv);
		glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
		reportFunc("glGetShaderInfoLog", glGetShaderInfoLog);
		glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
		reportFunc("glCreateProgram", glCreateProgram);
		glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
		reportFunc("glLinkProgram", glLinkProgram);
		glValidateProgram = (PFNGLVALIDATEPROGRAMPROC)wglGetProcAddress("glValidateProgram");
		reportFunc("glValidateProgram", glValidateProgram);
		glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
		reportFunc("glUseProgram", glUseProgram);
		glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
		reportFunc("glDeleteProgram", glDeleteProgram);
		glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
		reportFunc("glGetProgramiv", glGetProgramiv);
		glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
		reportFunc("glGetProgramInfoLog", glGetProgramInfoLog);
		glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
		reportFunc("glUniform1f", glUniform1f);
		glUniform2f = (PFNGLUNIFORM2FPROC)wglGetProcAddress("glUniform2f");
		reportFunc("glUniform2f", glUniform2f);
		glUniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");
		reportFunc("glUniform3f", glUniform3f);
		glUniform4f = (PFNGLUNIFORM4FPROC)wglGetProcAddress("glUniform4f");
		reportFunc("glUniform4f", glUniform4f);
		glUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FVPROC)wglGetProcAddress("glUniformMatrix3fv");
		reportFunc("glUniformMatrix3fv", glUniformMatrix3fv);
		glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
		reportFunc("glUniformMatrix4fv", glUniformMatrix4fv);
		glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
		reportFunc("glUniform1i", glUniform1i);
		glUniform2i = (PFNGLUNIFORM2IPROC)wglGetProcAddress("glUniform2i");
		reportFunc("glUniform2i", glUniform2i);
		glUniform3i = (PFNGLUNIFORM3IPROC)wglGetProcAddress("glUniform3i");
		reportFunc("glUniform3i", glUniform3i);
		glUniform4i = (PFNGLUNIFORM4IPROC)wglGetProcAddress("glUniform4i");
		reportFunc("glUniform4i", glUniform4i);
		glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
		reportFunc("glUniform1fv", glUniform1fv);
		glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
		reportFunc("glUniform2fv", glUniform2fv);
		glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
		reportFunc("glUniform3fv", glUniform3fv);
		glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
		reportFunc("glUniform4fv", glUniform4fv);
		glUniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
		reportFunc("glUniform1iv", glUniform1iv);
		glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
		reportFunc("glUniform2iv", glUniform2iv);
		glUniform3iv = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv");
		reportFunc("glUniform3iv", glUniform3iv);
		glUniform4iv = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv");
		reportFunc("glUniform4iv", glUniform4iv);
		glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1f");
		reportFunc("glVertexAttrib1f", glVertexAttrib1f);
		glVertexAttrib2f = (PFNGLVERTEXATTRIB2FPROC)wglGetProcAddress("glVertexAttrib2f");
		reportFunc("glVertexAttrib2f", glVertexAttrib2f);
		glVertexAttrib3f = (PFNGLVERTEXATTRIB3FPROC)wglGetProcAddress("glVertexAttrib3f");
		reportFunc("glVertexAttrib3f", glVertexAttrib3f);
		glVertexAttrib4f = (PFNGLVERTEXATTRIB4FPROC)wglGetProcAddress("glVertexAttrib4f");
		reportFunc("glVertexAttrib4f", glVertexAttrib4f);
		glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
		reportFunc("glGetUniformLocation", glGetUniformLocation);
		glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
		reportFunc("glGetAttribLocation", glGetAttribLocation);
		glTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLEPROC)wglGetProcAddress("glTexImage2DMultisample");
		reportFunc("glTexImage2DMultisample", glTexImage2DMultisample);
		glTexStorage2D = (PFNGLTEXSTORAGE2DPROC)wglGetProcAddress("glTexStorage2D");
		reportFunc("glTexStorage2D", glTexStorage2D);
		glTexImage3D = (PFNGLTEXIMAGE3DPROC)wglGetProcAddress("glTexImage3D");
		reportFunc("glTexImage3D", glTexImage3D);
		glTexStorage3D = (PFNGLTEXSTORAGE3DPROC)wglGetProcAddress("glTexStorage3D");
		reportFunc("glTexStorage3D", glTexStorage3D);
		glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)wglGetProcAddress("glTexSubImage3D");
		reportFunc("glTexSubImage3D", glTexSubImage3D);
		glGenerateTextureMipmap = (PFNGLGENERATETEXTUREMIPMAPPROC)wglGetProcAddress("glGenerateTextureMipmap");
		reportFunc("glGenerateTextureMipmap", glGenerateTextureMipmap);
		glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
		reportFunc("glGenFramebuffers", glGenFramebuffers);
		glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
		reportFunc("glBindFramebuffer", glBindFramebuffer);
		glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)wglGetProcAddress("glBlitFramebuffer");
		reportFunc("glBlitFramebuffer", glBlitFramebuffer);
		glDrawBuffers = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers");
		reportFunc("glDrawBuffers", glDrawBuffers);
		glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
		reportFunc("glCheckFramebufferStatus", glCheckFramebufferStatus);
		glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)wglGetProcAddress("glDeleteFramebuffers");
		reportFunc("glDeleteFramebuffers", glDeleteFramebuffers);
		glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
		reportFunc("glFramebufferTexture2D", glFramebufferTexture2D);
		glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)wglGetProcAddress("glFramebufferTextureLayer");
		reportFunc("glFramebufferTextureLayer", glFramebufferTextureLayer);
		glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbuffer");
		reportFunc("glFramebufferRenderbuffer", glFramebufferRenderbuffer);
		glGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffers");
		reportFunc("glGenRenderbuffers", glGenRenderbuffers);
		glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSPROC)wglGetProcAddress("glDeleteRenderbuffers");
		reportFunc("glDeleteRenderbuffers", glDeleteRenderbuffers);
		glBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbuffer");
		reportFunc("glBindRenderbuffer", glBindRenderbuffer);
		glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorage");
		reportFunc("glRenderbufferStorage", glRenderbufferStorage);
		glActiveTexture = (PFNGLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");
		reportFunc("glActiveTexture", glActiveTexture);
		glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
		reportFunc("glGenVertexArrays", glGenVertexArrays);
		glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)wglGetProcAddress("glDeleteVertexArrays");
		reportFunc("glDeleteVertexArrays", glDeleteVertexArrays);
		glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
		reportFunc("glBindVertexArray", glBindVertexArray);
		glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
		reportFunc("glVertexAttribPointer", glVertexAttribPointer);
		glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)wglGetProcAddress("glVertexAttribIPointer");
		reportFunc("glVertexAttribIPointer", glVertexAttribIPointer);
		glVertexAttribLPointer = (PFNGLVERTEXATTRIBLPOINTERPROC)wglGetProcAddress("glVertexAttribLPointer");
		reportFunc("glVertexAttribLPointer", glVertexAttribLPointer);
		glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)wglGetProcAddress("glVertexAttribDivisor");
		reportFunc("glVertexAttribDivisor", glVertexAttribDivisor);
		glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
		reportFunc("glEnableVertexAttribArray", glEnableVertexAttribArray);
		glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
		reportFunc("glDisableVertexAttribArray", glDisableVertexAttribArray);
		glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
		reportFunc("glBindBuffer", glBindBuffer);
		glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress("glDeleteBuffers");
		reportFunc("glDeleteBuffers", glDeleteBuffers);
		glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
		reportFunc("glGenBuffers", glGenBuffers);
		glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
		reportFunc("glBufferData", glBufferData);
		glClearBufferiv = (PFNGLCLEARBUFFERIVPROC)wglGetProcAddress("glClearBufferiv");
		reportFunc("glClearBufferiv", glClearBufferiv);
		glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC)wglGetProcAddress("glClearBufferuiv");
		reportFunc("glClearBufferuiv", glClearBufferuiv);
		glClearBufferfv = (PFNGLCLEARBUFFERFVPROC)wglGetProcAddress("glClearBufferfv");
		reportFunc("glClearBufferfv", glClearBufferfv);
		glClearBufferfi = (PFNGLCLEARBUFFERFIPROC)wglGetProcAddress("glClearBufferfi");
		reportFunc("glClearBufferfi", glClearBufferfi);
		glStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATEPROC)wglGetProcAddress("glStencilFuncSeparate");
		reportFunc("glStencilFuncSeparate", glStencilFuncSeparate);
		glStencilOpSeparate = (PFNGLSTENCILOPSEPARATEPROC)wglGetProcAddress("glStencilOpSeparate");
		reportFunc("glStencilOpSeparate", glStencilOpSeparate);
		glStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATEPROC)wglGetProcAddress("glStencilMaskSeparate");
		reportFunc("glStencilMaskSeparate", glStencilMaskSeparate);
	}

	void RenderContext::Swap(void)
	{
		SwapBuffers(hDC);
	}

	void RenderContext::SetVSync(int value)
	{
		wglSwapIntervalEXT(value);
	}

	RenderContext::~RenderContext(void)
	{
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);
	}

}