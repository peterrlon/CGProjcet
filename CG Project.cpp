/*

Specification : display a hierarchical object with 3 parts : Robotic hand example in Lecture

*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <glew.h>
#include <glut.h>
#include <gl\GL.h>
#include <gl\GLU.h>


//////////////////////////////////////////////////////////////////
// 
// Include the header file of our rotation user-interface.
// 
#include "gsrc.h"
// 
//////////////////////////////////////////////////////////////////


#define WIN_POSX    400
#define WIN_POSY    400
#define WIN_WIDTH   400
#define WIN_HEIGHT  300
#define GRIDSIZE 5
#define GL_CLAMP_TO_EDGE 0x812F
/* Global Declarations */
#define IW	808				// Image Width    
#define IH	627				// Image Height
#define M_PI 3.141592654



typedef struct point{ // define a structure for 3D point (x, y, z)
	GLfloat x;
	GLfloat y;
	GLfloat z;
} vertex;

vertex mesh[GRIDSIZE][GRIDSIZE];

unsigned char InputImage[IW][IH][4];
unsigned char Skin[IW][IH][4];
double theta;			 // rotation angles of robot, lower-and-upper arm, upper arm respectivley
GLUquadric *skin;
GLfloat ambientLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat initLightPos[] = { 200.0f, 80.0f, 200.0f, 1.0f };
GLfloat materialSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat materialEmission[] = { 0.5f, 0.5f, 0.5f, 1.0f };
GLfloat materialDiffse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat newLightPos[4];
GLfloat newLightAngle = 0.0;

void calculateplane(void)
// calculate the parameters of the plane mesh
{
	for (int i = 0; i < GRIDSIZE; i++)
		for (int j = 0; j < GRIDSIZE; j++)
		{
			mesh[i][j].x = 2 * float(i) / (GRIDSIZE - 1) - 1;
			mesh[i][j].z = 2 * float(j) / (GRIDSIZE - 1) - 1;
			mesh[i][j].y = 0;
		}
}

void draw_plane(unsigned char Image[IW][IH][4])
{
	int i, j;
	int K = GRIDSIZE - 1;
	
	glPushAttrib(GL_LIGHTING_BIT);
	//glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ambientLight);
	glPushMatrix();
	glScalef(150, 0, 150);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 808, 627, 0, GL_RGBA, GL_UNSIGNED_BYTE, InputImage);
	glEnable(GL_TEXTURE_2D);

	for (i = 0; i < K; i++)
		for (j = 0; j < K; j++)
		{
			glBegin(GL_QUADS);
			glTexCoord2f(0, 1); glVertex3f(mesh[i][j + 1].x, mesh[i][j + 1].y, mesh[i][j + 1].z);					//leftup
			glTexCoord2f(1, 1); glVertex3f(mesh[i + 1][j + 1].x, mesh[i + 1][j + 1].y, mesh[i + 1][j + 1].z);       //rightup
			glTexCoord2f(1, 0); glVertex3f(mesh[i + 1][j].x, mesh[i + 1][j].y, mesh[+1][j].z);                      //rightdown
			glTexCoord2f(0, 0); glVertex3f(mesh[i][j].x, mesh[i][j].y, mesh[i][j].z);                               //leftdown
			glEnd();
		}
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopAttrib();
}

void ReadRawImage(unsigned char Image[][IH][4], int x)
{
	FILE *fp;
	int  i, j, k;
	char filename[50];
	unsigned char temp;


	if (x == 1)
		strcpy(filename, "ground.raw");
	else
		strcpy(filename, "metalskin.raw");

	//printf("Error (ReadImage) : Cannot read the file!!\n");
	if ((fp = fopen(filename, "rb")) == NULL)
	{
		printf("Error (ReadImage) : Cannot read the file!!\n");
		exit(1);
	}

	for (i = 0; i < IW; i++)
	{
		for (j = 0; j < IH; j++)
		{
			for (k = 0; k < 3; k++)       // k = 0 is Red  k = 1 is Green K = 2 is Blue
			{
				fscanf(fp, "%c", &temp);
				Image[i][j][k] = (unsigned char)temp;
			}
			Image[i][j][3] = (unsigned char)0;         // alpha = 0.0
		}
	}
	fclose(fp);

}

//draw body
void draw_body(unsigned char Image[IW][IH][4])
{
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);					//set material specular coeff
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, materialDiffse);		//set material diffuse & ambient coeff
	//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, materialEmission);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);				//set texture magnification function to linear
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);					//Sets the wrap parameter for texture coordinate to repeat
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glEnable(GL_TEXTURE_2D);
	gluQuadricTexture(skin, GL_TRUE);
	glRotatef(-90, 1, 0, 0);
	glScalef(1.6, 1.6, 1.9);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 808, 627, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	gluSphere(skin, 1, 32, 32);
	glDisable(GL_TEXTURE_2D);
	glPopAttrib();
	glPopMatrix();
}

