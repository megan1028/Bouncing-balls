//For clarity,error checking has been omitted.
#include <CL/cl.h>
#include "tool.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>  // for MS Windows
#include "Dependencies\glew\glew.h"
#include "Dependencies\freeglut\freeglut.h"
#include <Math.h>     // Needed for sin, cos
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <iostream>
#include <thread>
using namespace std;
#define PI 3.14159265f
#define MAX_SOURCE_SIZE (0x100000)

float WINDOW_SIZE = 700;

int const NUM = 3;            //total number of circles (10 by default)
float radius = 0.1;            //circle radius
int waitTime = 33;            //milliseconds between steps
							  //coordinates
float *x = new float[NUM];
float *y = new float[NUM];
//velocity
float *vx = new float[NUM];
float *vy = new float[NUM];
//gravity
float g = 9.8;


//creates a gl triangle fan circle of indicated radius and segments
void flatCircle(float cx, float cy, float radius, int segments) {
	float phi, x1, y1;
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy);                    //center vertex
	for (int j = 0; j <= segments; j++) {    //for every segment,
		phi = 2 * PI * j / segments;    //calculate the new vertex
		x1 = radius * cos(phi) + cx;
		y1 = radius * sin(phi) + cy;
		glVertex2f(x1, y1);
	}
	glEnd();
} //end circle


  //initializes all circle posiitons, colors, and velocities
void initCircles(void) {
	srand(time(NULL));                            // seed the random number generator
	for (int i = 0; i < NUM; i++) {                // for each circle,
												   //current position
		x[i] = ((rand() % (int)(200 - (radius * 200))) / 100.0) - (1.0 - radius); //  random number between
		y[i] = ((rand() % (int)(200 - (radius * 200))) / 100.0) - (1.0 - radius); //    -0.9 and 0.9 (to account for radius size)

																				  //velocity
		vx[i] = ((rand() % 200) / 10000.0) - 0.01;     //    random velocities between
		vy[i] = ((rand() % 200) / 10000.0) - 0.01;     //      -0.02 and 0.02

	}
}



/* Callback handler for window re-paint event */
void display() {

	glClear(GL_COLOR_BUFFER_BIT);  // Clear the color buffer

	for (int i = 0; i < NUM; i++) {
		if (i % 3 == 0) {
			glColor3f(1.0, 0.0, 0.0);
			flatCircle(x[i] + x[i] / 15, y[i] + (y[i] - 1.0) / 20, radius, 30);
		}
		else if (i % 3 == 1) {
			glColor3f(0.0, 1.0, 0.0);
			flatCircle(x[i] + x[i] / 15, y[i] + (y[i] - 1.0) / 20, radius + 0.025, 30);
		}
		else {
			glColor3f(0.0, 0.0, 1.0);
			flatCircle(x[i] + x[i] / 15, y[i] + (y[i] - 1.0) / 20, radius + 0.04, 30);
		}

	}
	glFlush();
}


/* Call back when the windows is re-sized */
void reshape(int w, int h) {
	float aspectRatio = 1.0;

	//Compute the aspect ratio of the resized window
	aspectRatio = (float)h / (float)w;

	// Adjust the clipping box
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (h >= w)
		gluOrtho2D(-1.0, 1.0, -aspectRatio, aspectRatio);
	else
		gluOrtho2D(-1.0 / aspectRatio, 1.0 / aspectRatio, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);

	//adjust the viewport
	glViewport(0, 0, w, h);
}


/* Called back when the timer expired */
void timer(int value) {
	//Actually move the circles
	for (int i = 0; i < NUM; i++) {
		x[i] += vx[i];
		y[i] += vy[i];
	}
	for (int i = 0; i < NUM; i++) {
		vy[i] = vy[i] - g * 0.0001 * value;
	}

	//resolve collisions
	for (int i = 0; i < NUM; i++) {    //for each ball,
									   // Reverse direction when you reach edges
		if (x[i] > 1.0 - radius) {        //right edge
			x[i] = 1.0 - radius;                //to prevent balls from sticking
			vx[i] = -vx[i];                    //change velocity

		}
		else if (x[i] < -1.0 + radius) {    //left edge
			x[i] = -1.0 + radius;                ///to prevent balls from sticking
			vx[i] = -vx[i];                    //change velocity

		}

		if (y[i] > 1.0 - radius) {        //top edge
			y[i] = 1.0 - radius;            //to prevent balls from sticking
			vy[i] = -vy[i];                    //change velocity

		}
		else if (y[i] < -1.0 + radius) {    //bottom edge
			y[i] = -1.0 + radius;            //to prevent balls from sticking
			vy[i] = -vy[i];                    //change velocity

		}
	}

	glutPostRedisplay();
	glutTimerFunc(waitTime, timer, 1);
}



