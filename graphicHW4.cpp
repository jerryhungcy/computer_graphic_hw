/*-----------------------------------------------------------
 * An example program to draw a car
 *   Divide the window into four parts, each part shows an image
 *   resulting from a projection method.
 *   Author: S.K. Ueng
 *   Date:  11/4/2001
 */
#include <stdio.h>
#include <math.h>

#include <GL/glut.h>

#define  PI   3.141592653
#define STOP 0
#define WALK 1
#define SWINGHAND 2
#define JUMP 3
#define GOLD 1
#define SILVER 2
#define PLASTIC 3

#define Step  0.5

 /*-----Define a unit box--------*/
 /* Vertices of the box */
float  points[][3] = { {-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5},
					  {0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5},
					  {-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5},
					  {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5} };
/* face of box, each face composing of 4 vertices */
int    face[][4] = { {0, 3, 2, 1}, {0, 1, 5, 4}, {1, 2, 6, 5},
					{4, 5, 6, 7}, {2, 3, 7, 6}, {0, 4, 7, 3} };
/* indices of the box faces */

float  normal[6][3] = { {0.0,0.0,-1.0}, {0.0,-1.0,0.0}, {1.0,0.0,0.0},
					 {0.0,0.0,1.0}, {0.0,1.0,0.0}, {-1.0,0.0,0.0} };



/*-Declare GLU quadric objects, sphere, cylinder, and disk --*/
GLUquadricObj  *sphere = NULL, *cylind = NULL, *disk;


/*-Declare car position, orientation--*/
float  self_ang = -90.0, glob_ang = 0.0;
float  position[3] = { 8.0, 0.0, 0.0 };
float  fovy;
float orth;
/*-----Define window size----*/
int width = 600, height = 600;
float px, py, pz;
float left_arm_shoderTheta1, left_arm_shoderTheta2, right_arm_shoderTheta2, right_arm_shoderTheta1;
float left_arm_frontTheta, right_arm_frontTheta;
float rightleg_theta, leftleg_theta, rightfeet_theta, leftfeet_theta;
float diraction;
float k = 0.1;
float v;
int timer;
int number_object = 4;

/*-----Translation and rotations of eye coordinate system---*/
float   eyeDx = 0.0, eyeDy = 0.0, eyeDz = 0.0;
float   eyeAngx = 0.0, eyeAngy = 0.0, eyeAngz = 0.0;
double  Eye[3] = { 0.0, 30.0, 30.0 }, Focus[3] = { 0.0, 0.0, 0.0 },
Vup[3] = { 0.0, 1.0, 0.0 };


float   u[3][3] = { {1.0,0.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0} };
float   eye[3];
float   cv, sv; /* cos(5.0) and sin(5.0) */
float   znf[2];
float   view_point[8][3];
int		node_order[12][2] = { {0,1}, {1,2}, {2,3}, {3,0}, {4, 5}, {5, 6}, {6,7}, {7,4}, {0, 4}, {1, 5}, {2, 6}, {3,7} };

/*-----Drawing stye
 0:4-windows, 1:x direction, 2:y direction, 3:z-dirtection, 4:perspective
 */
int style = 0;
int form = 0;
int show_global;



/*----Define material properties for cube -----*/
float  mat_ambient[] = { 0.1, 0.3, 0.1, 1.0 };
float  mat_diffuse[] = { 0.1, 0.9, 0.1, 1.0 };
float  plastic_specular[] = { 0.4, 0.4, 0.4, 1.0 };
float  metal_specular[] = { 0.9, 0.9, 0.9, 1.0 };

float  mat_shininess = 64.0;

float  flr_diffuse[] = { 0.60, 0.60, 0.60, 1.0 };
float  flr_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
float  flr_specular[] = { 0.0, 0.0, 0.0, 1.0 };

/*----Define light source properties -------*/
float  lit0_position[4] = { 10.0, 10.0, 10.0, 0.0 };
float  lit1_position[4] = { 10.0, 14.0, 0.0, 1.0 };
float  lit2_position[4];
float  lit2_direction[4];

float  lit_diffuse[4] = { 0.8, 0.4, 0.4, 1.0 };
float  lit_specular[4] = { 0.7, 0.7, 0.7, 1.0 };
float  global_ambient[4] = { 0.2, 0.2, 0.2, 1.0 };
float  lit_cutoff = 60.0;
float  lit_exponent = 8.0;

float gold_diff[] = {0.75, 0.60, 0.22, 1.0};
float gold_spec[] = {0.62, 0.55, 0.36, 1.0};
float gold_amb[] = { 0.24, 0.2, 0.07, 1.0 };
float gold_shine = 0.4;

float plastic_diff[] = {0.55, 0.55, 0.55, 1.0 };
float plastic_spec[] = {0.7, 0.7, 0.7, 1.0 };
float plastic_amb[] = { 0.0, 0.0, 0.0, 1.0 };
float plastic_shine = 0.25;

float silver_diff[] = {0.5, 0.5, 0.5, 1.0 };
float silver_spec[] = {0.5, 0.5, 0.5, 1.0 };
float silver_amb[] = { 0.2, 0.2, 0.2, 1.0 };
float silver_shine = 0.4;