void draw_head(unsigned char Image[IW][IH][4])
{
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, materialDiffse);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glEnable(GL_TEXTURE_2D);
	gluQuadricTexture(skin, GL_TRUE);
	glRotatef(-90, 1, 0, 0);
	glScalef(1.2, 1.0, .9);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 808, 627, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	gluSphere(skin, 1, 32, 32);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopAttrib();
}

void draw_eye()
{
	
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glEnable(GL_COLOR_MATERIAL);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, diffuseCoeff);
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glColor3f(0.0, 0.0, 0.0);
	glScalef(0.2, 0.2, 0.15);
	glutSolidTorus(.3,.35,32,32);
	glDisable(GL_COLOR_MATERIAL);
	glPopAttrib();
	glPopMatrix();
	
}

void draw_ear(unsigned char Image[IW][IH][4])
{
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glEnable(GL_TEXTURE_2D);
	gluQuadricTexture(skin, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, materialEmission);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, materialDiffse);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 808, 627, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	glRotatef(-90, 1, 0, 0);
	glScalef(0.5, 0.5, 0.3);
	gluSphere(skin, 1, 32, 32);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopAttrib();
}

void draw_arm(unsigned char Image[IW][IH][4])
{
	glPushAttrib(GL_LIGHTING_BIT);
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	gluQuadricTexture(skin, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, materialEmission);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, materialDiffse);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 808, 627, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	glRotatef(-90, 1, 0, 0);
	glScalef(0.5, 0.5, 1.3);
	glTranslatef(0, 0, 1.5);
	gluSphere(skin, 1, 32, 32);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopAttrib();
}

void draw_leg(unsigned char Image[IW][IH][4])
{
	glPushAttrib(GL_LIGHTING_BIT);
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	gluQuadricTexture(skin, GL_TRUE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, materialEmission);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, materialDiffse);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 808, 627, 0, GL_RGBA, GL_UNSIGNED_BYTE, Image);
	glRotatef(-90, 1, 0, 0);
	glScalef(0.6, 0.6, 1.9);
	glTranslatef(0, 0, -.5);
	gluSphere(skin, 1, 32, 32);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
	glPopAttrib();
}

//glutIdle function: cal the walking motion angle 
void walk(void)					
{
	static float da = 1.0;

	theta += da;

	if (theta >  30.0)  da = -1.0;
	if (theta < -30.0)  da = 1.0;

	glutPostRedisplay();
}


//draw the bear model
void draw_bear()
{
	
	glPushMatrix();

		glTranslatef(0, 4.1, 0);
		draw_body(Skin);								//draw body
		glPushMatrix();

			glTranslatef(0, 2.3, 0);
			draw_head(Skin);							//draw head

			glPushMatrix();								//store head
				glTranslatef(.8, .6, 0);
				glRotatef(-25, 0, 0, 1);
				draw_ear(Skin);							//draw right ear
			glPopMatrix();								//restore head

			glPushMatrix();								//store head
				glScalef(-1, 1, 1);
				glTranslatef(.8, .6, 0);
				glRotatef(-25, 0, 0, 1);
				draw_ear(Skin);							//draw left ear
			glPopMatrix();								//restore head
			glPushAttrib(GL_CURRENT_BIT);
			glPushMatrix();
				glTranslatef(.45, .3, .83);
				draw_eye();								//draw left eye
			glPopMatrix();					

			glPushMatrix();
				glTranslatef(-.45, .3, .83);			//draw right eye
				draw_eye();
			glPopMatrix();
		glPopMatrix();									//resotre body with head and ears eyes
		glPopAttrib();
		glPushMatrix();									//store body with head and ears		
			glTranslatef(-.5, 1.4, 0);
			glRotatef(theta, 1, 0, 0);					//arm walking motion, rotation with x axis
			glRotated(135, 0, 0, 1);
			draw_arm(Skin);								//draw right arm
		glPopMatrix();									//restore body with left arm, head and ears
	
		glPushMatrix();									//store body with left arm,head and ears
			glScalef(-1, 1, 1);
			glTranslatef(-.5, 1.4, 0);
			glRotatef(-theta, 1, 0, 0);
			glRotated(135, 0, 0, 1);
			draw_arm(Skin);								//draw left arm
		glPopMatrix();									//restore body with head, ears and arms

		glPushMatrix();									//store body with head, ears, arms
			glTranslatef(-1.1, -1.5, 0);
			glRotatef(-theta, 1, 0, 0);					//leg walking motion, rotation with x axis
			glRotatef(-12, 0, 0, 1);
			draw_leg(Skin);								//draw left leg
		glPopMatrix();									//restore body with left leg,head,ears,arms

		glPushMatrix();									//store body with left leg,head,ears,arms
			glScalef(-1, 1, 1);
			glTranslatef(-1.1, -1.5, 0);
			glRotatef(theta, 1, 0, 0);
			glRotatef(-12, 0, 0, 1);
			draw_leg(Skin);							//draw right leg
		glPopMatrix();							//restore whole model
	glPopMatrix();
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
}

