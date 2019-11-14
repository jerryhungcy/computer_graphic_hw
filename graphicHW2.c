
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define WALK 1
#define SWINGHAND 2
#define JUMP 3
#define STOP 4
#define RUN 5
#define CONTROL 6
#define PI 3.14159
/*-----Define a unit box--------*/
/* Vertices of the box */
float  points[][3] = {{-0.5, -0.5, -0.5}, {0.5, -0.5, -0.5}, 
                      {0.5, 0.5, -0.5}, {-0.5, 0.5, -0.5}, 
                      {-0.5, -0.5, 0.5}, {0.5, -0.5, 0.5},
                      {0.5, 0.5, 0.5}, {-0.5, 0.5, 0.5}};
/* face of box, each face composing of 4 vertices */
int    face[][4] = {{0, 3, 2, 1}, {0, 1, 5, 4}, {1, 2, 6, 5}, 
                    {4, 5, 6, 7}, {2, 3, 7, 6}, {0, 4, 7, 3}};
/* indices of the box faces */
int    cube[6] = {0, 1, 2, 3, 4, 5};

/*-----Define GLU quadric objects, a sphere and a cylinder----*/
GLUquadricObj  *sphere=NULL, *cylind=NULL;

/*-----Define window size----*/
int width=512, height=512;
int number_object= 4;

float px, py, pz;
float left_arm_shoderTheta1, left_arm_shoderTheta2, right_arm_shoderTheta2, right_arm_shoderTheta1;
float left_arm_frontTheta, right_arm_frontTheta;
float rightleg_theta, leftleg_theta, rightfeet_theta, leftfeet_theta;
float diraction;
float theta = 0.0;
float k = 0.1;
int idle_on, form;
int timer;
float v;
int zoomin;
typedef struct objects {
	int type;
	float x;
	float z;
	float size_x;
	float size_y;
	float size_z;
} ob;

ob objectList[10];

int distance(float npx, float npz, ob object) {
	if (object.type == 1) {
		if (fabs(npx - object.x) >  1.0 +object.size_x / 2)return 1;
		if (fabs(npz - object.z ) > 1.0 + object.size_z / 2)return 1;
	}
	else if (object.type == 2) {
		float d = (npx - object.x) * (npx - object.x) + (npz - object.z) * (npz - object.z);
		if (d > object.size_z / 2 * object.size_z / 2) return 1;
	}
	else if (object.type == 3) {
		if ( fabs(npx - object.x) > object.size_x / 2)return 1;
		if (npz < object.z || npz - object.z > object.size_z / 2)return 1;
	}
	//fprintf(stderr, "type =%d npx  = %f npz = %f x = %f\n", object.type, fabs(npx - object.x), npx, object.x);
	return 0;
	
}

/*--------------------------------------------------------
 * Procedure to draw a cube. The geometrical data of the cube
 * are defined above.
 */
void draw_cube()
{
  int    i;
  for(i=0;i<6;i++){
    glBegin(GL_POLYGON);  /* Draw the face */
      glVertex3fv(points[face[i][0]]);
      glVertex3fv(points[face[i][1]]);
      glVertex3fv(points[face[i][2]]);
      glVertex3fv(points[face[i][3]]);
    glEnd();
  }
}

void draw_floor()
{
	int   i, j;

	for (i = -9; i < 50; i+=3)
		for (j = -9; j < 50; j+=3) {
			if ((i + j) % 6 == 0) glColor3f(0.45, 0.45, 0.45);
			else glColor3f(0.1, 0.1, 0.1);
			glBegin(GL_POLYGON);
			glVertex3f(i, 0.0, j);
			glVertex3f(i, 0.0, j + 3);
			glVertex3f(i + 3, 0.0, j + 3);
			glVertex3f(i + 3, 0.0, j);
			glEnd();
		}
}