typedef struct objects {
	int type;
	float x;
	float z;
	float size_x;
	float size_y;
	float size_z;
} ob;

ob objectList[10];

void draw_scene(void);

int distance(float npx, float npz, ob object) {
	if (object.type == 1) {
		if (fabs(npx - object.x) > 1.0 + object.size_x / 2)return 1;
		if (fabs(npz - object.z) > 1.0 + object.size_z / 2)return 1;
	}
	else if (object.type == 2) {
		float d = (npx - object.x) * (npx - object.x) + (npz - object.z) * (npz - object.z);
		if (d > object.size_z / 2 * object.size_z / 2) return 1;
	}
	else if (object.type == 3) {
		if (fabs(npx - object.x) > object.size_x / 2)return 1;
		if (npz < object.z || npz - object.z > object.size_z / 2)return 1;
	}
	//fprintf(stderr, "type =%d npx  = %f npz = %f x = %f\n", object.type, fabs(npx - object.x), npx, object.x);
	return 0;

}

void setup_light0()  //set up directional light
{

	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, lit0_position);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lit_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lit_specular);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
}

void setup_light1()  //set up point light
{

	glEnable(GL_LIGHT1);
	//glLightfv(GL_LIGHT1, GL_POSITION, point_pos);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lit_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, lit_specular);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
}

void setup_light2()  //set up spot light
{

	glEnable(GL_LIGHT2);
	glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, lit_cutoff);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lit_diffuse);
	glLightfv(GL_LIGHT2, GL_SPECULAR, lit_specular);
	glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, lit_exponent);
}


/*----------------------------------------------------------
 * Procedure to initialize the working environment.
 */
void  myinit()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);      /*set the background color BLACK */
					 /*Clear the Depth & Color Buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);  /*Enable depth buffer for shading computing */
	glEnable(GL_NORMALIZE);   /*Enable mornalization  */

	glEnable(GL_LIGHTING);    /*Enable lighting effects */
	setup_light1();
	setup_light0();
	setup_light2();
	/*-----Define some global lighting status -----*/
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE); /* local viewer */
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient); /*global ambient*/

	/*-----Enable face culling -----*/
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	/*---Create quadratic objects---*/
	if (sphere == NULL) {
		sphere = gluNewQuadric();
		gluQuadricDrawStyle(sphere, GLU_FILL);
		gluQuadricNormals(sphere, GLU_SMOOTH);
	}
	if (cylind == NULL) {
		cylind = gluNewQuadric();
		gluQuadricDrawStyle(cylind, GLU_FILL);
		gluQuadricNormals(cylind, GLU_SMOOTH);
	}

	/*---- Compute cos(5.0) and sin(5.0) ----*/
	cv = cos(5.0*PI / 180.0);
	sv = sin(5.0*PI / 180.0);
	/*---- Copy eye position ---*/
	eye[0] = Eye[0];
	eye[1] = Eye[1];
	eye[2] = Eye[2];
	fovy = 90.0;
	orth = 40.0;
	float y[3], z[3];
	for (int j = 0; j < 9; j++) {
		y[0] = u[1][0] * cv - u[2][0] * sv;
		y[1] = u[1][1] * cv - u[2][1] * sv;
		y[2] = u[1][2] * cv - u[2][2] * sv;

		z[0] = u[2][0] * cv + u[1][0] * sv;
		z[1] = u[2][1] * cv + u[1][1] * sv;
		z[2] = u[2][2] * cv + u[1][2] * sv;

		for (int i = 0; i < 3; i++) {
			u[1][i] = y[i];
			u[2][i] = z[i];
		}
	}
	left_arm_shoderTheta2 = 0.0; left_arm_shoderTheta1 = 90.0;
	right_arm_shoderTheta2 = 0.0; right_arm_shoderTheta1 = 90.0;
	left_arm_frontTheta = 0.0; right_arm_frontTheta = 0.0;
	py = 4.0;
	px = 5.0;
	pz = 5.0;
	rightleg_theta = 90.0; leftleg_theta = 90.0;
	rightfeet_theta = 0.0; leftfeet_theta = 0.0;
	znf[1] = 100.0f;
	znf[0] = 1.5f;
	timer = 0;
	show_global = 0;
	style = 4;
}


void draw_sphere(float radius, int m) {
	if (m == GOLD) {
		glEnable(GL_COLOR_MATERIAL);
		glMaterialf(GL_FRONT, GL_SHININESS, gold_shine*128);
		glMaterialfv(GL_FRONT, GL_SPECULAR, gold_spec);
		glMaterialfv(GL_FRONT, GL_AMBIENT, gold_amb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, gold_diff);
	}
	else if (m == SILVER) {
		glEnable(GL_COLOR_MATERIAL);
		glMaterialf(GL_FRONT, GL_SHININESS, silver_shine * 128);
		glMaterialfv(GL_FRONT, GL_SPECULAR, silver_spec);
		glMaterialfv(GL_FRONT, GL_AMBIENT, silver_amb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, silver_diff);
	}
	else if (m == PLASTIC) {
		glEnable(GL_COLOR_MATERIAL);
		glMaterialf(GL_FRONT, GL_SHININESS, plastic_shine * 128);
		glMaterialfv(GL_FRONT, GL_SPECULAR, plastic_spec);
		glMaterialfv(GL_FRONT, GL_AMBIENT, plastic_amb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, plastic_diff);
	}
	gluSphere(sphere, radius, 32, 32);
}