void setupRC()
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	//glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	
	glClearColor(0.0, 0.0, 0.0, 1.0);	// Set display-window color to black.
	glClear(GL_COLOR_BUFFER_BIT);
}

void setLight()
{
	newLightPos[0] = initLightPos[0] * cos(newLightAngle * 2 * M_PI / 360) - initLightPos[2] * sin(newLightAngle * 2 * M_PI / 360);
	newLightPos[1] = initLightPos[1];
	newLightPos[2] = initLightPos[0] * sin(newLightAngle * 2 * M_PI / 360) + initLightPos[2] * cos(newLightAngle * 2 * M_PI / 360);
	newLightPos[3] = 1.0;
	newLightAngle += 1;


	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1.0f, 1.0f, 0.0f);
	glTranslatef(newLightPos[0], newLightPos[1], newLightPos[2]);
	glutSolidSphere(2, 32, 32);
	glDisable(GL_COLOR_MATERIAL);
	
	glPopAttrib();
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_POSITION, newLightPos);
	glShadeModel(GL_SMOOTH);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 1);
	glEnable(GL_LIGHT0);
	glutPostRedisplay();

}


void draw_shadow(GLfloat shadwoMat[])
{
	glDisable(GL_LIGHTING);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(newLightPos[0], newLightPos[1], newLightPos[2]);			//reverse translate back to wc
	glMultMatrixf(shadwoMat);												//perspective projection matrix on x-z plane
	glTranslatef(-newLightPos[0], -newLightPos[1], -newLightPos[2]);		//translate to camera
	glColor3f(.0, .0, .0);
	draw_bear();
	glEnable(GL_LIGHTING);
	glPopMatrix();
}

void drawscene(void)
{
	//////////////////////////////////////////////////////////////////
	// 
	// Setup perspective projection and the rotation
	// 
	GLint viewport[4];
	GLfloat shadwoMat[16];
	for (int i = 0; i < 16; i++)									//cal perspective projection matrix on x-z plane
	{
		shadwoMat[i] = 0;
		shadwoMat[0] = shadwoMat[5] = shadwoMat[10] = 1;
		shadwoMat[7] = -1.0 / (newLightPos[1]-0.001);
	}

	glGetIntegerv(GL_VIEWPORT, viewport); // viewport is by default the display window
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90, double(viewport[2]) / viewport[3], 0.1, 500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		0, 35, 40,   //eye
		0, 0, 0,   //center
		0, 1, 0);  //up
	glMultMatrixf(gsrc_getmo());  // get the rotation matrix from the rotation user-interface

	setupRC();
	//Lighting
	setLight();

	//Draw Floor
	draw_plane(InputImage);
	
	glScalef(4.5, 4.5, 4.5);
	//Draw Bear
	draw_bear();

	//Draw Shadow
	draw_shadow(shadwoMat);

	glutSwapBuffers();
}

void main(int argc, char** argv)
{
	calculateplane();
	ReadRawImage(InputImage, 1);							//read texture raw file into buffer 
	ReadRawImage(Skin, 0);
	glutInit(&argc, argv);			                      // Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Set display mode

	glutInitWindowPosition(WIN_POSX, WIN_POSY);         // Set display-window position at (WIN_POSX, WIN_POSY) 
	// where (0, 0) is top left corner of monitor screen
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);		  // Set display-window width and height.

	glutCreateWindow("Robotic Arm");					  // Create display window.

	skin = gluNewQuadric();
	theta = 0;
	//////////////////////////////////////////////////////////////////
	// 
	// Register mouse-click and mouse-move glut callback functions
	// for the rotation user-interface.
	// 
	glutMouseFunc(gsrc_mousebutton);
	glutMotionFunc(gsrc_mousemove);
	//////////////////////////////////////////////////////////////////
	glutIdleFunc(walk);
	glutDisplayFunc(drawscene);   // put everything you wish to draw in drawscene

	glutMainLoop();

}