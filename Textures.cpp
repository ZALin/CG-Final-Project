#include "Textures.h"
//#include "imageIO.c"
#include <fstream>
#include "Global.h"
/*
#include <iostream>
#include <string>
#include <map>

#include <GL/glfw.h>

/*** Global variables ***/

static map<string, CTextureID> TextureList;






/********************MyBMP_Image********************/

/*** Public member functions ***/

MyBMP_Image::MyBMP_Image()
{
	pixels = NULL;
	width = 0;
	height = 0;
	BytesPerPixel = 4;
	header = new unsigned char[54];//header大54byte(一般情況)
}

int MyBMP_Image::four_c_to_int(unsigned char* s)
{
	int ans=0;
	for(int i=3 ; i>=0 ; i--)
	{
		ans += s[i];
		if(i >= 1){ans <<= 8;}
	}
	return ans;
}

void MyBMP_Image::int_to_four_c(unsigned char* s , int x)
{
	for(int i=0 ; i<=3 ; i++)
	{
		s[i] = x % 256;
		x >>= 8;
	}
}

bool MyBMP_Image::ReadImage(const char* file_name)
{
	ifstream inf;
	//--
	inf.open(file_name , ifstream::binary);//開檔
	if(inf.is_open() == false){ return false;}
	inf.read((char*)(header) , 54);
	//for(int i=0 ; i<=56 ; i++){cout<<hex<<i<<":"<<dec<<(int)header[i]<<" ";}
	width = four_c_to_int(&header[18]);
	height = four_c_to_int(&header[22]);
	//cout <<"width:"<<width<<endl;
	//cout <<"height:"<<height<<endl;
	int allign_len=4-((width*3)%4);
	if(allign_len == 4){allign_len = 0;}//剛好是4的倍數就不用補了
	int row_len = width * 3 + allign_len;
	int data_len = height * row_len;
	unsigned char* buffer=new unsigned char[data_len];
	pixels = new unsigned char[width * 3 * height];
	inf.read((char*)(buffer) , data_len);
	int temp_buffer_id;
	int temp_pixel_id;
	for(int which_col=0 ; which_col<height ; which_col++)
	{
		for(int which_row=0 ; which_row<width ; which_row++)
		{
			temp_buffer_id = row_len * which_col + which_row * 3;
			temp_pixel_id = width * 3 * which_col + which_row * 3;
			pixels[temp_pixel_id] = buffer[temp_buffer_id+2];
			pixels[temp_pixel_id+1] = buffer[temp_buffer_id+1];
			pixels[temp_pixel_id+2] = buffer[temp_buffer_id];
		}
	}
	//關檔
	inf.close();
	return true;
}

void MyBMP_Image::LoadImage()
{
	glTexImage2D(GL_TEXTURE_2D,0,3,width,height,0,GL_RGB,GL_UNSIGNED_BYTE, pixels);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
}


/********************CTextureManager********************/

/*** Public member functions ***/


CTextureManager::CTextureManager():m_iTexturesInUse(2)
{

    for( int i = 2; i < GL_MAX_TEXTURE_UNITS; ++i ){
        m_rgfTexMatUsed[i] = false;
    }
    m_rgfTexMatUsed[0] = true;
    m_rgfTexMatUsed[1] = true;

    m_rgTextureList.clear();
}


CTextureManager::~CTextureManager()
{
    for( unsigned i = 0; i < m_rgTextureList.size(); ++i ){
        glDeleteTextures(1, &m_rgTextureList[i]);
        //delete m_rgTextureList[i];
    }
    m_rgTextureList.clear();
}


void CTextureManager::Initial()
{
    SetDefaultEnv();
}


GLenum CTextureManager::RequestTextureUnit(){
    GLenum result;

    if( m_iTexturesInUse < GL_MAX_TEXTURE_UNITS ){
        result = GL_TEXTURE0 + m_iTexturesInUse;
        ++m_iTexturesInUse;
    }else{
        result = GL_INVALID_ENUM;
    }
    return result;
}


void CTextureManager::SetDefaultEnv()
{
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                     GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    //glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, (float[]){1, 1, 1, 0} );

    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //glTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, (float[]){0, 0, 0, 0} );
    glEnable(GL_TEXTURE_2D);

}


void CTextureManager::BindTexture( CTextureID pTexture ){
    //glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pTexture);

	GLuint texLoc = glGetUniformLocation( GetShaderProgram() , "tex");
    glUniform1i(texLoc, 0);
}


void CTextureManager::BindNormalTexture( CTextureID pTexture ){
    //glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pTexture);
}


CTextureID CTextureManager::CreateTexture( const char strTextureName[] )
{
	map<string, CTextureID>::iterator it = TextureList.find(strTextureName);

	if( it == TextureList.end() ){
		//printf("it == TextureList.end() is true!!\n");//<<<<


		CTextureID newTex = CreateEmptyTexture();
		//printf("in CreateEmptyTexture() => newTex:%d\n",newTex);//<<<<
 
		glBindTexture(GL_TEXTURE_2D, newTex);
		//printf("newTex:%d\n",newTex);//<<<<
		if( ReadTexture(strTextureName) ){
			m_rgTextureList.push_back(newTex);
		}else{
			cerr << "Failed to create texture "<< strTextureName << endl;
			glDeleteTextures(1, &newTex);
			glBindTexture(GL_TEXTURE_2D, 0);
			return NULL;
		}

		TextureList[strTextureName] = newTex;

		glBindTexture(GL_TEXTURE_2D, 0);
		return newTex;
	}else{
		return it->second;
	}
}


CTextureID CTextureManager::CreateEmptyTexture()
{
    CTextureID newTex;
    glGenTextures(1, &newTex);
	//printf("in CreateEmptyTexture() => newTex:%d\n",newTex);//<<<<
    return newTex;
}


int	CTextureManager::GetTextureUsed(){
	return m_rgTextureList.size();
}


bool CTextureManager::ReadTexture( const char strTextureName[] )
{
	
	bool result=true;
	MyBMP_Image texData;
	/*
	ColorImage texData;
	readPPM("checker.ppm", &texData);
    glTexImage2D(GL_TEXTURE_2D,0,3,texData.xRes,texData.yRes,0,GL_RGB,GL_UNSIGNED_BYTE, texData.pPixel);
		*/
	/*
	string test(strTextureName);
	test += "1";
	ofstream outfile(test);
	outfile.close();
	*/
	result = texData.ReadImage(strTextureName);
	if( result ){
        
		texData.LoadImage();
    }else{
        cerr << "Failed to read texture " << strTextureName << endl;
    }
	
	/*
    CTextureID result;
	GLFWimage texData;
	
    result = glfwReadImage(strTextureName, &texData, GLFW_BUILD_MIPMAPS_BIT);
	cerr << result << endl;
	printf("result:%d\n",result);//<<<<
    if( result ){
        
        glfwLoadTextureImage2D(&texData, GLFW_BUILD_MIPMAPS_BIT);

		glfwFreeImage(&texData);
    }else{
        cerr << "Failed to read texture " << strTextureName << endl;
    }
	*/
    return result;
	
}


void CTextureManager::DeleteTexture( CTextureID pTexture )
{
    for( unsigned i = 0; i < m_rgTextureList.size(); ++i ){
        if( m_rgTextureList[i] == pTexture ){
            glDeleteTextures(1, &m_rgTextureList[i]);
            m_rgTextureList.erase(m_rgTextureList.begin()+i);
            break;
        }
    }
}

/********************CTextureManager********************/