void draw_object(const ob object) {
	glPushMatrix();
	glTranslatef(object.x, 0.0, object.z);
	
	if (object.type == 1) {
		glColor3f(0.20, 0.6, 0.9);
		glScalef(object.size_x, object.size_y, object.size_z);
		draw_cube();
	}
	else if (object.type == 2) {
		glColor3f(0.20, 0.1, 0.9);
		if (sphere == NULL) {
			sphere = gluNewQuadric();
			gluQuadricDrawStyle(sphere, GLU_FILL);
		}
		gluSphere(sphere, object.size_z/2, 12, 12);
	}
	else if (object.type == 3) {
		if (cylind == NULL) {
			cylind = gluNewQuadric();
			gluQuadricDrawStyle(cylind, GLU_FILL);
			gluQuadricNormals(cylind, GLU_SMOOTH);
		}
		glColor3f(0.7, 0.4, 0.1);
		gluCylinder(cylind, object.size_x/2, object.size_x/2, object.size_z/2, 12, 3);
	}
	glPopMatrix();
}

void display(void)
{
  /*Clear previous frame and the depth buffer */
  glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);

  /*----Define the current eye position and the eye-coordinate system---*/
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();           /* Initialize modelview matrix */
  if (zoomin) {
	  gluLookAt(px + 20.0, py+20.0, pz+20.0, px, py, pz, 0.0, 1.0, 0.0);
  }
  else gluLookAt(35.0, 50.0, 45.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

  draw_floor();
  for (int i = 0; i < number_object; i++) {
	  draw_object(objectList[i]);
  }

  glTranslatef(px, py, pz );
  if (diraction < 0) diraction = 360.0;
  if (diraction > 360.0) diraction = 0.0;
  glRotatef(diraction, 0.0, 1.0, 0.0);
 
  glTranslatef( 0.0, 2.0, 0.0);
  
  glPushMatrix();
  glScalef( 2.0, 4.0, 1.5);
  glColor3f(0.20, 0.75, 0.0);  /* Set the current color */
  draw_cube();
  glPopMatrix();

  glTranslatef( 1.3, 1.7, 0.0);
  glPushMatrix();

  if(sphere==NULL){
    sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
  }
  glColor3f(0.68, 0.68, 0.68);
  gluSphere(sphere, 0.3, 12, 12);   //left joint 1

  glRotatef(left_arm_shoderTheta1, 1.0, 0.0, 0.0); // 0~360 
  glRotatef(left_arm_shoderTheta2, 0.0, 1.0, 0.0); // 0 ~ 90
  if(cylind==NULL){               
    cylind = gluNewQuadric();
    gluQuadricDrawStyle(cylind, GLU_FILL);
    gluQuadricNormals(cylind, GLU_SMOOTH);
  }
  glColor3f(0.95, 0.2, 0.1);  
  gluCylinder(cylind, 0.25, 0.25, 2.0, 12, 3); //leftarm

  glTranslatef(0.0, 0.0, 2.0);
  glColor3f(0.68, 0.68, 0.68);
  gluSphere(sphere, 0.3, 12, 12); //left joint 2

  glRotatef( -left_arm_frontTheta, 1.0, 0.0, 0.0);
  glColor3f(0.2, 0.1, 0.95);  
  gluCylinder(cylind, 0.25, 0.25, 1.5, 12, 3); //left forarm
  
  glTranslatef(0.0, 0.0, 1.5);
  glColor3f(0.68, 0.68, 0.68);
  gluSphere(sphere, 0.3, 12, 12);   //left joint3  
  glPopMatrix();
  //leftarm;
  
  glTranslatef(-2.6,0.0,0.0);
  glPushMatrix();

  
  glColor3f(0.68, 0.68, 0.68);
  gluSphere(sphere, 0.3, 12, 12); //right arm joint 1

  glRotatef(right_arm_shoderTheta1, 1.0, 0.0, 0.0); // 360
  glRotatef(-right_arm_shoderTheta2, 0.0, 1.0, 0.0); // 90
  glColor3f(0.95, 0.2, 0.1);  
  gluCylinder(cylind, 0.25, 0.25, 2.0, 12, 3); // right arm
  
  glTranslatef(0.0, 0.0, 2.0);
  glColor3f(0.68, 0.68, 0.68);
  gluSphere(sphere, 0.3, 12, 12);  //right arm joint 2


  glRotatef( -right_arm_frontTheta, 1.0, 0.0, 0.0);
  glColor3f(0.2, 0.1, 0.95);  
  gluCylinder(cylind, 0.25, 0.25, 1.5, 12, 3);  // right forarm 

  glTranslatef(0.0, 0.0, 1.5);
  glColor3f(0.68, 0.68, 0.68);
  gluSphere(sphere, 0.3, 12, 12);  //right arm joint 3
  glPopMatrix();
  //rightarm;
  
  glTranslatef( 1.3, 0.8, 0.0);
  glColor3f(0.95, 0.2, 0.1);
  gluSphere(sphere, 0.8, 12, 12); //head


  glTranslatef( 0.6, -4.5, 0.0);
  glColor3f(0.68, 0.68, 0.68);
  gluSphere(sphere, 0.4, 12, 12); //left leg joint 1 
  //leftleg
  glPushMatrix();
  glRotatef(leftleg_theta, 1.0, 0.0, 0.0);
  //leftleg_theta is more then 90
  glColor3f(0.95, 0.2, 0.1);
  gluCylinder(cylind, 0.35, 0.35, 2.0, 12, 3); //left leg
  
  glTranslatef( 0.0, 0.0, 2.0);
  glColor3f(0.68, 0.68, 0.68);
  gluSphere(sphere, 0.4, 12, 12); //left leg joint 2 

  glRotatef(leftfeet_theta, 1.0, 0.0, 0.0);
  glColor3f(0.2, 0.1, 0.95);
  gluCylinder(cylind, 0.35, 0.35, 2.0, 12, 3); // left feet
  
  glTranslatef(0.0, 0.0, 2.0);
  glColor3f(0.68, 0.68, 0.68);
  gluSphere(sphere, 0.4, 12, 12);   //left leg joint3    
  glPopMatrix();

  
  glTranslatef( -1.2, 0.0, 0.0);
  glColor3f(0.68, 0.68, 0.68);
  gluSphere(sphere, 0.4, 12, 12);  //right leg join1 

  glRotatef(rightleg_theta, 1.0, 0.0, 0.0); 
  glColor3f(0.95, 0.2, 0.1);
  gluCylinder(cylind, 0.35, 0.35, 2.0, 12, 3);//right leg

  glTranslatef( 0.0, 0.0, 2.0);
  glColor3f(0.68, 0.68, 0.68);
  gluSphere(sphere, 0.4, 12, 12); //right leg joint 2 

  //rightfeet
  glRotatef(rightfeet_theta, 1.0, 0.0, 0.0);
  glColor3f(0.2, 0.1, 0.95);
  gluCylinder(cylind, 0.35, 0.35, 2.0, 12, 3); // right feet

  glTranslatef(0.0, 0.0, 2.0);
  glColor3f(0.68, 0.68, 0.68);
  gluSphere(sphere, 0.4, 12, 12);  //right leg joint 3

  glutSwapBuffers();
  glFlush(); /*--Display the results----*/
}
void init_robot() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (zoomin) {
		glOrtho(-10.0, 10.0, -10.0, 10.0, 0.0, 120.0);
	}
	else glOrtho(-40.0, 40.0, -40.0, 40.0, 0.0, 120.0);

	left_arm_shoderTheta2 = 0.0; left_arm_shoderTheta1 = 90.0;
	right_arm_shoderTheta2 = 0.0; right_arm_shoderTheta1 = 90.0;
	left_arm_frontTheta = 0.0; right_arm_frontTheta = 0.0;
	py = 4.0;
	rightleg_theta = 90.0; leftleg_theta = 90.0;
	rightfeet_theta = 0.0; leftfeet_theta = 0.0;
}

