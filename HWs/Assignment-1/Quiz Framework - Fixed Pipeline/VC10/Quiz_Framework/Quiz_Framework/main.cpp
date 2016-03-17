#include <glew.h> // glew.h must be included before gl.h/glu.h/freeglut.h
#include <freeglut.h>
#include <cstdio>
#include <cstdlib>
#include <texture_loader.h>

#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_EXIT 3

const GLfloat tri_v1[3] = {-0.5f, -0.4f, 0.0f};
const GLfloat tri_v2[3] = { 0.5f, -0.4f, 0.0f};
const GLfloat tri_v3[3] = { 0.0f,  0.6f, 0.0f};

GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

int view_x, view_y, view_z;
int press;

GLuint texture_jpg;
GLuint texture_png;

// Print OpenGL context related information.
void dumpInfo(void)
{
    printf("Vendor: %s\n", glGetString (GL_VENDOR));
    printf("Renderer: %s\n", glGetString (GL_RENDERER));
    printf("Version: %s\n", glGetString (GL_VERSION));
    printf("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glRotated(view_x, -1.0, 0.0, 0.0);
	glRotated(view_y, 0.0, -1.0, 0.0);

	glPushMatrix();	//body
		glScalef(1.0, 2.0, 1.0);
		glColor3f(0.1f, 1.0f, 0.5f);
		glutSolidCube(4.0);
	glPopMatrix();

	glPushMatrix();	//center of body
		glTranslatef(0.0, 6.0, 0.0);
		glPushMatrix();	//head
			glColor3f(0.0f, 0.0f, 1.0f);
			glutSolidSphere(2.0, 200, 20);
			glPushMatrix();
				glTranslatef(0.7, 0.5, 2.0);	//eyes
				glColor3f(0.3, 0.5, 0.5);
				glutSolidSphere(0.3, 200, 20);
				glTranslatef(-1.3, 0.0, 0.0);
				glutSolidSphere(0.3, 200, 20);
			glPopMatrix();
			glPushMatrix();		//hat
				glColor3f(0.7, 0.2, 0.2);
				glTranslatef(0.0, 2.0, 0.0);
				glPushMatrix();
				glScalef(1.5, 0.0, 1.0);
				glutSolidSphere(2.15, 200, 20);
				glPopMatrix();
				glTranslatef(0.0, 1.0, 0.0);
				glutSolidCube(2.0);
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();

	glPushMatrix();		//left arm
		glTranslatef(-2.5, 1.3, 0.0);
		glColor3f(1.0, 1.0, 0.0);
		glutSolidSphere(0.5, 200, 20);
		glRotated(75, 0.0, 0.0, 1.0);
		glTranslatef(-2.0, 0.0, 0.0);
		glColor3f(0.5, 0.5, 0.5);
		glPushMatrix();
		glScalef(1.5, 0.5, 1.0);
		glutSolidCube(2.0);
		glPopMatrix();
		glTranslatef(-1.75, 0.0, 0.0);
		glColor3f(1.0, 1.0, 0.0);
		glutSolidSphere(0.4, 200, 20);
		glTranslatef(-1.75, 0.0, 0.0);
		glColor3f(0.5, 0.5, 0.5);
		glPushMatrix();
		glScalef(2.0/1.5, 0.5, 1.0);
		glutSolidCube(2.0);
		glPopMatrix();
		glColor3f(0.0, 1.0, 0.0);
		glTranslatef(-1.25, 0.0, 0.0);
		glRotated(90, 0.0, -1.0, 0.0);
		glutSolidCone(1.0, 2.0, 200, 20);
	glPopMatrix();

	glPushMatrix();		//right arm
		glTranslatef(2.5, 1.3, 0.0);
		glColor3f(1.0, 1.0, 0.0);
		glutSolidSphere(0.5, 200, 20);
		glRotated(75, 0.0, 0.0, -1.0);
		glTranslatef(2.0, 0.0, 0.0);
		glColor3f(0.5, 0.5, 0.5);
		glPushMatrix();
		glScalef(1.5, 0.5, 1.0);
		glutSolidCube(2.0);
		glPopMatrix();
		glTranslatef(1.75, 0.0, 0.0);
		glColor3f(1.0, 1.0, 0.0);
		glutSolidSphere(0.4, 200, 20);
		glTranslatef(1.75, 0.0, 0.0);
		glColor3f(0.5, 0.5, 0.5);
		glPushMatrix();
		glScalef(2.0/1.5, 0.5, 1.0);
		glutSolidCube(2.0);
		glPopMatrix();
		glColor3f(0.0, 1.0, 0.0);
		glTranslatef(1.25, 0.0, 0.0);
		glRotated(90, 0.0, 1.0, 0.0);
		glutSolidCone(1.0, 2.0, 200, 20);
	glPopMatrix();


	glutSwapBuffers();
}

void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	float viewportAspect = (float)width / (float)height;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, width/height, 1, 50);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 30.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void My_Timer(int val)
{
	if(press == 1){
		view_x = 15;
	}else if(press == 2){
		view_x = -15;
	}else if(press == 3){
		view_y = 15;
	}else if (press == 4){
		view_y = -15;
	}else {
		view_x = 0;
		view_y = 0;
	}

	press = 0;

	timer_cnt++;
	glutPostRedisplay();
	if(timer_enabled)
	{
		glutTimerFunc(timer_speed, My_Timer, val);
	}
}

