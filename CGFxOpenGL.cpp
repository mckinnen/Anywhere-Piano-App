/*
  checks to see if windows.h is not already included
	or instantiated.  if it isn't, it is added, as well
	as a header file that deals with basic console i/o.
	the other header files pretain to OpenGL, GLUT, or
	IrrKlang--the sound engine used to play the piano
	pitches.
*/

#ifdef _WINDOWS
#include <Windows.h>
#endif
#if defined(WIN32)
#include <conio.h>
#else
#include "../common/conio.h"
#endif
#include<GLee.h>
#include <stdio.h>
#include <irrKlang.h>
#include <gl/GL.h>
#include<gl/GLU.h>
#include<math.h>
#include "CGFxOpenGL.h"
#include <iostream>
#include <string.h>

/*
	Compiler directives; the first disables
	implicit float/double casting; the rest
	tell VS2010 to find the appropriate files
	and include them in the compilation and
	debugging of the program.  namespace
	irrklang is used to avoid the irrklang::
	that would be necessary to use methods
	specific to irrklang.
*/
#pragma warning(disable:4305)
#pragma comment(lib, "OPENGL32.lib")
#pragma comment(lib, "GLU32.lib")
#pragma comment(lib, "irrKlang.lib")
using namespace irrklang;
using namespace std;

// Selection Buffer:  for picking w/the mouse
#define SelBufferSize 512

// Mouse/Picking Stuff //
#define RENDER					1
#define SELECT					2
#define BUFSIZE 1024
GLuint selectBuf[BUFSIZE];

//prototype used to reset the rendering mode
//and pass the number of hits to the processPick
//funtion
void stopPicking();

//here the actual object that was clicked is found;
//the cursor position as well as the hit record,
//or the information concerning the users' click,
//is filtered through to determine which object was
//clicked
void processPick(GLint, GLuint[], int);

//in this function, the appropriate piano keys'
//pitch is heard based on the value passed into
//the Picked funtion; the number, which in this 
//program is the name of the obect, determines
//which pitch is heard.
void Picked(GLuint);

//this function draws a key based on the value passed in
//which is an iterative sentinel, initially utilized in a for
//loop.  This value determines which key is drawn, a white or
//black key, as well as the object's name, which is used by
//the Picked and processPick functions to determine what 
//key was clicked
void DrawWhiteKey(int);

//this function is called within the drawWhiteKey function.
//as the title states, this function draws a black key on 
//a similiar plane as a white key.
void DrawBlackKey();
GLint hits;						//the number of objects clicked
int mode = RENDER;				//sets the mode for drawing
int cursorX,cursorY;
bool isClicked = false;			//to determine if a sound should be played

CGFxOpenGL::CGFxOpenGL()
{}

CGFxOpenGL::~CGFxOpenGL()
{}