void draw_cylin(float radius, float hight, int m) {
	if (m == GOLD) {
		glEnable(GL_COLOR_MATERIAL);
		glMaterialf(GL_FRONT, GL_SHININESS, gold_shine * 128);
		glMaterialfv(GL_FRONT, GL_SPECULAR, gold_spec);
		glMaterialfv(GL_FRONT, GL_AMBIENT, gold_amb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, gold_diff);
	}
	else if (m == SILVER) {
		glEnable(GL_COLOR_MATERIAL);
		glMaterialf(GL_FRONT, GL_SHININESS, silver_shine * 128);
		glMaterialfv(GL_FRONT, GL_SPECULAR, silver_spec);
		glMaterialfv(GL_FRONT, GL_AMBIENT, silver_amb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, silver_diff);
	}
	else if (m == PLASTIC) {
		glEnable(GL_COLOR_MATERIAL);
		glMaterialf(GL_FRONT, GL_SHININESS, plastic_shine * 128);
		glMaterialfv(GL_FRONT, GL_SPECULAR, plastic_spec);
		glMaterialfv(GL_FRONT, GL_AMBIENT, plastic_amb);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, plastic_diff);
	}
	gluCylinder(cylind, radius, radius, hight, 32, 3);
}

/*--------------------------------------------------------
 * Procedure to draw a 1x1x1 cube. The cube is within
 * (-0.5,-0.5,-0.5) ~ (0.5,0.5,0.5)
 */
void draw_cube_metal()
{
	float  cube_color[] = { 0.20, 0.75, 0.0, 1.0 };
	float  white[] = { 0.95, 0.95, 0.95, 1.0 };

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_SPECULAR);
	glColor3fv(white);
	glMaterialf(GL_FRONT, GL_SHININESS, 6.0);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glColor3fv(cube_color);

	int    i;
	for (i = 0; i < 6; i++) {
		//glColor3fv(colors[i]);  /* Set color */
		glNormal3fv(normal[i]);
		glBegin(GL_POLYGON);  /* Draw the face */
		glVertex3fv(points[face[i][0]]);
		glVertex3fv(points[face[i][1]]);
		glVertex3fv(points[face[i][2]]);
		glVertex3fv(points[face[i][3]]);
		glEnd();
	}
}

void draw_cube_plastic()
{
	float  cube_color[] = { 0.20, 0.75, 0.0, 1.0 };
	float  specular[] = { 0.3, 0.3, 0.3, 1.0 };

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_SPECULAR);
	glColor3fv(specular);
	glMaterialf(GL_FRONT, GL_SHININESS, 6.0);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glColor3fv(cube_color);

	int    i;
	for (i = 0; i < 6; i++) {
		//glColor3fv(colors[i]);  /* Set color */
		glNormal3fv(normal[i]);
		glBegin(GL_POLYGON);  /* Draw the face */
		glVertex3fv(points[face[i][0]]);
		glVertex3fv(points[face[i][1]]);
		glVertex3fv(points[face[i][2]]);
		glVertex3fv(points[face[i][3]]);
		glEnd();
	}
}

void draw_object(const ob object) {
	glPushMatrix();
	glTranslatef(object.x, 0.0, object.z);

	if (object.type == 1) {
		//glColor3f(0.20, 0.6, 0.9);
		glScalef(object.size_x, object.size_y, object.size_z);
		draw_cube_plastic();
	}
	else if (object.type == 2) {
		glColor3f(0.20, 0.1, 0.9);
		if (sphere == NULL) {
			sphere = gluNewQuadric();
			gluQuadricDrawStyle(sphere, GLU_FILL);
		}
		gluSphere(sphere, object.size_z / 2, 12, 12);
	}
	else if (object.type == 3) {
		if (cylind == NULL) {
			cylind = gluNewQuadric();
			gluQuadricDrawStyle(cylind, GLU_FILL);
			gluQuadricNormals(cylind, GLU_SMOOTH);
		}
		glColor3f(0.7, 0.4, 0.1);
		gluCylinder(cylind, object.size_x / 2, object.size_x / 2, object.size_z / 2, 12, 3);
	}
	glPopMatrix();
}

/*---------------------------------------------------------
 * Procedure to draw the floor.
 */
void draw_floor()
{
	float  flr_color_black[] = { 0.8, 0.8, 0.8, 1.0 };
	float  flr_color_white[] = { 0.2, 0.2, 0.4, 1.0 };
	int  i, j;

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glNormal3f(0.0, 1.0, 0.0);
	for (i = 0; i < 10; i++)
		for (j = 0; j < 10; j++) {
			if ((i + j) % 2 == 0) glColor3fv(flr_color_black);
			else glColor3fv(flr_color_white);
			glBegin(GL_POLYGON);
			glVertex3f((i - 5.0)*10.0, -2, (j - 5.0)*10.0);
			glVertex3f((i - 5.0)*10.0, -2, (j - 4.0)*10.0);
			glVertex3f((i - 4.0)*10.0, -2, (j - 4.0)*10.0);
			glVertex3f((i - 4.0)*10.0, -2, (j - 5.0)*10.0);
			glEnd();
		}
	glDisable(GL_COLOR_MATERIAL);
}


