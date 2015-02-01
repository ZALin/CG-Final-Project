#ifndef TEXTURES_H
#define TEXTURES_H

#include <vector>
#include <iostream>
#include <string>
#include <map>

#include <GL/glew.h>
#include <GL/glut.h>

//#include <GL/glfw.h>

using namespace std;

//typedef unsigned int  CTextureID;
typedef GLuint CTextureID;

class CTextureManager{
public:
    CTextureManager();
    ~CTextureManager();

    void    Initial();

    void    SetDefaultEnv();

    void    BindTexture( CTextureID pTexture );

    void    BindNormalTexture( CTextureID pTexture );

    GLenum  RequestTextureUnit();

    CTextureID  CreateTexture( const char strTextureName[] );

    CTextureID  CreateNormalTexture( const char strTextureName[] );

    CTextureID  CreateEmptyTexture();

	int		GetTextureUsed();

	bool    ReadTexture( const char strTextureName[] );

    void    DeleteTexture( CTextureID pTexture );

private:
    int     m_iTexturesInUse;
    bool    m_rgfTexMatUsed[GL_MAX_TEXTURE_UNITS-1];
    vector<CTextureID> m_rgTextureList;
};

class MyBMP_Image
{
public:
	unsigned char* pixels;
	int width;
	int height;
	int BytesPerPixel; 
	unsigned char* header;
	//--
	MyBMP_Image();
	bool ReadImage(const char* file_name);
	void LoadImage();
	//void gen_texture(char* file_name);

//private:
	int four_c_to_int(unsigned char* s);
	void int_to_four_c(unsigned char* s , int x);
};


#endif // TEXTURES_H