bool CGFxOpenGL::Init()
{
	//sets background color
	glClearColor(0.8f, 0.8f, 0.8f, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glRenderMode(GL_SELECT);

	return true;
}

bool CGFxOpenGL::Shutdown()
{
	return true;
}

void CGFxOpenGL::SetupProjection(int width, int height)
{
	if (height == 0)
		height =1;

	glViewport(0, 0, width, height);	//reset viewport to find new dimensions; sets up camera
	glMatrixMode(GL_PROJECTION);		//setup projection matrix current matrix
	glLoadIdentity();

	//calc aspect ratio of window
	gluPerspective(52.0f, (GLfloat)width/(GLfloat)height, 0.2f, 1000.0f);

	glMatrixMode(GL_MODELVIEW);		//set model view matrix; transformations on the actual object
	glLoadIdentity();				//reset modelview matrix:  looking down -z axis

	m_windowWidth = width;
	m_windowHeight = height;

}

/*
	The following features are disabled to more easily 
	allow two objects, or in this case multiple primitives,
	to be rendered a similiar z-axis.  This also sets
	the mode from render, which is used to draw the objects,
	to select, a mode used during the picking processes to
	communicate via the rendering window the position of the
	cursor and hit record of the click.
*/
void CGFxOpenGL::Prepare()
{
	glDisable (GL_BLEND); 
	glDisable (GL_DITHER); 
	glDisable (GL_FOG); 
	glDisable (GL_LIGHTING); 
	glDisable (GL_TEXTURE_1D); 
	glDisable (GL_TEXTURE_2D); 
	glDisable (GL_TEXTURE_3D); 
	glShadeModel (GL_FLAT);
	mode = SELECT;
	glRenderMode(GL_SELECT);

}
bool getIsClicked(){return isClicked;}
void setIsClicked(bool i){isClicked = i;}

void CGFxOpenGL::Render(float x, float y, int b)
{
	//clear screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	//move back 5 units so the drawn objects can be seen;
	//if not, then only a fraction of the screen is visible
	glTranslatef(0.0, 0.0, -15.0f);
	glRotatef(135.0f, 1.0f, 0.0f, 0.0f);

	//checks to see what mode the app is currently in...
	//also checks to see if isClicked is true, to indicate
	//a click, or false.
	if (b ==0)
		isClicked = false;
	else
		isClicked = true;

	if (mode == SELECT)
	{
		Picking(x, y, b);
	}
	
	Draw();

	if (mode == SELECT) 
		stopPicking();
	else{
		mode = RENDER;
		glRenderMode(GL_RENDER);
	}
}

/*
	This is main drawing function.  Here drawWhiteKey
	is called, which, based on the iteration of the 
	for loop, calls drawBlackKey.  The name of the 
	object, used during the picking process, is 
	given based on the iteration of the for loop.
*/
void CGFxOpenGL::Draw()
{
	for(int i=0; i< 13; i++)
	{
		glPushMatrix();
		glPushName(i+1);	//C is 1; C# is 2; etc...
		glTranslatef((-5.0f+(i/1.5f)), 0, 0);
		DrawWhiteKey(i);
		glPopName();
		glPopMatrix();
	}
}

/*
	Here a drawArray method is used; this allows one to specify vertices
	beforehand within an array, wich is then passed to the drawAray method
	as well as the number of vertices, and the type of primitives to be drawn
	so the entire object can be drawn at one time and not through numerous
	creations of separate vertice points.
*/
void DrawWhiteKey(int i)
{
	GLfloat verticesWK[] = {0.5,4,1,  -0.5,4,1,  -0.5,-4,1,  0.5,-4,1,        // v0-v1-v2-v3
						0.5,4,1,  0.5,-4,1,  0.5,-4,-1,  0.5,4,-1,			// v0-v3-v4-v5
                      0.5,4,1,  0.5,4,-1,  -0.5,4,-1,  -0.5,4,1,			// v0-v5-v6-v1
                      -0.5,4,1,  -0.5,4,-1,  -0.5,-4,-1,  -0.5,-4,1,		// v1-v6-v7-v2
                      -0.5,-4,-1,  0.5,-4,-1,  0.5,-4,1,  -0.5,-4,1,		// v7-v4-v3-v2
                      0.5,-4,-1,  -0.5,-4,-1,  -0.5,4,-1,  0.5,4,-1};		// v4-v7-v6-v5
	

	GLubyte indicesWK[] = {0, 1, 2, 3,							//the vertex points
						 0, 3, 4, 5,							//of the cube.
						 0, 5, 6, 1, 
						 1, 6, 7, 2,
						 7, 4, 3, 2, 
						 4, 7, 6, 5};
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, verticesWK);
	
	//this loop determines the creation of black keys
	if(i+1==2 || i+1==4 || i+1==7 || i+1==9 || i+1==11)
	{
		if(i+1==2)
		{ 
			glPushMatrix();
			glPushName(i+1);
			glTranslatef(0.0f, -2.0f, -1.0f);  //x:4.3, z:4.8, y:-5.8
			DrawBlackKey();
			glColor3f(0.0f, 0.0f, 0.0f);
			glDepthRange(0.0f, 0.5f);
			glTranslatef(0, 0, 0);
			glBegin(GL_LINES);//to divide the keys
			{
				glVertex3f(-0.5f, 5.0f, 1.5f);
				glVertex3f(-0.5f, -4.4f, 1.5f);
			}glEnd();
			glPopName();
			glPopMatrix(); }
		
		else if(i+1==4)
		{
			glPushMatrix();
			glPushName(i+1);
			glTranslatef(0.0f, -2.0f, -1.0f);  //x:4.3, z:4.8, y:-5.8
			DrawBlackKey();
			glColor3f(0.0f, 0.0f, 0.0f);
			glDepthRange(0.0f, 0.5f);
			glTranslatef(0, 0, 0);
			glBegin(GL_LINES);//to divide the keys
			{
				glVertex3f(-0.3f, 5.0f, 1.5f);
				glVertex3f(-0.3f, -4.4f, 1.5f);
			}glEnd();
			glPopName();
			glPopMatrix(); }
		
		else if(i+1==7) 
		{
			glPushMatrix();
			glPushName(i+1);
			glTranslatef(0.0f, -2.0f, -1.0f);  //x:4.3, z:4.8, y:-5.8
			DrawBlackKey();
			glColor3f(0.0f, 0.0f, 0.0f);
			glDepthRange(0.0f, 0.5f);
			glTranslatef(0, 0, 0);
			glBegin(GL_LINES);//to divide the keys
			{
				glVertex3f(-0.1f, 5.0f, 1.5f);
				glVertex3f(-0.1f, -4.4f, 1.5f);
			}glEnd();
			glPopName();
			glPopMatrix(); }

		else if(i+1==9) 
		{
			glPushMatrix();
			glPushName(i+1);
			glTranslatef(0.0f, -2.0f, -1.0f);  //x:4.3, z:4.8, y:-5.8
			DrawBlackKey();
			glColor3f(0.0f, 0.0f, 0.0f);
			glDepthRange(0.0f, 0.5f);
			glTranslatef(0, 0, 0);
			glBegin(GL_LINES);//to divide the keys
			{
				glVertex3f(0.0f, 5.0f, 1.5f);
				glVertex3f(0.0f, -4.4f, 1.5f);
			}glEnd();
			glPopName();
			glPopMatrix();  }

		else if(i+1==11) 
		{
			glPushMatrix();
			glPushName(i+1);
			glTranslatef(0.0f, -2.0f, -1.0f);  //x:4.3, z:4.8, y:-5.8
			DrawBlackKey();
			glColor3f(0.0f, 0.0f, 0.0f);
			glDepthRange(0.0f, 0.5f);
			glTranslatef(0, 0, 0);
			glBegin(GL_LINES);//to divide the keys
			{
				glVertex3f(0.2f, 5.0f, 1.5f);
				glVertex3f(0.2f, -4.4f, 1.5f);
			}glEnd();
			glPopName();
			glPopMatrix(); }
	}
	//this part determines the creation of white keys
	else{
		glPushMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);
		glDepthMask(GL_TRUE);
		glDepthFunc(GL_LESS);
		glDepthRange(0.1f, 1.0f);
		glDrawArrays(GL_QUADS, 0, 24);

		if(i+1==5)
		{
			glColor3f(0.0f, 0.0f, 0.0f);
			glDepthRange(0.0f, 0.5f);
			glTranslatef(0, 0, 0);
			glBegin(GL_LINES);//to divide the keys
			{
				glVertex3f(0.0f, 2.5f, 1.5f);
				glVertex3f(0.0f, -7.5f, 1.5f);
			}glEnd(); 
		}
		else if(i+1==12)
		{
			glColor3f(0.0f, 0.0f, 0.0f);
			glDepthRange(0.0f, 0.5f);
			glTranslatef(0, 0, 0);
			glBegin(GL_LINES);//to divide the keys
			{
				glVertex3f(0.8f, 2.5f, 1.5f);
				glVertex3f(0.8f, -7.5f, 1.5f);
			}glEnd(); 
		}
		
		glPopMatrix();}

	glDisableClientState(GL_VERTEX_ARRAY);  // disable vertex arrays

	return;

}