/*-------------------------------------------------------
 * Make viewing matrix
 */
void make_view(int x)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	switch (x) {
	case 4:       /* Perspective */

	  /* In this sample program, eye position and Xe, Ye, Ze are computed
		 by ourselves. Therefore, use them directly; no transform is
		 applied upon eye coordinate system
		 */
		gluLookAt(eye[0], eye[1], eye[2],
			eye[0] - u[2][0], eye[1] - u[2][1], eye[2] - u[2][2],
			u[1][0], u[1][1], u[1][2]);
		break;

	case 1:       /* X direction parallel viewing */
		gluLookAt(70.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		break;
	case 2:       /* Y direction parallel viewing */
		gluLookAt(0.0, 70.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0);
		break;
	case 3:
		gluLookAt(0.0, 0.0, 70.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		break;
	}
}

/*------------------------------------------------------
 * Procedure to make projection matrix
 */
void make_projection(int x)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (x == 4) {
		gluPerspective(fovy, (double)width / (double)height, znf[0], znf[1]);
	}
	else {
		if (width > height)
			glOrtho(-orth, orth, -orth * (float)height / (float)width,
				orth*(float)height / (float)width,
				-0.0, 100.0);
		else
			glOrtho(-orth*(float)width / (float)height,
				orth*(float)width / (float)height, -orth, orth,
				-0.0, 100.0);
	}
	glMatrixMode(GL_MODELVIEW);
}


/*---------------------------------------------------------
 * Procedure to draw view volume, eye position, focus ,...
 * for perspective projection
 */
void draw_view()
{
	int    i;

	glMatrixMode(GL_MODELVIEW);

	/*----Draw Eye position-----*/
	glPushMatrix();
	glTranslatef(eye[0], eye[1], eye[2]);
	glColor3f(0.0, 1.0, 0.0);
	glutWireSphere(1.0, 10, 10);
	glPopMatrix();

	/*----Draw eye coord. axes -----*/
	glColor3f(1.0, 1.0, 0.0);           /* Draw Xe */
	glBegin(GL_LINES);
	glVertex3f(eye[0], eye[1], eye[2]);
	glVertex3f(eye[0] + 20.0*u[0][0], eye[1] + 20.0*u[0][1], eye[2] + 20.0*u[0][2]);
	glEnd();

	glColor3f(1.0, 0.0, 1.0);          /* Draw Ye */
	glBegin(GL_LINES);
	glVertex3f(eye[0], eye[1], eye[2]);
	glVertex3f(eye[0] + 20.0*u[1][0], eye[1] + 20.0*u[1][1], eye[2] + 20.0*u[1][2]);
	glEnd();

	glColor3f(0.0, 1.0, 1.0);          /* Draw Ze */
	glBegin(GL_LINES);
	glVertex3f(eye[0], eye[1], eye[2]);
	glVertex3f(eye[0] + 20.0*u[2][0], eye[1] + 20.0*u[2][1], eye[2] + 20.0*u[2][2]);
	glEnd();

	float tan_theta;
	tan_theta = tan(fovy*PI / 360.0);
	for (int i = 0; i < 2; i++) {
		for (int k = 0; k < 3; k++) {
			view_point[4*i][k] = eye[k] - znf[i] * u[0][k] * tan_theta + znf[i] * u[1][k] * tan_theta - znf[i] * u[2][k];
			view_point[4*i+1][k] = eye[k] + znf[i] * u[0][k] * tan_theta + znf[i] * u[1][k] * tan_theta - znf[i] * u[2][k];
			view_point[4*i+2][k] = eye[k] + znf[i] * u[0][k] * tan_theta - znf[i] * u[1][k] * tan_theta - znf[i] * u[2][k];
			view_point[4*i+3][k] = eye[k] - znf[i] * u[0][k] * tan_theta - znf[i] * u[1][k] * tan_theta - znf[i] * u[2][k];
		}
	}

	for (int i = 0; i < 12; i++) {
		glColor3f(1.0, 1.0, 1.0);          
		glBegin(GL_LINES);
		glVertex3f(view_point[node_order[i][0]][0], view_point[node_order[i][0]][1], view_point[node_order[i][0]][2]);
		glVertex3f(view_point[node_order[i][1]][0], view_point[node_order[i][1]][1], view_point[node_order[i][1]][2]);
		glEnd();
	}

}

void draw_light(){
	
}


/*--------------------------------------------------------
 * Display callback procedure to draw the scene
 */
