/*

Specification : display a hierarchical object with 3 parts : Robotic hand example in Lecture

*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <gl\glut.h>

//////////////////////////////////////////////////////////////////
// 
// Include the header file of our rotation user-interface.
// 
#include "gsrc.h"
// 
//////////////////////////////////////////////////////////////////


#define PI			3.141592654  // Prime
#define WIN_POSX    400
#define WIN_POSY    400
#define WIN_WIDTH   400
#define WIN_HEIGHT  300
#define GRIDSIZE 31

typedef struct point{ // define a structure for 3D point (x, y, z)
	GLfloat x;
	GLfloat y;
	GLfloat z;
} vertex;

vertex mesh[GRIDSIZE][GRIDSIZE];


double theta, phi, psi;			 // rotation angles of robot, lower-and-upper arm, upper arm respectivley

void cube()
// draw a standard 2 x 2 x 2 cube whose center is at (0, 1, 0)
{

	/* The use of glPushMatrix and glPopMatrix here protects the glRotatef from
	exercising its influence on the relative transformation beyond this function */
	glPushMatrix();

	glTranslatef(0, 1, 0);
	glutSolidCube(2);

	glPopMatrix();

}

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

void draw_base(void)
// draw base : blue cylinder of radius 30 height 40  x-z as base
{
	glPushMatrix();


	GLUquadricObj *pObj1, *pObj2, *pObj3;

	glColor3f(0.0, 0.0, 1.0);
	glRotatef(-90.0, 1.0, 0.0, 0.0);   // rotate about x axis by -90 deg.

	// draws a hollow cylinder along the z axis with base on x-y axis, origin at (0, 0, 0) 
	pObj1 = gluNewQuadric();
	gluCylinder(pObj1, 30, 30, 40, 10, 10);
	// base radius 30  
	// top  radius 30 
	// height 40
	// 10 x 10 controls the sampling

	// draw a solid disk to cover the base 
	pObj2 = gluNewQuadric();
	gluDisk(pObj2, 0, 30, 10, 10);

	// draw a solid disk to cover the top
	glPushMatrix();
	glTranslatef(0, 0, 40);
	pObj3 = gluNewQuadric();
	gluDisk(pObj3, 0, 30, 10, 10);
	glPopMatrix();

	glPopMatrix();
}

void draw_lower_arm(void)
{
	glPushMatrix();					// draw lower robotic arm : green box of dimension 15 x 70 x 15
	glColor3f(0.0, 1.0, 0.0);
	glScalef(7.5, 35.0, 7.5);
	cube();
	glPopMatrix();
}


void draw_upper_arm(void)
{
	glPushMatrix();					// draw upper robotic arm : yellow box of dimension 15 x 40 x 15
	glColor3f(1.0, 1.0, 0.0);
	glScalef(7.5, 20.0, 7.5);
	cube();
	glPopMatrix();
}

void draw_plane(void)
{
	int i, j;
	int K = GRIDSIZE - 1;
	glPushMatrix();
	glColor3f(1, 0, 0);
	glScalef(8, 0, 4);
	for (i = 0; i < K; i++)
		for (j = 0; j < K; j++)
		{
			glBegin(GL_QUADS);
			glVertex3f(mesh[i][j + 1].x, mesh[i][j + 1].y, mesh[i][j + 1].z);					//leftup
			glVertex3f(mesh[i + 1][j + 1].x, mesh[i + 1][j + 1].y, mesh[i + 1][j + 1].z);       //rightup
			glVertex3f(mesh[i + 1][j].x, mesh[i + 1][j].y, mesh[+1][j].z);                      //rightdown
			glVertex3f(mesh[i][j].x, mesh[i][j].y, mesh[i][j].z);                               //leftdown
			glEnd();
		}

	glPopMatrix();
}