void DrawBlackKey()
{
	glPushMatrix();
	glColor3f(0.0f, 0.0f, 0.0f);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glDepthRange(0.0f, 0.5f);
	
	glBegin(GL_QUADS);
	{
		glVertex3f(0.2,2.0,0.5);
		glVertex3f(-0.2,2.0,0.5);
		glVertex3f(-0.2,-2.0,0.5);
		glVertex3f(0.2,-2.0,0.5);
			
		glVertex3f(0.2,2.0,0.5);
		glVertex3f(0.2,-2.0,0.5);
		glVertex3f(0.2,-2.0,-0.5);
		glVertex3f(0.2,2.0,-0.5);
				
		glVertex3f(0.2,2.0,0.5);
		glVertex3f(0.2,2.0,-0.5);
		glVertex3f(-0.2,2.0,-0.5);
		glVertex3f(-0.2,2.0,0.5);
				
		glVertex3f(-0.2,2.0,0.5);
		glVertex3f(-0.2,2.0,-0.5);
		glVertex3f(-0.2,-2.0,-0.5);
		glVertex3f(-0.2,-2.0,0.5);
				
		glVertex3f(-0.2,-2.0,-0.5);
		glVertex3f(0.2,-2.0,-0.5);
		glVertex3f(0.2,-2.0,-0.5);
		glVertex3f(-0.2,-2.0,0.5);
				
		glVertex3f(0.2,-2.0,-0.5);
		glVertex3f(-0.2,-2.0,-0.5);
		glVertex3f(-0.2,2.0,-0.5);
		glVertex3f(0.2,2.0,-0.5);
	}
	glEnd();

	glPopMatrix();
}

