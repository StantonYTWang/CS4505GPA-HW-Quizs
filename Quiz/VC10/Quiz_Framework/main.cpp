#include <glew.h>
#include <freeglut.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <cstdlib>
#include <texture_loader.h>

#define MENU_TIMER_START 1
#define MENU_TIMER_STOP 2
#define MENU_EXIT 3

GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

using namespace glm;

GLuint program;

mat4 scale(float x, float y, float z)
{
	mat4 m(1);
	// TODO compute the scaling matrix
	m[0][0] = x;
	m[1][1] = y;
	m[2][2] = z;
	return m;
}

mat4 translate(float x, float y, float z)
{
	mat4 m(1);
	// TODO compute the translation matrix
	m[3][0] = x;
	m[3][1] = y;
	m[3][2] = z;
	return m;
}

mat4 rotate(float angle, vec3 R)
{
	mat4 m(1);
	// TODO compute the rotation matrix
	// Tip: you have to convert angle from degree to radian
	float degree = angle*3.14159265358979323846;
	R = normalize(R);
	vec4 Q;
	Q.x = R.x*sin(degree/2.0);
	Q.y = R.y*sin(degree/2.0);
	Q.z = R.z*sin(degree/2.0);
	Q.w = cos(degree/2.0);
	float C = 1 - cos(degree);
	m[0][0] = cos(degree) + R.x*R.x*C;
	m[1][0] = R.x*R.y*C - R.x*sin(degree);
	m[2][0] = R.x*R.z*C + R.y*sin(degree);
	m[0][1] = R.x*R.y*C + R.z*sin(degree);
	m[1][1] = cos(degree) + R.y*R.y*C;
	m[2][1] = R.y*R.z*C - R.x*sin(degree);
	m[0][2] = R.x*R.z*C - R.y*sin(degree);
	m[1][2] = R.y*R.z*C + R.x*sin(degree);
	m[2][2] = cos(degree) + R.z*R.z*C;


	return m;
}

mat4 lookat(vec3 eye, vec3 center, vec3 up)
{
	// TODO compute the lookat matrix
	vec3 F = normalize(center - eye);
	vec3 S = normalize(cross(F, up));
	vec3 U_prime = normalize(cross(S, F));
	mat4 m1(1);
	mat4 m2(1);
	m1[0][0] = S.x;
	m1[1][0] = S.y;
	m1[2][0] = S.z;
	m1[0][1] = U_prime.x;
	m1[1][1] = U_prime.y;
	m1[2][1] = U_prime.z;
	m1[0][2] = -F.x;
	m1[1][2] = -F.y;
	m1[2][2] = -F.z;
	
	m2[3][0] = -eye.x;
	m2[3][1] = -eye.y;
	m2[3][2] = -eye.z;
    return m1*m2;
}

mat4 frustum(float left, float right, float bottom, float top, float nearVal, float farVal)
{
	mat4 m(0);
	// TODO compute the frustum projection matrix
	float l = left;
	float r = right;
	float b = bottom;
	float t = top;
	float n = nearVal;
	float f = farVal;

	m[0][0] = 2*n/(r-l);
	m[2][0] = (r+l)/(r-l);
	m[1][1] = 2*n/(t-b);
	m[2][1] = (t+b)/(t-b);
	m[2][2] = -(f+n)/(f-n);
	m[3][2] = -(2*f*n)/(f-n);
	m[2][3] = -1;
	return m;
}

void checkError(const char *functionName)
{
    GLenum error;
    while (( error = glGetError() ) != GL_NO_ERROR) {
        fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);
    }
}

// Print OpenGL context related information.
void dumpInfo(void)
{
    printf("Vendor: %s\n", glGetString (GL_VENDOR));
    printf("Renderer: %s\n", glGetString (GL_RENDERER));
    printf("Version: %s\n", glGetString (GL_VERSION));
    printf("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
}

char** loadShaderSource(const char* file)
{
	FILE* fp = fopen(file, "rb");
	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *src = new char[sz + 1];
	fread(src, sizeof(char), sz, fp);
	src[sz] = '\0';
	char **srcp = new char*[1];
	srcp[0] = src;
	return srcp;
}

void freeShaderSource(char** srcp)
{
	delete srcp[0];
	delete srcp;
}

void shaderLog(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		printf("%s\n", errorLog);
		delete errorLog;
	}
}

void My_Init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

    program = glCreateProgram();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char** vertexShaderSource = loadShaderSource("vertex.vs.glsl");
	char** fragmentShaderSource = loadShaderSource("fragment.fs.glsl");
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
	freeShaderSource(vertexShaderSource);
	freeShaderSource(fragmentShaderSource);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
	shaderLog(vertexShader);
    shaderLog(fragmentShader);
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glUseProgram(program);
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

// GLUT callback. Called to draw the scene.
void My_Display()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// !! DO NOT CHANGE THESE THREE LINES !!
	mat4 model = translate(0.5f, 0.3f, 0.0f) * scale(-0.8f, 1.2f, 1.0f) * rotate(30, vec3(10.0f, 10.0f, 10.0f));
	mat4 view = lookat(vec3(0.3, 0.5, 1), vec3(0.2, 0.4, 0), vec3(20, 100, 0));
	mat4 projection = frustum(-0.6, 1.2, -0.5, 1.3, 0.7, 3.0);
	// TODO compute the mvp matrix
	mat4 mvp = model*view*projection;
	glUniformMatrix4fv(glGetUniformLocation(program, "um4mvp"), 1, GL_FALSE, value_ptr(mvp));

	float f_timer_cnt = timer_cnt / 255.0f;
	float data[18] = {
		-0.5f, -0.4f, 0.0f,
		 0.5f, -0.4f, 0.0f,
	     0.0f,  0.6f, 0.0f,
		 f_timer_cnt, 0.0f, 1.0f - f_timer_cnt,
		 1.0f, f_timer_cnt, 1.0f - f_timer_cnt,
		 1.0f - f_timer_cnt, 0.0, f_timer_cnt
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float) * 9));
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glutSwapBuffers();
}

void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

void My_Timer(int val)
{
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
	My_Init();
	////////////////////

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