void  myinit()
{
	glClearColor(0.0, 0.0, 0.0, 1.0);      /*set the background color BLACK */
					 /*Clear the Depth & Color Buffers */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, 499, 499);

	/*-----Set a parallel projection mode-----*/
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-40.0, 40.0, -40.0, 40.0, 0.0, 120.0);
	zoomin = 0;
	glEnable(GL_DEPTH_TEST);  /*Enable depth buffer for shading computing */
	px = 0.0; py = 4.0; pz = 0.0;
	init_robot();
	diraction = 0.0;
	timer = 0;
	display();
	glFlush();/*Enforce window system display the results*/
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
  if (zoomin) {
	  glOrtho(-10.0, 10.0, -10.0, 10.0, 0.0, 120.0);
  }
  else glOrtho(-40.0, 40.0, -40.0, 40.0, 0.0, 120.0);
}


/*--------------------------------------------------
 * Keyboard callback func. When a 'q' key is pressed,
 * the program is aborted.
 */
void my_keyboard(unsigned char key, int x, int y)
{ 
	  if (key == 'C' || key == 'c') {
		  form = CONTROL;
		  zoomin = 1;
		  init_robot();
	  }
	  if (key == 'd' || key == 'D') {
		  if (zoomin == 1) zoomin = 0;
		  else zoomin = 1;
		  init_robot();
	  }
	  if(key=='Q'||key=='q') exit(0);
	  if (key == 'W' || key == 'w') {
		  form = WALK;
		  init_robot();
	  }
	  if (key == 'T' || key == 't') {
		  form = SWINGHAND;
		  timer = 0;
		  init_robot();
	  }
	  if (key == 'E' || key == 'e') {
		  form = JUMP;
		  timer = 0;
		  left_arm_shoderTheta2 = 0.0; left_arm_shoderTheta1 = 90.0;
		  right_arm_shoderTheta2 = 0.0; right_arm_shoderTheta1 = 90.0;
		  left_arm_frontTheta = 0.0; right_arm_frontTheta = 0.0;
		  rightleg_theta = 30.0; leftleg_theta = 30.0;
		  rightfeet_theta = 120.0; leftfeet_theta = 120.0;
		  py = 2.0;
	  }
	  if (key == 'i' || key == 'I') diraction += 3.0;
	  if (key == 'o' || key == 'O') diraction -= 3.0;
	  if (key == 'P' || key == 'p') idle_on = !idle_on;
	  if (key == 'r' || key == 'R') {
		  form = RUN;
		  init_robot();
	  }
	  if (form == CONTROL) {
		  if (key == 'F' || key == 'f') {
			  left_arm_shoderTheta2 += 5.0;
			  if (left_arm_shoderTheta2 > 90) left_arm_shoderTheta2 = 90.0;
			}
		  if (key == 'v' || key == 'V') {
			  left_arm_shoderTheta2 -= 5.0;
			  if (left_arm_shoderTheta2 < 0) left_arm_shoderTheta2 = 0.0;
		  }
		  if (key == 'G' || key == 'g') {
			  left_arm_shoderTheta1 += 5.0;
			  if (left_arm_shoderTheta1 > 180) left_arm_shoderTheta1 = 180.0;
		  }
		  if (key == 'B' || key == 'b') {
			  left_arm_shoderTheta1 -= 5.0;
			  if (left_arm_shoderTheta1 < -120) left_arm_shoderTheta1 = -120.0;
		  }
		  if (key == 'h' || key == 'H') {
			  right_arm_shoderTheta2 += 5.0;
			  if (right_arm_shoderTheta2 > 90) right_arm_shoderTheta2 = 90.0;
		  }
		  if (key == 'N' || key == 'n') {
			  right_arm_shoderTheta2-=5.0;
			  if (right_arm_shoderTheta2 < 0) right_arm_shoderTheta2 = 0.0;
		  }
		  if (key == 'J' || key == 'j') {
			  right_arm_shoderTheta1+=5.0;
			  if (right_arm_shoderTheta1 > 180) right_arm_shoderTheta1 = 180.0;
		  }
		  if (key == 'a' || key == 'A') {
			  right_arm_frontTheta += 5.0;
			  if (right_arm_frontTheta > 90) right_arm_frontTheta = 90.0;
		  }
		  if (key == 'z' || key == 'Z') {
			  right_arm_frontTheta -= 5.0;
			  if (right_arm_frontTheta < 0.0) right_arm_frontTheta = 0.0;
		  }
		  if (key == 's' || key == 'S') {
			  left_arm_frontTheta += 5.0;
			  if (left_arm_frontTheta > 90) left_arm_frontTheta = 90.0;
		  }
		  if (key == 'x' || key == 'X') {
			  left_arm_frontTheta -= 5.0;
			  if (left_arm_frontTheta < 0.0) left_arm_frontTheta = 0.0;
		  }
		  if (key == 'm' || key == 'M') {
			  right_arm_shoderTheta1 -= 5.0;
			  if (right_arm_shoderTheta1 < -120) right_arm_shoderTheta1 = -120.0;
		  }

	  }
  //fprintf(stderr, "idle on %d %c\n",idle_on, key);
}