void My_Mouse(int button, int state, int x, int y)
{
	if(state == GLUT_DOWN)
	{
		printf("Mouse %d is pressed at (%d, %d)\n", button, x, y);
	}
	else if(state == GLUT_UP)
	{
		printf("Mouse %d is released at (%d, %d)\n", button, x, y);
	}
}

void My_Keyboard(unsigned char key, int x, int y)
{
	switch(key){
		case 'w':
			press = 1;
			break;
		case 's':
			press = 2;
			break;
		case 'a':
			press = 3;
			break;
		case 'd':
			press = 4;
			break;
		default : press = 0;
	}
	printf("Key %c is pressed at (%d, %d)\n", key, x, y);
}

void My_SpecialKeys(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_F1:
		printf("F1 is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_PAGE_UP:
		printf("Page up is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_LEFT:
		printf("Left arrow is pressed at (%d, %d)\n", x, y);
		break;
	default:
		printf("Other special key is pressed at (%d, %d)\n", x, y);
		break;
	}
}

void My_Menu(int id)
{
	switch(id)
	{
	case MENU_TIMER_START:
		if(!timer_enabled)
		{
			timer_enabled = true;
			glutTimerFunc(timer_speed, My_Timer, 0);
		}
		break;
	case MENU_TIMER_STOP:
		timer_enabled = false;
		break;
	case MENU_EXIT:
		exit(0);
		break;
	default:
		break;
	}
}

/*void initTextures()
{
	// load jpg
	texture_data tdata = load_jpg("nthu.jpg"); // return width * height * 3 uchars
	if(tdata.data == 0)
	{
		// load failed
		return;
	}
	glGenTextures(1, &texture_jpg);
    glBindTexture(GL_TEXTURE_2D, texture_jpg);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tdata.width, tdata.height, 0, GL_RGB, GL_UNSIGNED_BYTE, tdata.data); // Use GL_RGB
	glGenerateMipmap(GL_TEXTURE_2D);
	free_texture_data(tdata);

	// load png
	tdata = load_png("nthu.png"); // return width * height * 4 uchars
	if(tdata.data == 0)
	{
		// load failed
		return;
	}
	glGenTextures(1, &texture_png);
    glBindTexture(GL_TEXTURE_2D, texture_png);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tdata.width, tdata.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tdata.data); // Use GL_RGBA
	glGenerateMipmap(GL_TEXTURE_2D);
	free_texture_data(tdata);
}*/

int main(int argc, char *argv[])
{
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Quiz Framework"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
	glewInit();
    dumpInfo();
	////////////////////
	
	// Initialize OpenGL states.
	////////////////////////
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	//initTextures();
	////////////////////////

	// Create a menu and bind it to mouse right button.
	////////////////////////////
	int menu_main = glutCreateMenu(My_Menu);
	int menu_timer = glutCreateMenu(My_Menu);

	glutSetMenu(menu_main);
	glutAddSubMenu("Timer", menu_timer);
	glutAddMenuEntry("Exit", MENU_EXIT);

	glutSetMenu(menu_timer);
	glutAddMenuEntry("Start", MENU_TIMER_START);
	glutAddMenuEntry("Stop", MENU_TIMER_STOP);

	glutSetMenu(menu_main);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	////////////////////////////

	// Register GLUT callback functions.
	///////////////////////////////
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutMouseFunc(My_Mouse);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(timer_speed, My_Timer, 0); 
	///////////////////////////////

	// Enter main event loop.
	//////////////
	glutMainLoop();
	//////////////
	return 0;
}