void draw_body(double rx, double ry, double rz)
{
	glPushMatrix();
	glColor3f(0.0, 0.1, 1.0);
	glScalef((GLfloat)(rx), (GLfloat)(ry), (GLfloat)(rz));
	glutWireSphere(1, 10, 10);

	glPopMatrix();
}
void drawscene(void)
{


	//////////////////////////////////////////////////////////////////
	// 
	// Setup perspective projection and the rotation
	// 
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport); // viewport is by default the display window
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, double(viewport[2]) / viewport[3], 0.1, 50);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(
		0, 2.5, 6,   //eye
		0, 0, 0,   //center
		0, 1, 0);  //up
	glMultMatrixf(gsrc_getmo());  // get the rotation matrix from the rotation user-interface
	//
	//////////////////////////////////////////////////////////////////


	/*  Enable Z buffer method for visibility determination. */
	//  Z buffer code starts

	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// Z buffer code ends */

	glClearColor(1.0, 1.0, 1.0, 0.0);	// Set display-window color to white.
	glClear(GL_COLOR_BUFFER_BIT);		// Clear display window.


	//glColor3f(1, 0, 0);  // Set line segment color to red.
	//glScalef(2, 0, 200);
	// Advice: draw as few lines as possible. Eliminate all redundant drawing.


	//glPushMatrix();
	//glRotatef(theta, 0.0, 1.0, 0.0);      // rotate the whole robot arm by theta degrees
	//draw_base();
	//glTranslatef(0.0, 40.0, 0.0);		// M_(lower arm to base)
	//glRotatef(phi, 0.0, 0.0, 1.0);      // rotate upper and lower arm by phi degrees
	//draw_lower_arm();
	//glTranslatef(0.0, 70.0, 0.0);      // M_(upper arm to lower arm) 
	//glRotatef(psi, 0.0, 0.0, 1.0);      // rotate upper arm by psi degrees
	//draw_upper_arm();
	//glPopMatrix();

	draw_plane();
	glTranslatef(0, .8, 0);
	glRotatef(90, 1, 0, 0);
	draw_body(1, .8, .8);


	glutSwapBuffers();
}

void drawing(GLubyte key, GLint xMouse, GLint yMouse)
{
	/* simple animation can be achieved by repeating key tabs */

	switch (key) {
	case '0':		theta = phi = psi = 0;
		break;
	case '1':		theta = 0;  phi = 0;  psi += 30;
		break;
	case '2':		theta = 0;  phi += 60; psi = 30;
		break;
	case '3':		theta += 45; phi = 60; psi = 30;
		break;
	default:		printf("invalid choices!");
	}

	glutPostRedisplay();                   // redisplay the modified object

}


void main(int argc, char** argv)
{
	calculateplane();
	glutInit(&argc, argv);			                      // Initialize GLUT
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Set display mode

	glutInitWindowPosition(WIN_POSX, WIN_POSY);         // Set display-window position at (WIN_POSX, WIN_POSY) 
	// where (0, 0) is top left corner of monitor screen
	glutInitWindowSize(WIN_WIDTH, WIN_HEIGHT);		  // Set display-window width and height.

	glutCreateWindow("Robotic Arm");					  // Create display window.

	theta = 0; phi = 0; psi = 0;

	printf("Type                                              \n");
	printf("0 : Initial position							   \n");
	printf("1 : Rotate only the upper arm by 30 deg           \n");
	printf("2 : Rotate the lower arm by 45 deg, then (1)      \n");
	printf("3 : Rotate the whole robot by 45 deg, then (2)    \n");
	printf("                                                  \n");
	printf("Try                                               \n");
	printf("Press 0 then 1                                    \n");
	printf("Press 0 then 2                                    \n");
	printf("Press 0 then 3                                    \n");
	printf("Observe the difference							   \n");

	glutKeyboardFunc(drawing);


	//////////////////////////////////////////////////////////////////
	// 
	// Register mouse-click and mouse-move glut callback functions
	// for the rotation user-interface.
	// 
	glutMouseFunc(gsrc_mousebutton);
	glutMotionFunc(gsrc_mousemove);
	//
	//////////////////////////////////////////////////////////////////


	glutDisplayFunc(drawscene);   // put everything you wish to draw in drawscene

	glutMainLoop();

}