/*
	Thus begins the picking methods.  Here, the value of
	the cursor position, x and y, and the value of b, an
	arbitrary variable for determining whether an object
	was clicked, are used to determine the user's interactions
	with the screen.  "b" is given a value in WinMain during
	the mouse event handler; if the button was pressed, the 
	value of b is 1, which is true; if it is 0, the value
	is false and no click occurred, or the button was
	released.  The name stack is created, which holds
	the names of the objects created.  The buffer to hold
	the pickiing data is determined, as well as the size
	of said buffer.  Necessary changes are made to the camera
	and rendering window because now the mode is selection; 
	the y value of the cursor is decremented by the value 
	of the viewport because OpenGL's coordinates begin at
	the lower right of the screen, whereas windows coordinates
	begin at the upper left.  They are inverse!  For this reason,
	the viewport, or user's camera, is used to decrement the 
	y value of the OpenGL cursor.
*/
void CGFxOpenGL::Picking(float x, float y, int b)
{
	GLint viewport[4];
	float ratio;
	cursorX = x;
	cursorY = y;
	if (b == 0)
		isClicked = false;
	else
		isClicked = true;

	glInitNames();

	glSelectBuffer(BUFSIZE,selectBuf);

	glGetIntegerv(GL_VIEWPORT,viewport);

	glRenderMode(GL_SELECT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	gluPickMatrix(cursorX,viewport[3]-cursorY,5,5,viewport);
	ratio = (viewport[2]+0.0f) / viewport[3];
	gluPerspective(45,ratio,0.1,1000);
	glMatrixMode(GL_MODELVIEW);
}

void stopPicking()
{
	//restoring original projection
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glFlush();

	//returning to render mode
	hits = glRenderMode(GL_RENDER);
	if (hits != 0){
		processPick(hits,selectBuf,0);
	}
	mode = RENDER;
}
//selection buffer has four vals per hit: number of names in name stack for each hit,
//min z range of prim, max z range of prim, and the name of the prim clicked.
/*
	This function takes the values that were stored in the selection buffer,
	also known as the hit record, and uses them to identify which object
	was clicked.  The name of the object is stored in the variable ptrNames.
	This value is then passed into the Picked function which uses it to
	determine which pitch should be played.
*/
void processPick (GLint hits, GLuint buffer[], int sw)
{
   GLint i, j, numberOfNames;
   GLuint names, *ptr, minZ,*ptrNames;

   ptr = (GLuint *) buffer;	//pointer to selection buffer; to easily navigate through the array
   minZ = 0xffffffff;		//minimum z depth set to 0
   for (i = 0; i < hits; i++)
   {	
      names = *ptr;	//points to the num of hits
	  ptr++;		//points to min z range of hit
	  if (*ptr < minZ)
	  {
		  numberOfNames = names;	//now points to num of hits
		  minZ = *ptr;				//re-assign min z range
		  ptrNames = ptr+2;			//ptrNames points to the name of prim clicked
	  }
	  
	  ptr += names+2;
   }
   if (numberOfNames> 0) 
   {
	  ptr = ptrNames;
	  for (j = 0; j < numberOfNames; j++,ptr++) 
	  { 
		 Picked(*ptr);
	  }
	}
   else
	   return;
   }
   
/*In here!  Check to see what was clicked!!
	And play appropriate pitch!!  Recorded 
	by Nick McKinney with his Acer Aspire One*/
void Picked(GLuint j)
{
	ISoundEngine* engine = createIrrKlangDevice();

	if (!engine)
	{
		printf("Could not startup engine\n");
		return; // error starting up the engine
	}
	
	if(j == 1)//C
	{
		if(isClicked)
		{
			engine->play2D("pkC.wav");
			//change color here
			isClicked = false;

		}
		else return;
	}
	
	else if(j == 2)//C#
	{
		if(isClicked)
		{
			engine->play2D("pkC#.wav");
			//change color here
			isClicked = false;
		}
		else return;
	}

	else if(j ==3)//D
	{
		if(isClicked)
		{
			engine->play2D("pkD.wav");
			//change color here
			isClicked = false;
		}
		else return;
	}
	
	else if (j ==4)//D#(Eb)
	{
		if(isClicked)
		{
			engine->play2D("pkD#.wav");
			//change color here
			isClicked =false;
		}
		else return;
	}

	else if (j == 5)//E
	{
		if(isClicked)
		{
			engine->play2D("pkE.wav");
			//change color here
			isClicked = false;
		}
		else return;
	}

	else if (j == 6)//F
	{
		if(isClicked)
		{
			engine->play2D("pkF.wav");
			//change color here
			isClicked = false;
		}
		else return;
	}

	else if (j == 7)//F#(Gb)
	{
		if(isClicked)
		{
			engine->play2D("pkF#.wav");
			//change color here
			isClicked = false;
		}
		else return;
	}

	else if (j == 8)//G
	{
		if(isClicked)
		{
			engine->play2D("pkG.wav");
			//change color here
			isClicked = false;
		}
		else return;
	}

	else if (j == 9)//G#(Ab)
	{
		if(isClicked)
		{
			engine->play2D("pkAb.wav");
			//change color here
			isClicked = false;
		}
		else return;
	}

	else if (j == 10)//A
	{
		if(isClicked)
		{
			engine->play2D("pkA.wav");
			//change color here
			isClicked = false;
		}
		else return;
	}

	else if(j == 11)//A#(Bb)
	{
		if(isClicked)
		{
			engine->play2D("pkBb.wav");
			//change color here
			isClicked = false;
		}
		else return;
	}

	else if (j == 12)//B
	{
		if(isClicked)
		{
			engine->play2D("pkB.wav");
			//change color here
			isClicked = false;
		}
		else return;
	}

	else if (j == 13)
	{
		if(isClicked)
		{
			engine->play2D("pkC1.wav");
			isClicked = false;
		}
		else return;
	}
}
		
