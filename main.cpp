/** @file main.cpp
 *  @brief Computer Graphic homework 2
 *  @Compiling enviroment : Win7-64 with VC++
                            Win7-64 with Visual Studio 2010 professional
 *  @author 40047032S
 */

//#include <Windows.h>    // for solving the Code::Blocks errors

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <time.h>
#include <math.h>
#include <sstream> 

#include <GL/glew.h>
#include <GL/glut.h>

#include "textfile.h"
#include "Textures.h"

#include "glm.h"
#include "Global.h"

//static GLfloat theta[] = {0.0,0.0,0.0};
//static GLint axis = 2;

GLMmodel *myObj = NULL;
GLMmodel *iona_q[20];
GLMmodel *haruna_q[12];
GLMmodel *takao_q[7];
GLMmodel *iona_n = NULL;
GLMmodel *town = NULL; 
GLMmodel *ecasttle = NULL;
bool orth = false, color = false; 
int fovy = 30;

double k_PI = 2*asin(1.0);
 
GLint TotalScale = 5 ;

// Angles of x-axis and y-asis.
GLint theta = 304, omega = 0;

// Variances of angle
GLint var_theta = 0, var_omega = 0;

// Parameter of gluLookAt.
GLfloat radius = 0.01f;
GLfloat movestep = 1.0f;
GLfloat eye[3];
GLfloat camera[3] = {47.0f, 1.0f, -25.0f};

// Last mouse position.
int last_x, last_y;


//light source (sun)
GLfloat sun_x=180,sun_y=0;
GLfloat diffuse0[]={1.0, 1.0, 1.0, 1.0};
GLfloat ambient0[]={1.0, 1.0, 1.0, 1.0};
GLfloat specular0[]={1.0, 1.0, 1.0, 1.0};
GLfloat light0_pos[]={sun_x, sun_y, 0.0, 1.0};
GLint sun_omega=0;
GLint sun_radius=180;

//Some parameter for shaders
GLint loc , angleloc;
GLhandleARB v,f;
bool isDiffuseWithColor = false; 
bool isDiffuse = false; 
bool isNoShader = true; 


//Some parameter for Animate
int displayNum_yaya = 0;
int displayNext_yaya = 1;
int displayNum_iona = 0;
int displayNext_iona = 1;
int displayNum_haruna = 0;
int displayAngle_haruna = 0;
int displayNum_takao = 0;
int displayNext_takao = 1;

GLuint skytexID;
GLuint skytoptexID;
GLuint groundtexID;

void PrintStateInfo()
{
	printf("\n\n=====================================================\n");
	printf("Now is ");
	if( isNoShader )
	{
		printf("No Shader !\n");
	}
	else if( isDiffuse )
	{
		printf("Diffuse Shader !\n");
	}
	else
	{
		printf("DiffuseWithColor Shader !\n");
	}
	printf("光源現在的角度: %d\n",sun_omega);
	printf("光源現在的位置: ( %f , %f , %f )\n",light0_pos[0],light0_pos[1],light0_pos[2]);
	printf("相機現在的位置: ( %f , %f , %f )\n",camera[0],camera[1],camera[2]);
	printf("fovy:%d 看向: ( %f , %f , %f )\n",fovy,eye[0],eye[1],eye[2]);
	printf("=====================================================\n");
}

#define printOpenGLError() printOglError(__FILE__, __LINE__)