int main(int argc, char* argv[])
{
	cl_int    status;
	/**Step 1: Getting platforms and choose an available one(first).*/
	cl_platform_id platform;
	getPlatform(platform);

	/**Step 2:Query the platform and choose the first GPU device if has one.*/
	cl_device_id *devices = getCl_device_id(platform);

	/**Step 3: Create context.*/
	cl_context context = clCreateContext(NULL, 1, devices, NULL, NULL, NULL);

	/**Step 4: Creating command queue associate with the context.*/
	cl_command_queue commandQueue = clCreateCommandQueue(context, devices[0], 0, NULL);

	/**Step 5: Create program object */
	const char *filename = "dataparallel.cl";
	string sourceStr;
	status = convertToString(filename, sourceStr);
	const char *source = sourceStr.c_str();
	size_t sourceSize[] = { strlen(source) };
	cl_program program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);

	/**Step 6: Build program. */
	status = clBuildProgram(program, 1, devices, NULL, NULL, NULL);

	/**Step 7: Initial input,output for the host and create memory objects for the kernel*/

	/*const int NUM = 512000;
	double* input = new double[NUM];
	for (int i = 0; i<NUM; i++)
	input[i] = i;
	double* output = new double[NUM];*/
	float * randomNumber = new float[4 * NUM];

	srand(time(NULL));                            // seed the random number generator
	for (int i = 0; i < NUM; i++) {

		randomNumber[i * 4] = (rand() % (int)(200 - (radius * 200))) / 100.0; //  random number between	
		randomNumber[i * 4 + 1] = (rand() % (int)(200 - (radius * 200))) / 100.0; //  random number between
		randomNumber[i * 4 + 2] = (rand() % 200) / 10000.0;
		randomNumber[i * 4 + 3] = (rand() % 200) / 10000.0;

	}

	/*for (int i = 0; i < NUM; i++) {
		cout << randomNumber[i * 4] << "\n";
		cout << randomNumber[i * 4 +1] << "\n";
		cout << randomNumber[i * 4+2] << "\n";
		cout << randomNumber[i * 4+3] << "\n";
	}
*/
	float *r = &radius;
	
	
	cl_mem inputBuffer1 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float), (void *)r, NULL);
	cl_mem inputBuffer2 = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, 4 * (NUM) * sizeof(float), (void *)randomNumber, NULL);
	cl_mem outputBuffer1 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, NUM * sizeof(float), NULL, NULL);
	cl_mem outputBuffer2 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, NUM * sizeof(float), NULL, NULL);
	cl_mem outputBuffer3 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, NUM * sizeof(float), NULL, NULL);
	cl_mem outputBuffer4 = clCreateBuffer(context, CL_MEM_WRITE_ONLY, NUM * sizeof(float), NULL, NULL);
	

	/**Step 8: Create kernel object */
	cl_kernel kernel[4] = {NULL, NULL, NULL, NULL};
      kernel[0] = clCreateKernel(program, "dataParallel", NULL);
	 kernel[1] = clCreateKernel(program, "dataParallel2", NULL);
	 kernel[2] = clCreateKernel(program, "dataParallel3", NULL);
	 kernel[3] = clCreateKernel(program, "dataParallel4", NULL);

	/**Step 9: Sets Kernel arguments.*/
	 for (int i = 0; i < 4; i++) {

		 status = clSetKernelArg(kernel[i], 0, sizeof(cl_mem), (void *)&inputBuffer1);
		 status = clSetKernelArg(kernel[i], 1, sizeof(cl_mem), (void *)&inputBuffer2);
	}
	
	status = clSetKernelArg(kernel[0], 2, sizeof(cl_mem), (void *)&outputBuffer1);
	status = clSetKernelArg(kernel[1], 2, sizeof(cl_mem), (void *)&outputBuffer2);
	status = clSetKernelArg(kernel[2], 2, sizeof(cl_mem), (void *)&outputBuffer3);
	status = clSetKernelArg(kernel[3], 2, sizeof(cl_mem), (void *)&outputBuffer4);

	
	

	/**Step 10: Running the kernel.*/
	size_t global_work_size[1] = { NUM };
	//cl_event enentPoint;
	for (int i = 0; i < 4; i++) {
		status = clEnqueueNDRangeKernel(commandQueue, kernel[i], 1, NULL, global_work_size, NULL, 0, NULL, NULL);
	}
		/*clWaitForEvents(1, &enentPoint); ///wait
	
	
	clReleaseEvent(enentPoint);*/

	/**Step 11: Read the cout put back to host memory.*/
	status = clEnqueueReadBuffer(commandQueue, outputBuffer1, CL_TRUE, 0, NUM * sizeof(float), x, 0, NULL, NULL);
	status = clEnqueueReadBuffer(commandQueue, outputBuffer2, CL_TRUE, 0, NUM * sizeof(float), y, 0, NULL, NULL);
	status = clEnqueueReadBuffer(commandQueue, outputBuffer3, CL_TRUE, 0, NUM * sizeof(float), vx, 0, NULL, NULL);
	status = clEnqueueReadBuffer(commandQueue, outputBuffer4, CL_TRUE, 0, NUM * sizeof(float), vy, 0, NULL, NULL);

	
	cout << x[0];

	/**Step 12: Clean the resources.*/
	for (int i = 0; i < 4; i++) {
		status = clReleaseKernel(kernel[i]);//*Release kernel.
	}
	
	status = clReleaseProgram(program);    //Release the program object.
	status = clReleaseMemObject(inputBuffer1);//Release mem object.
	status = clReleaseMemObject(inputBuffer2);//Release mem object.
	status = clReleaseMemObject(outputBuffer1);
	status = clReleaseMemObject(outputBuffer2);
	status = clReleaseMemObject(outputBuffer3);
	status = clReleaseMemObject(outputBuffer4);
	status = clReleaseCommandQueue(commandQueue);//Release  Command queue.
	status = clReleaseContext(context);//Release context.


	//initCircles();        //initialize circle values

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_SINGLE | GLUT_RGBA | GLUT_ALPHA);

	glutInitWindowPosition(0, 0);                    //window position
	glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);    //window size
	glutCreateWindow("Bouncing balls");                //window name
	glClearColor(0.0, 0.0, 0.0, 0.0);                //background color
	glClear(GL_COLOR_BUFFER_BIT);

	//The four following statements set up the viewing rectangle
	glMatrixMode(GL_PROJECTION);                    // use proj. matrix
	glLoadIdentity();                                // load identity matrix
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);                // set orthogr. proj.
	glMatrixMode(GL_MODELVIEW);                        // back to modelview m.

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutDisplayFunc(display);

	glutTimerFunc(waitTime, timer, 1);
	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;

}























































