void display()
{
	/*Clear previous frame and the depth buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	switch (style) {
	case 0:
		make_view(4);
		make_projection(4);
		glViewport(width / 2, 0, width / 2, height / 2);
		draw_scene();

		make_view(1);
		make_projection(1);
		glViewport(0, height / 2, width / 2, height / 2);
		//draw light
		draw_scene();
		draw_view();

		make_view(2);
		glViewport(width / 2, height / 2, width / 2, height / 2);
		draw_scene();
		draw_view();

		make_view(3);
		glViewport(0, 0, width / 2, height / 2);
		draw_scene();
		draw_view();
		break;
	case 1:
		make_view(1);
		make_projection(1);
		glViewport(0, 0, width, height);
		draw_scene();
		draw_view();
		break;
	case 2:
		make_view(2);
		make_projection(2);
		glViewport(0, 0, width, height);
		draw_scene();
		
		draw_view();
		break;
	case 3:
		make_view(3);
		make_projection(3);
		glViewport(0, 0, width, height);
		draw_scene();
		draw_view();
		break;
	case 4:
		make_view(4);
		make_projection(4);
		glViewport(0, 0, width, height);
		draw_scene(); 
		draw_view();
		break;
	}
	/*-------Swap the back buffer to the front --------*/
	glutSwapBuffers();
	return;

}

/*-------------------------------------------------------
 * This procedure draw the car
 */
void draw_scene()
{
	draw_light();

	draw_floor();

	for (int i = 0; i < number_object; i++) {
		draw_object(objectList[i]);
	}
	
	/* draw spot light*/

	/* draw robot */
	glTranslatef(px, py, pz);
	if (diraction < 0) diraction = 360.0;
	if (diraction > 360.0) diraction = 0.0;
	glRotatef(diraction, 0.0, 1.0, 0.0);

	glTranslatef(0.0, 2.0, 0.0);

	glPushMatrix();
	glScalef(2.0, 4.0, 1.5);
	//glColor3f(0.20, 0.75, 0.0);  /* Set the current color */
	draw_cube_metal();
	glPopMatrix();

	glTranslatef(1.3, 1.7, 0.0);
	glPushMatrix();

	draw_sphere(0.3f, GOLD);

	glRotatef(left_arm_shoderTheta1, 1.0, 0.0, 0.0); // 0~360 
	glRotatef(left_arm_shoderTheta2, 0.0, 1.0, 0.0); // 0 ~ 90
	
	draw_cylin(0.25, 2.0, SILVER);
	//glColor3f(0.95, 0.2, 0.1);
	//gluCylinder(cylind, 0.25, 0.25, 2.0, 12, 3); //leftarm

	glTranslatef(0.0, 0.0, 2.0);
	draw_sphere(0.3f, GOLD);
	//glColor3f(0.68, 0.68, 0.68);
	//gluSphere(sphere, 0.3, 12, 12); //left joint 2

	glRotatef(-left_arm_frontTheta, 1.0, 0.0, 0.0);
	draw_cylin(0.25, 1.5, SILVER);
	//glColor3f(0.2, 0.1, 0.95);
	//gluCylinder(cylind, 0.25, 0.25, 1.5, 12, 3); //left forarm

	glTranslatef(0.0, 0.0, 1.5);
	draw_sphere(0.3f, GOLD);
	//glColor3f(0.68, 0.68, 0.68);
	//gluSphere(sphere, 0.3, 12, 12);   //left joint3  
	glPopMatrix();
	//leftarm;

	glTranslatef(-2.6, 0.0, 0.0);
	glPushMatrix();

	draw_sphere(0.3f, GOLD);
	//glColor3f(0.68, 0.68, 0.68);
	//gluSphere(sphere, 0.3, 12, 12); //right arm joint 1

	glRotatef(right_arm_shoderTheta1, 1.0, 0.0, 0.0); // 360
	glRotatef(-right_arm_shoderTheta2, 0.0, 1.0, 0.0); // 90
	draw_cylin(0.25, 2.0, SILVER);
    //glColor3f(0.95, 0.2, 0.1);
	//gluCylinder(cylind, 0.25, 0.25, 2.0, 12, 3); // right arm

	glTranslatef(0.0, 0.0, 2.0);
	draw_sphere(0.3f, GOLD);
	//glColor3f(0.68, 0.68, 0.68);
	//gluSphere(sphere, 0.3, 12, 12);  //right arm joint 2


	glRotatef(-right_arm_frontTheta, 1.0, 0.0, 0.0);
	draw_cylin(0.25, 1.5, SILVER);
	//glColor3f(0.2, 0.1, 0.95);
	//gluCylinder(cylind, 0.25, 0.25, 1.5, 12, 3);  // right forarm 

	glTranslatef(0.0, 0.0, 1.5);
	draw_sphere(0.3f, GOLD);
	//glColor3f(0.68, 0.68, 0.68);
	//gluSphere(sphere, 0.3, 12, 12);  //right arm joint 3
	glPopMatrix();
	//rightarm;

	glTranslatef(1.3, 0.8, 0.0);
	draw_sphere(0.8f, GOLD);
	//glColor3f(0.95, 0.2, 0.1);
	//gluSphere(sphere, 0.8, 12, 12); //head


	glTranslatef(0.6, -4.5, 0.0);
	draw_sphere(0.3f, GOLD);
	//glColor3f(0.68, 0.68, 0.68);
	//gluSphere(sphere, 0.4, 12, 12); //left leg joint 1 
	//leftleg
	glPushMatrix();
	glRotatef(leftleg_theta, 1.0, 0.0, 0.0);
	//leftleg_theta is more then 90
	draw_cylin(0.35, 2.0, SILVER);
	//glColor3f(0.95, 0.2, 0.1);
	//gluCylinder(cylind, 0.35, 0.35, 2.0, 12, 3); //left leg

	glTranslatef(0.0, 0.0, 2.0);
	draw_sphere(0.4f, GOLD);
	//glColor3f(0.68, 0.68, 0.68);
	//gluSphere(sphere, 0.4, 12, 12); //left leg joint 2 

	glRotatef(leftfeet_theta, 1.0, 0.0, 0.0);
	draw_cylin(0.35, 2.0, SILVER);
	//glColor3f(0.2, 0.1, 0.95);
	//gluCylinder(cylind, 0.35, 0.35, 2.0, 12, 3); // left feet

	glTranslatef(0.0, 0.0, 2.0);
	draw_sphere(0.4f, GOLD);
	//glColor3f(0.68, 0.68, 0.68);
	//gluSphere(sphere, 0.4, 12, 12);   //left leg joint3    
	glPopMatrix();


	glTranslatef(-1.2, 0.0, 0.0);
	draw_sphere(0.4f, GOLD);
	//glColor3f(0.68, 0.68, 0.68);
	//gluSphere(sphere, 0.4, 12, 12);  //right leg join1 

	glRotatef(rightleg_theta, 1.0, 0.0, 0.0);
	draw_cylin(0.35, 2.0, SILVER);
	//glColor3f(0.95, 0.2, 0.1);
	//gluCylinder(cylind, 0.35, 0.35, 2.0, 12, 3);//right leg

	glTranslatef(0.0, 0.0, 2.0);
	draw_sphere(0.4f, GOLD);
	//glColor3f(0.68, 0.68, 0.68);
	//gluSphere(sphere, 0.4, 12, 12); //right leg joint 2 

	//rightfeet
	glRotatef(rightfeet_theta, 1.0, 0.0, 0.0);
	draw_cylin(0.35, 2.0, SILVER);
	//glColor3f(0.2, 0.1, 0.95);
	//gluCylinder(cylind, 0.35, 0.35, 2.0, 12, 3); // right feet

	glTranslatef(0.0, 0.0, 2.0);
	draw_sphere(0.4f, GOLD);
	//glColor3f(0.68, 0.68, 0.68);
	//gluSphere(sphere, 0.4, 12, 12);  //right leg joint 3



}