int printOglError(char *file, int line)
{
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        printf("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
    return retCode;
}


void printInfoLog(GLhandleARB obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;

	glGetObjectParameterivARB(obj, GL_OBJECT_INFO_LOG_LENGTH_ARB,
                                         &infologLength);

    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetInfoLogARB(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n",infoLog);
        free(infoLog);
    }
}

void setShaders() 
{
	static bool inited = false;
	if(isNoShader ) 
	{
		glUseProgramObjectARB(0);
	}
	else
	{
		char *vs = NULL,*fs = NULL;
	
		if (! inited) {
			v = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
			f = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		}

		vs = textFileRead("toon.vert");
		if( isDiffuseWithColor )
		{
			fs = textFileRead("diffuseWithColor.frag");
		}
		else if( isDiffuse )
		{
			fs = textFileRead("diffuse.frag");
		
		}
	
		const char * vv = vs;
		const char * ff = fs;

		glShaderSourceARB(v, 1, &vv,NULL);
		glShaderSourceARB(f, 1, &ff,NULL);

		free(vs);free(fs);

		glCompileShaderARB(v);
		glCompileShaderARB(f);

		printInfoLog(v);
		printInfoLog(f);

		if (! inited) {
			GetShaderProgram() = glCreateProgramObjectARB();
		}
		glAttachObjectARB(GetShaderProgram(),v);
		glAttachObjectARB(GetShaderProgram(),f);

		glLinkProgramARB(GetShaderProgram());
		printInfoLog(GetShaderProgram());

		glUseProgramObjectARB(GetShaderProgram());
		
		loc = glGetUniformLocation (GetShaderProgram(), "lightpos");
		angleloc = glGetUniformLocation (GetShaderProgram() , "angle");
		inited = true;
	}
	
	
}

void drawOBJ()
{
	
	if (! myObj) return;

	//glmDraw(myObj, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL  );
	glmDraw(myObj, GLM_SMOOTH | GLM_TEXTURE  | GLM_MATERIAL );
		/*
		float *p;
		static GLMgroup* group;
		group = myObj->groups;
		while (group) {

			if(group->material && myObj->materials[group->material].texture)
					glBindTexture(GL_TEXTURE_2D, (myObj->materials[group->material].texture));	

			for (i=0; i<myObj->numtriangles; i++) { 
				 // The current triangle is: myObj->triangles[i]
				
				 glBegin(GL_TRIANGLES);
					for (v=0; v<3; v++) {
						// Process the normals.
						if (myObj->numnormals > 0) {
						   p = & myObj->normals[ myObj->triangles[i].nindices[v]*3 ];
						   glNormal3fv(p);
						}
            
						// Process the texture coordinates.
						if (myObj->numtexcoords > 0) {
						   p = & myObj->texcoords[ myObj->triangles[i].tindices[v]*2 ];
               
               
						   glTexCoord2fv(p);
               
						}

						p = & myObj->vertices[ myObj->triangles[i].vindices[v]*3 ];

						glVertex3fv( p );
					}
				 glEnd();
			}
			group = group->next;
		 }
		
		*/
}
void drawBG()
{
	
	glBindTexture(GL_TEXTURE_2D, skytoptexID);
	glBegin( GL_TRIANGLES );
		
		glTexCoord2f(0.0f,1.0f);
		glVertex3f( 1000.0, 700.0, 1000.0 );
		glTexCoord2f(1.0f,1.0f);
		glVertex3f( 1000.0, 700.0, -1000.0 );
		glTexCoord2f(0.5f,0.0f);
		glVertex3f( 0.0, 700.0, 0.0 );
		
		glTexCoord2f(1.0f,1.0f);
		glVertex3f( 1000.0, 700.0, -1000.0 );
		glTexCoord2f(0.0f,1.0f);
		glVertex3f( -1000.0, 700.0, -1000.0 );
		glTexCoord2f(0.5f,0.0f);
		glVertex3f( 0.0, 700.0, 0.0 );
		
		glTexCoord2f(0.0f,1.0f);
		glVertex3f( -1000.0, 700.0, -1000.0 );
		glTexCoord2f(1.0f,1.0f);
		glVertex3f( -1000.0, 700.0, 1000.0 );
		glTexCoord2f(0.5f,0.0f);
		glVertex3f( 0.0, 700.0, 0.0 );

		glTexCoord2f(1.0f,1.0f);
		glVertex3f( -1000.0, 700.0, 1000.0 );
		glTexCoord2f(0.0f,1.0f);
		glVertex3f( 1000.0, 700.0, 1000.0 );
		glTexCoord2f(0.5f,0.0f);
		glVertex3f( 0.0, 700.0, 0.0 );
		
	glEnd();
	
	glBindTexture(GL_TEXTURE_2D, groundtexID);
	glBegin( GL_QUADS );
		glTexCoord2f(25.0f,25.0f);
		glVertex3f( 1000.0, -6.0, 1000.0 );
		glTexCoord2f(25.0f,-25.0f);
		glVertex3f( 1000.0, -6.0, -1000.0 );
		glTexCoord2f(-25.0f,-25.0f);
		glVertex3f( -1000.0, -6.0, -1000.0 );
		glTexCoord2f(-25.0f,25.0f);
		glVertex3f( -1000.0, -6.0, 1000.0 );
	glEnd();

	glBindTexture(GL_TEXTURE_2D, skytexID);
	glBegin( GL_QUADS );
		glTexCoord2f(0.0f,1.0f);
		glVertex3f( -1000.0, 700.0, -1000.0 );
		glTexCoord2f(1.0f,1.0f);
		glVertex3f( 1000.0, 700.0, -1000.0 );
		glTexCoord2f(1.0f,0.0f);
		glVertex3f( 1000.0, -7.0, -1000.0 );
		glTexCoord2f(0.0f,0.0f);
		glVertex3f( -1000.0, -7.0, -1000.0 );
		
		glTexCoord2f(0.0f,1.0f);
		glVertex3f( 1000.0, 700.0, 1000.0 );
		glTexCoord2f(1.0f,1.0f);
		glVertex3f( 1000.0, 700.0, -1000.0 );
		glTexCoord2f(1.0f,0.0f);
		glVertex3f( 1000.0, -7.0, -1000.0 );
		glTexCoord2f(0.0f,0.0f);
		glVertex3f( 1000.0, -7.0, 1000.0 );

		glTexCoord2f(1.0f,1.0f);
		glVertex3f( -1000.0, 700.0, 1000.0 );
		glTexCoord2f(0.0f,1.0f);
		glVertex3f( 1000.0, 700.0, 1000.0 );
		glTexCoord2f(0.0f,0.0f);
		glVertex3f( 1000.0, -7.0, 1000.0 );
		glTexCoord2f(1.0f,0.0f);
		glVertex3f( -1000.0, -7.0, 1000.0 );

		glTexCoord2f(1.0f,1.0f);
		glVertex3f( -1000.0, 700.0, 1000.0 );
		glTexCoord2f(0.0f,1.0f);
		glVertex3f( -1000.0, 700.0, -1000.0 );
		glTexCoord2f(0.0f,0.0f);
		glVertex3f( -1000.0, -7.0, -1000.0 );
		glTexCoord2f(1.0f,0.0f);
		glVertex3f( -1000.0, -7.0, 1000.0 );
	glEnd();
}

void display(void)
{
	/* display callback, clear frame buffer and z buffer,
	   rotate cube and draw, swap buffers */
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glLoadIdentity();

	gluLookAt( camera[0], camera[1], camera[2],
               eye[0], eye[1], eye[2],
               0.0f, 1.0f, 0.0f );

	/*//for test
	printf("theta: %d  omega:%d \n",theta,omega);
	printf("eye:");
	for(int i=0;i<3;++i)printf(" %f ",eye[i]);
	printf("\n");
	printf("camera:");
	for(int i=0;i<3;++i)printf(" %f ",camera[i]);
	printf("\n");
	*/
	
	// Draw Yaya 
	glPushMatrix();
		glRotatef( -75.0f, 0.0f, 1.0f, 0.0f );
		drawOBJ();
	glPopMatrix();
	
	
	// Draw Iona normal
	glPushMatrix();
		glTranslatef( 6.0f *TotalScale/5 , -0.45f *TotalScale/5 ,10.0f*TotalScale/5 );
		glRotatef( -45.0f, 0.0f, 1.0f, 0.0f );
		glmDraw(iona_n, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL );
	glPopMatrix();

	//Draw Iona Q animate
	glPushMatrix();
		glTranslatef( 6.0f *TotalScale/5, 0.0f, -8.0f *TotalScale/5);
		glScalef( 0.40f *TotalScale/5 ,0.40f*TotalScale/5 ,0.40f*TotalScale/5 );
		glRotatef( -60.0f, 0.0f, 1.0f, 0.0f );
		glTranslatef( 0.0f , -12.3f  , 0.0f);
		glmDraw(iona_q[displayNum_iona], GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL );
	glPopMatrix();
	
	//Draw Harun Q animate
	glPushMatrix();
		glTranslatef( 13.0f *TotalScale/5, 0.0f, -2.0f *TotalScale/5);
		glScalef( 0.40f *TotalScale/5 ,0.40f*TotalScale/5 ,0.40f*TotalScale/5 );
		glRotatef( -60.0+displayAngle_haruna, 0.0f, 1.0f, 0.0f );
		glTranslatef( 0.0f , -12.3f  , 0.0f);
		glmDraw(haruna_q[displayNum_haruna], GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL );
	glPopMatrix();

	//Draw Takao Q animate
	glPushMatrix();
		glTranslatef( 14.0f *TotalScale/5, 0.0f, 7.0f *TotalScale/5);
		glScalef( 0.40f *TotalScale/5 ,0.40f*TotalScale/5 ,0.40f*TotalScale/5 );
		glRotatef( -60.0f, 0.0f, 1.0f, 0.0f );
		glTranslatef( 0.0f , -12.3f  , 0.0f);
		glmDraw(takao_q[displayNum_takao], GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL );
	glPopMatrix();
	


	glPushMatrix();
		glTranslatef( TotalScale*7.0f, TotalScale*3.95f, TotalScale*8.0f);
		glmDraw(town, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL );
    glPopMatrix();
	

	glPushMatrix();
		glTranslatef( -132.0f *TotalScale/5, 45.0f *TotalScale/5, 761.0f *TotalScale/5);
		glRotatef( 180.0f, 0.0f, 1.0f, 0.0f );
		glmDraw(ecasttle, GLM_SMOOTH | GLM_TEXTURE | GLM_MATERIAL );
    glPopMatrix();

	drawBG();
	



	glFlush();
	glutSwapBuffers();
}

void loop()
{
	static clock_t counter = clock();
	static int count_iona = 0;
	static int count_haruna = 0;
	static int count_takao = 0;
	// FPS = 60
    if( clock() - counter > CLOCKS_PER_SEC/60 )
    {
        // Sets up eye location.
        eye[0] = camera[0] + radius*cos(omega*k_PI/180.0)*sin(theta*k_PI/180.0);
        eye[1] = camera[1] + radius*sin(omega*k_PI/180.0);
        eye[2] = camera[2] + radius*cos(omega*k_PI/180.0)*cos(theta*k_PI/180.0);


		light0_pos[0] = sun_x = sun_radius*cos(sun_omega*k_PI/180.0);
		light0_pos[1] = sun_y = sun_radius*sin(sun_omega*k_PI/180.0);
		glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	
		glUniform4fvARB(loc,4,light0_pos);
	
        // Resets counter.
        counter = clock();

		if( count_iona == 10 )
		{
			displayNum_iona += displayNext_iona;
			if( (displayNum_iona == 0 && displayNext_iona == -1) ||  (displayNum_iona == 19 && displayNext_iona == 1)) 
			{
				displayNext_iona *= -1;
			}
			count_iona = 0;
		}

		if( count_haruna == 15 )
		{
			displayNum_haruna = (displayNum_haruna+1)%12;
			count_haruna = 0;
			displayAngle_haruna += 3;
			if(displayAngle_haruna>=360) displayAngle_haruna-=360; 
		}

		if( count_takao == 15 )
		{
			displayNum_takao += displayNext_takao ;
			if( (displayNum_takao == 0 && displayNext_takao == -1) ||  (displayNum_takao == 6 && displayNext_takao == 1)) 
			{
				displayNext_takao *= -1;
			}
			count_takao=0;
		}

		count_iona++ ;
		count_haruna++;
		count_takao++;

        /* display(); */
        glutPostRedisplay();
    }

}

void mouse(int btn, int state, int x, int y)
{
/* mouse callback, selects an axis about which to rotate */
	if(state == GLUT_DOWN)
	{
		last_x = x;
        last_y = y;
	}
}

void ChangeOmega(int var)
{
	if( omega + var_omega < 90 && omega + var_omega > -90 )
        omega += var;
}

void ChangeTheta(int var)
{
	theta += var;
	if( theta >= 360 )theta -= 360;
    if( theta <= -360 )theta += 360;
}

void ChangeSunOmega(int var)
{
	sun_omega += var;
	if( sun_omega >= 360 ) sun_omega -= 360;
    if( sun_omega < 0 ) sun_omega += 360;
	glUniform1iARB(angleloc, sun_omega);

}
	
void motion(int x, int y){

    var_theta = last_x - x;
    var_omega = last_y - y;

	ChangeTheta(var_theta);
	ChangeOmega(var_omega);

    last_x = x;
    last_y = y;
}

void myReshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
	if( orth )
	{
		if (w <= h)
			glOrtho(-2.0, 2.0, -2.0 * (GLfloat) h / (GLfloat) w,
				2.0 * (GLfloat) h / (GLfloat) w, -10.0, 10.0);
		else
			glOrtho(-2.0 * (GLfloat) w / (GLfloat) h,
				2.0 * (GLfloat) w / (GLfloat) h, -2.0, 2.0, -10.0, 10.0);
	}
	else
	{
		gluPerspective( fovy, (GLfloat) w / (GLfloat) h, 0.1, 10000 );
	}
	glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y){
    switch( key ){
        case 'o':
        case 'O':
            orth = true;
            myReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            break;
        case 'p':
        case 'P':
            orth = false; 
            myReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            break;
        case '>':
            if( fovy < 120 )fovy += 5;
            myReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            break;
        case '<':
            if( fovy > 10 )fovy -= 5;
            myReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
            break;
        case 'w':
        case 'W':
            camera[0] += sin(theta*k_PI/180.0)*movestep;
            camera[2] += cos(theta*k_PI/180.0)*movestep;
            break;
        case 's':
        case 'S':
            camera[0] -= sin(theta*k_PI/180.0)*movestep;
            camera[2] -= cos(theta*k_PI/180.0)*movestep;
            break;
        case 'a':
        case 'A':
            camera[0] += cos(theta*k_PI/180.0)*movestep;
            camera[2] -= sin(theta*k_PI/180.0)*movestep;
            break;
        case 'd':
        case 'D':
            camera[0] -= cos(theta*k_PI/180.0)*movestep;
            camera[2] += sin(theta*k_PI/180.0)*movestep;
            break;
		case 'r':
        case 'R':
            camera[1] += movestep;
            break;
		case 'f':
        case 'F':
            camera[1] -= movestep;
            break;
		case 'h':
        case 'H': 
				ChangeTheta(1);
            break;
		case 'k':
        case 'K':
				ChangeTheta(-1);
			break;
		case 'u':
        case 'U':
			if(omega < 89 ) ChangeOmega(1);
            break;
		case 'j':
        case 'J':
            if(omega > -89 ) ChangeOmega(-1);
            break;
		case 'i':
        case 'I':
			camera[0] = 47.0f ;
			camera[1] = 1.0f ;
			camera[2] = -25.0f ;
			orth = false ;
			color = true;
			fovy = 30 ;
			theta = 304 ;
			omega = 0 ;
			sun_omega=0;
			isDiffuseWithColor = false; 
			isDiffuse = false; 
			isNoShader = true; 
			setShaders();
			myReshape(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
			break;
		case '+':
            ChangeSunOmega(1);
            break;
        case '-':
             ChangeSunOmega(-1);
            break;
		case 'z':
        case 'Z': 
			isDiffuseWithColor = false; 
			isDiffuse = true; 
			isNoShader = false; 
			setShaders();
            break;
		case 'x':
        case 'X': 
			isDiffuseWithColor = false; 
			isDiffuse = false; 
			isNoShader = true; 
			setShaders();
            break;
		case 'c':
        case 'C': 
			isDiffuseWithColor = true; 
			isDiffuse = false; 
			isNoShader = false; 
			setShaders();
            break;
		case 'm':
        case 'M': 
			PrintStateInfo();
            break;
        default:
            break;
    }
}

void initObj()
{
	town = glmReadOBJ("models/old_town_block.obj");
	glmUnitize(town) ;
	glmScale(town , TotalScale*38);

	ecasttle = glmReadOBJ("models/eastern_ancient_casttle.obj");
	glmUnitize(ecasttle) ;
	glmScale(ecasttle , TotalScale*50);
	
	myObj = glmReadOBJ("models/MD_Yaya_MMD_test.obj");
	//myObj = glmReadOBJ("models/takao_Q/Takao_01.obj");
	glmUnitize(myObj) ;
	glmScale(myObj , TotalScale);
	
	iona_n = glmReadOBJ("models/iona_n.obj");
	glmUnitize(iona_n) ;
	glmScale(iona_n , TotalScale*0.9);
	
	string strname="";
	
	string iona_qdir = "models/iona_Q/Iona_" ;
	for(int i=0 ; i<20 ; ++i)
	{
		strname="";
		int tmpint = i + 1;
		string int_str;
		stringstream ss;
		ss << tmpint;
		ss >> int_str;

		strname += iona_qdir;
		if(i<=8)
		{
			
			strname += "0" ;
			strname += int_str ;
		}
		else
		{
			strname += int_str ;
		}
		strname += ".obj";
		//cout << strname << endl;
		char *cstr = new char[strname.length()+1];
		strcpy(cstr, strname.c_str());
		iona_q[i] = glmReadOBJ(cstr);
	}

	string haruna_qdir = "models/haruna_Q/Haruna_" ;
	for(int i=0 ; i<12 ; ++i)
	{
		strname="";
		int tmpint = i + 1;
		string int_str;
		stringstream ss;
		ss << tmpint;
		ss >> int_str;

		strname += haruna_qdir;
		if(i<=8)
		{
			strname += "0" ;
			strname += int_str ;
		}
		else
		{
			strname += int_str ;
		}
		strname += ".obj";
		//cout << strname << endl;
		char *cstr = new char[strname.length()+1];
		strcpy(cstr, strname.c_str());
		haruna_q[i] = glmReadOBJ(cstr);
	}


	string takao_qdir = "models/takao_Q/Takao_" ;
	for(int i=0 ; i<7 ; ++i)
	{
		strname="";
		int tmpint = i + 1;
		string int_str;
		stringstream ss;
		ss << tmpint;
		ss >> int_str;
			
		strname += takao_qdir;
		strname += "0" ;
		strname += int_str ;
		
		strname += ".obj";
		//cout << strname << endl;
		char *cstr = new char[strname.length()+1];
		strcpy(cstr, strname.c_str());
		takao_q[i] = glmReadOBJ(cstr);
	}

}

void initBG()
{
	MyBMP_Image sky;
	glGenTextures(1, &skytexID);
	//printf("skytexID : %d\n",skytexID);
	glBindTexture(GL_TEXTURE_2D, skytexID);
	if (sky.ReadImage("texture/BG/sky.bmp") )
	{
		sky.LoadImage();
	}
	else
	{
		printf("failed to read : texture/BG/sky.bmp\n");
	}
	MyBMP_Image ground;
	glGenTextures(1, &groundtexID);
	glBindTexture(GL_TEXTURE_2D, groundtexID);
	if (ground.ReadImage("texture/BG/grass.bmp") )
	{
		ground.LoadImage();
	}
	else
	{
		printf("failed to read : texture/BG/grass.bmp\n");
	}
	MyBMP_Image skytop;
	glGenTextures(1, &skytoptexID);
	glBindTexture(GL_TEXTURE_2D, skytoptexID);
	if (skytop.ReadImage("texture/BG/skytop.bmp") )
	{
		skytop.LoadImage();
	}
	else
	{
		printf("failed to read : texture/BG/skytop.bmp\n");
	}
}

void initial()
{
    glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );
}
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
	//glfwInit();

/* need both double buffering and z buffer */
	
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Yaya");
    glutReshapeFunc(myReshape);
    glutDisplayFunc(display);
	   glutIdleFunc(loop);
	   glutMouseFunc(mouse);
	   glutMotionFunc(motion);
	   glutKeyboardFunc(keyboard);
	   glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
	   //glEnable(GL_COLOR_MATERIAL);
	   glEnable(GL_LIGHTING);
	   glEnable(GL_LIGHT0);
	   glEnable(GL_TEXTURE_2D);
	   
	   initial();

	   glewInit();
		if (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)
			printf("Ready for GLSL\n");
		else {
			printf("No GLSL support\n");
			exit(1);
		}
		
	   
		//char name[100];
		//scanf("%s",name);
		initObj();
		initBG();
		setShaders();

    glutMainLoop();
    return 0;
}