/*---------------------------------------------
 * Idle event callback function, increase blade and arm
 * rotation angles.
 */

void idle_func()
{
	int  dontmove;
	float npx, npz;
	if (!idle_on)return;
	
	switch (form) {
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
			if(right_arm_shoderTheta1 > 90) {
				rightfeet_theta += -k;
			}
			
			dontmove=0;
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
				if (pz > 50) pz = -10;
				if (px > 50) px = -10;
				if (pz < -10) pz = 50;
				if (px < -10) px = 50;
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
			}else if (timer >= 0 && timer < 150) {
				leftleg_theta += 0.4;
				rightleg_theta += 0.4;
				leftfeet_theta += -0.8;
				rightfeet_theta += -0.8;
				left_arm_shoderTheta1 += -1.33;
				right_arm_shoderTheta1 += -1.33;
			}
			timer++;
			break;
		case CONTROL:
			break;
		case RUN:
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
			leftfeet_theta += -k;
			rightleg_theta += k;
			dontmove = 0;
			npz = pz + 0.02 * cos(diraction * PI / 180.0);
			npx = px + 0.02 * sin(diraction * PI / 180.0);
			for (int i = 0; i < number_object; i++) {
				if (!distance(npx, npz, objectList[i])) {
					dontmove = 1;
				}
			}
			if (!dontmove) {
				pz += 0.02 * cos(diraction * PI / 180.0);
				px += 0.02 * sin(diraction * PI / 180.0);
				if (pz > 50) pz = -10;
				if (px > 50) px = -10;
				if (pz < -10) pz = 50;
				if (px < -10) px = 50;
			}
			break;
		default:
			myinit();
			break;
	}
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
	objectList[1].x = 10.0;
	objectList[1].z = 30.0;
	objectList[1].size_x = 3.0;
	objectList[1].size_y = 3.0;
	objectList[1].size_z = 3.0;
	objectList[2].type = 3;
	objectList[2].x = 40.0;
	objectList[2].z = 10.0;
	objectList[2].size_x = 3.0;
	objectList[2].size_y = 5.0;
	objectList[2].size_z = 15.0;
	objectList[3].type = 1;
	objectList[3].x = 10.0;
	objectList[3].z = 10.0;
	objectList[3].size_x = 6.0;
	objectList[3].size_y = 6.0;
	objectList[3].size_z = 6.0;
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
  glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGB| GLUT_DEPTH);
  glutInitWindowSize(width, height);
  glutCreateWindow("robot");

  myinit();      /*---Initialize other state varibales----*/
  idle_on = 1;
  timer = 0;
  setobject();
  /*----Associate callback func's whith events------*/
  glutDisplayFunc(display);
  glutIdleFunc(idle_func);    /* Add idle event */
  glutReshapeFunc(my_reshape);
  glutKeyboardFunc(my_keyboard);

  glutMainLoop();
}