void init_robot() {
	if (form == JUMP) {
		timer = 0;
		left_arm_shoderTheta2 = 0.0; left_arm_shoderTheta1 = 90.0;
		right_arm_shoderTheta2 = 0.0; right_arm_shoderTheta1 = 90.0;
		left_arm_frontTheta = 0.0; right_arm_frontTheta = 0.0;
		rightleg_theta = 30.0; leftleg_theta = 30.0;
		rightfeet_theta = 120.0; leftfeet_theta = 120.0;
		py = 2.0;
	}
	else {
		timer = 0;
		left_arm_shoderTheta2 = 0.0; left_arm_shoderTheta1 = 90.0;
		right_arm_shoderTheta2 = 0.0; right_arm_shoderTheta1 = 90.0;
		left_arm_frontTheta = 0.0; right_arm_frontTheta = 0.0;
		py = 4.0;
		rightleg_theta = 90.0; leftleg_theta = 90.0;
		rightfeet_theta = 0.0; leftfeet_theta = 0.0;
	}

}
/*--------------------------------------------------
 * Reshape callback function which defines the size
 * of the main window when a reshape event occurrs.
 */
void my_reshape(int w, int h)
{

	width = w;
	height = h;

	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0, (double)w / (double)h, 1.5, 50.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void idle_func()
{
	int dontmove;
	float npz, npx;
	switch (form) {
	case STOP:
		break;
	case WALK:
		left_arm_frontTheta = 90.0;
		right_arm_frontTheta = 90.0;

		if (left_arm_shoderTheta1 > 120.0) {
			k = -0.1;
		}
		if (left_arm_shoderTheta1 < 60.0) {
			k = 0.1;
		}
		left_arm_shoderTheta1 += k;
		right_arm_shoderTheta1 += -k;
		leftleg_theta += -k;
		rightleg_theta += k;
		if (left_arm_shoderTheta1 > 90) {
			leftfeet_theta += k;
		}
		if (right_arm_shoderTheta1 > 90) {
			rightfeet_theta += -k;
		}
		dontmove = 0;
		npz = pz + 0.01 * cos(diraction * PI / 180.0);
		npx = px + 0.01 * sin(diraction * PI / 180.0);
		for (int i = 0; i < number_object; i++) {
			if (!distance(npx, npz, objectList[i])) {
				dontmove = 1;
			}
		}
		if (!dontmove) {
			pz += 0.01 * cos(diraction * PI / 180.0);
			px += 0.01 * sin(diraction * PI / 180.0);
			if (pz > 50) pz = -50;
			if (px > 50) px = -50;
			if (pz < -50) pz = 50;
			if (px < -50) px = 50;
		}
		
		break;
	case SWINGHAND:

		if (timer == 1800) timer = 0;
		if (timer >= 1350) {
			left_arm_shoderTheta2 += -0.4;
			right_arm_shoderTheta2 += -0.4;
		}
		else if (timer >= 450 && timer < 900) {
			left_arm_shoderTheta2 += 0.4;
			right_arm_shoderTheta2 += 0.4;
		}
		timer++;
		break;
	case JUMP:

		if (timer == 600) timer = 0;
		if (timer == 0) {
			v = 0.03;
			py = 2.0;
		}
		v -= 0.0001;
		py += v;

		if (timer >= 450) {
			leftleg_theta += -0.4;
			rightleg_theta += -0.4;
			leftfeet_theta += 0.8;
			rightfeet_theta += 0.8;
			left_arm_shoderTheta1 += 1.33;
			right_arm_shoderTheta1 += 1.33;
		}
		else if (timer >= 0 && timer < 150) {
			leftleg_theta += 0.4;
			rightleg_theta += 0.4;
			leftfeet_theta += -0.8;
			rightfeet_theta += -0.8;
			left_arm_shoderTheta1 += -1.33;
			right_arm_shoderTheta1 += -1.33;
		}
		timer++;
		break;
	default:
		myinit();
		break;
	}
	display();
}

void printusermaue() {
	fprintf(stderr, " Q quit\n");
	fprintf(stderr, " w forward\n");
	fprintf(stderr, " a move left\n");
	fprintf(stderr, " s backward\n");
	fprintf(stderr, " d move right\n");
	fprintf(stderr, " e up E down\n");
	fprintf(stderr, " Y change robot form");
	fprintf(stderr, " T trun robot\n");
	fprintf(stderr, " F pitch\n");
	fprintf(stderr, " H yaw\n");
	fprintf(stderr, " G roll\n");
	fprintf(stderr, " Z ortho zoom in and out\n");
	fprintf(stderr, " X Perspective zoom in and out\n");
	fprintf(stderr, " U mvoe near cliping\n");
	fprintf(stderr, " I mvoe far cliping\n");
	fprintf(stderr, " C show or unshow global ase\n");
	fprintf(stderr, " P show UserManual\n");
}

/*--------------------------------------------------
 * Keyboard callback func. When a 'q' key is pressed,
 * the program is aborted.
 */
void my_quit(unsigned char key, int ix, int iy)
{
	int    i;
	float  x[3], y[3], z[3];

	if (key == 'Q' || key == 'q') exit(0);
	if (key == 'u') {                        /* move car back b-key */
		znf[0] += 1.0;
		if (znf[1] - znf[0] < 10) znf[0] = znf[1] - 10.0f;
	}
	else if (key == 'U') {                        /* move car back b-key */
		znf[0] -= 1.0f;
		if (znf[0] <= 0.0) znf[0] = 1.0f;
	}
	else if (key == 'I') {                        /* move car back b-key */
		znf[1] += 1.0f;
		if (znf[1] > 150.0f) znf[1] = 150.0f;
	}
	else if (key == 'i') {                        /* move car back b-key */
		znf[1] -= 1.0f;
		if (znf[1] - znf[0] < 10) znf[1] = znf[0] + 10.0f;
	}
	else if (key == 'T') {                        /* make a left turn */
		diraction += 10.0;
		if (diraction > 360.0) diraction -= 360.0;
	}
	else if (key == 't') {                        /* make a right turn */
		diraction += -10.0;
		if (diraction < 0.0) diraction += 360.0;

		/*------transform the EYE coordinate system ------*/
	}
	else if (key == 'e') {
		eyeDy += 0.5;       /* move up */
		for (i = 0; i < 3; i++) eye[i] -= 0.5*u[1][i];
		//fprintf(stderr, "eye[0] = %f eye[1] = %f eye[2] = %f \n", eye[0], eye[1], eye[2]);
		
	}
	else if (key == 'E') {
		eyeDy += -0.5;       /* move down */
		for (i = 0; i < 3; i++) eye[i] += 0.5*u[1][i];
	}
	else if (key == 'a') {
		eyeDx += -0.5;       /* move left */
		for (i = 0; i < 3; i++) eye[i] += 0.5*u[0][i];
	}
	else if (key == 'd') {
		eyeDx += 0.5;        /* move right */
		for (i = 0; i < 3; i++) eye[i] -= 0.5*u[0][i];
	}
	else if (key == 'w') {
		eyeDz += 0.5;        /* zoom in */
		for (i = 0; i < 3; i++) eye[i] -= 0.5*u[2][i];
	}
	else if (key == 's') {
		eyeDz += -0.5;       /* zoom out */
		for (i = 0; i < 3; i++) eye[i] += 0.5*u[2][i];
	}
	else if (key == 'f') {             /* pitching */
		eyeAngx += 5.0;
		if (eyeAngx > 360.0) eyeAngx -= 360.0;
		y[0] = u[1][0] * cv - u[2][0] * sv;
		y[1] = u[1][1] * cv - u[2][1] * sv;
		y[2] = u[1][2] * cv - u[2][2] * sv;

		z[0] = u[2][0] * cv + u[1][0] * sv;
		z[1] = u[2][1] * cv + u[1][1] * sv;
		z[2] = u[2][2] * cv + u[1][2] * sv;

		for (i = 0; i < 3; i++) {
			u[1][i] = y[i];
			u[2][i] = z[i];
		}
	}
	else if (key == 'F') {
		eyeAngx += -5.0;
		if (eyeAngx < 0.0) eyeAngx += 360.0;
		y[0] = u[1][0] * cv + u[2][0] * sv;
		y[1] = u[1][1] * cv + u[2][1] * sv;
		y[2] = u[1][2] * cv + u[2][2] * sv;

		z[0] = u[2][0] * cv - u[1][0] * sv;
		z[1] = u[2][1] * cv - u[1][1] * sv;
		z[2] = u[2][2] * cv - u[1][2] * sv;

		for (i = 0; i < 3; i++) {
			u[1][i] = y[i];
			u[2][i] = z[i];
		}
	}
	else if (key == 'h') {            /* heading */
		eyeAngy += 5.0;
		if (eyeAngy > 360.0) eyeAngy -= 360.0;
		for (i = 0; i < 3; i++) {
			x[i] = cv * u[0][i] - sv * u[2][i];
			z[i] = sv * u[0][i] + cv * u[2][i];
		}
		for (i = 0; i < 3; i++) {
			u[0][i] = x[i];
			u[2][i] = z[i];
		}
	}
	else if (key == 'H') {
		eyeAngy += -5.0;
		if (eyeAngy < 0.0) eyeAngy += 360.0;
		for (i = 0; i < 3; i++) {
			x[i] = cv * u[0][i] + sv * u[2][i];
			z[i] = -sv * u[0][i] + cv * u[2][i];
		}
		for (i = 0; i < 3; i++) {
			u[0][i] = x[i];
			u[2][i] = z[i];
		}
	}
	else if (key == 'g') {            /* rolling */
		eyeAngz += 5.0;
		if (eyeAngz > 360.0) eyeAngz -= 360.0;
		for (i = 0; i < 3; i++) {
			x[i] = cv * u[0][i] - sv * u[1][i];
			y[i] = sv * u[0][i] + cv * u[1][i];
		}
		for (i = 0; i < 3; i++) {
			u[0][i] = x[i];
			u[1][i] = y[i];
		}
	}
	else if (key == 'G') {
		eyeAngz += -5.0;
		if (eyeAngz < 0.0) eyeAngz += 360.0;
		for (i = 0; i < 3; i++) {
			x[i] = cv * u[0][i] + sv * u[1][i];
			y[i] = -sv * u[0][i] + cv * u[1][i];
		}
		for (i = 0; i < 3; i++) {
			u[0][i] = x[i];
			u[1][i] = y[i];
		}
	}
	else if (key == 'r' || key == 'R') {
		style++;
		if (style == 5) {
			style = 0;
		}
	}
	else if (key == 'y' || key == 'Y') {
		form++;
		if (form > 3) {
			form = 0;
		}
		init_robot();
	}
	else if (key == 'Z' ) {
		orth += 5;
		if (orth > 50.0) orth = 50.0;
	}
	else if (key == 'z') {
		orth -= 5;
		if (orth < 30.0) orth = 30.0;
	}
	else if (key == 'X') {
		fovy += 5;
	    if (fovy > 90.0) fovy = 90.0;
	}
	else if (key == 'x') {
		fovy -= 5;
		if (fovy < 30.0) fovy = 30.0;
	}
	else if(key == 'C'|| key == 'c'){
		show_global = !show_global;
	}
	else if(key == 'P'|| key == 'p'){
		printusermaue();
	}
	/*
	for(int i =0;i<3;i++)
		fprintf(stderr, "u[%d][%d] = %f u[%d][%d] = %f u[%d][%d] = %f \n", i, 0,  u[i][0], i,1, u[i][1], i,2, u[i][2]);
	*/
	//fprintf(stderr, "eye[0] = %f eye[1] = %f eye[2] = %f \n", eye[0], eye[1], eye[2]);
	display();
}


void setobject() {
	objectList[0].type = 1;
	objectList[0].x = 30.0;
	objectList[0].z = 30.0;
	objectList[0].size_x = 6.0;
	objectList[0].size_y = 6.0;
	objectList[0].size_z = 6.0;
	objectList[1].type = 2;
	objectList[1].x = -10.0;
	objectList[1].z = 30.0;
	objectList[1].size_x = 3.0;
	objectList[1].size_y = 3.0;
	objectList[1].size_z = 3.0;
	objectList[2].type = 3;
	objectList[2].x = 40.0;
	objectList[2].z = -20.0;
	objectList[2].size_x = 3.0;
	objectList[2].size_y = 5.0;
	objectList[2].size_z = 15.0;
	objectList[3].type = 1;
	objectList[3].x = -10.0;
	objectList[3].z = -10.0;
	objectList[3].size_x = 10.0;
	objectList[3].size_y = 10.0;
	objectList[3].size_z = 10.0;
}

/*---------------------------------------------------
 * Main procedure which defines the graphics environment,
 * the operation style, and the callback func's.
 */
void main(int argc, char **argv)
{
	/*-----Initialize the GLUT environment-------*/
	glutInit(&argc, argv);

	/*-----Depth buffer is used, be careful !!!----*/
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(width, height);
	glutCreateWindow("robotScenes");

	myinit();      /*---Initialize other state varibales----*/
	setobject();
	/*----Associate callback func's whith events------*/
	glutDisplayFunc(display);
	/*  glutIdleFunc(display); */
	glutIdleFunc(idle_func);
	glutReshapeFunc(my_reshape);
	glutKeyboardFunc(my_quit);

	glutMainLoop();
}


