/*-----------------------------------------------------
 * This program to draw pictures
 * draw lines, polygon, circle
 * change color
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#define    MY_QUIT -1
#define    MY_CLEAR -2
#define    MY_SAVE  -3
#define    MY_BLEND -4
#define    MY_LOAD  -5
#define    GRID     -6


#define    WHITE   1
#define    RED     2
#define    GREEN   3
#define    BLUE    4
#define    BLACK   5
#define	   CONSOLE 6

#define    POINT   1
#define    LINE    2
#define    POLYGON 3
#define    CIRCLE  4
#define    CURVE   5
#define    TYPE    6

#define	   BACKGROUND 1
#define    PAINT   2



typedef    int   menu_t;
menu_t     top_m, color_m, file_m, type_m, size_menu, paint_m, background_m, font_m;

int        height = 800, width = 800;
unsigned char*  image;  /* Image data in main memory */
unsigned char*  background;
unsigned char*  under_line_image;
float      myColor[3] = { 0.0,0.0,0.0 };
float      backgroundColor[3] = { 0.0,0.0,0.0 };
int        obj_type = -1;
int        first = 0;      /* flag of initial points for lines and curve,..*/
int		   first_pos_x = -1, first_pos_y = -1;
int		   last_pos_x = -1, last_pos_y = -1;
int        vertex[128][2]; /*coords of vertices */
int        side = 0;         /*num of sides of polygon */
float      pnt_size = 3.0;
int		   right_bottom_value = 0;
int		   flag = 1;
int		   polygon_end = 0;
int        type_control = 0;
int		   font_type = 0;
char       string[200];
int		   len;
int        grid_line = 1;
int		   distance = 50;
void *fonts[] =
{
  GLUT_BITMAP_9_BY_15,
  GLUT_BITMAP_TIMES_ROMAN_10,
  GLUT_BITMAP_TIMES_ROMAN_24
};

/*

void PPMWriter(unsigned char *in, char *name, int dimx, int dimy)
{

	int i, j;
	FILE *fp = fopen(name, "wb"); //* b - binary mode 
	(void)fprintf(fp, "P6%d %d	255", dimx, dimy);
		for (j = 0; j < dimy; ++j)
		{
			for (i = 0; i < dimx; ++i)
			{
				static unsigned char color[3];
				color[0] = in[3 * i + 3 * j*dimy]; 
				color[1] = in[3 * i + 3 * j*dimy + 1];  
				color[2] = in[3 * i + 3 * j*dimy + 2];  
				(void)fwrite(color, 1, 3, fp);
			}
		}
	(void)fclose(fp);
}

void saveImage(float Ttime, int nbr)
{
	unsigned char* image = (unsigned char*)malloc(sizeof(unsigned char) * 3 * width * height);
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
	// Warning : enregistre de bas en haut
	char buffer[33];
	sprintf(buffer, "capture/%d SV-%f ms.ppm", nbr, Ttime);


	PPMWriter(image, buffer, 1024, 1024);
}
*/

void draw_polygon()
{
	int  i;
	glBegin(GL_POLYGON);
	for (i = 0; i < side; i++)
		glVertex2f(vertex[i][0], height - vertex[i][1]);
	glEnd();
	fprintf(stderr, "poly draw %d\n", side);
	glFinish();
	side = 0;    /* set side=0 for next polygon */
	polygon_end = 0;
	
}

void draw_circle() {
	first = 0;
	float r2 = (first_pos_x - last_pos_x) * (first_pos_x - last_pos_x) + (first_pos_y - last_pos_y) * (first_pos_y - last_pos_y);
	float r = sqrt(r2);
	/*
	glBegin(GL_LINE_LOOP);
	for (float i = 0.0; i < 3600; i++)
	{
		float theta = 2.0f * 3.1415926f * i / 3600.0f;//get the current angle

		float x = r * cosf(theta);//calculate the x component
		float y = r * sinf(theta);//calculate the y component

		glVertex2f(x + first_pos_x, height + y - first_pos_y);//output vertex

	}
	glEnd();
	glFinish();*/
	static GLUquadricObj *mycircle = NULL;

	if (mycircle == NULL) {
		mycircle = gluNewQuadric();
		gluQuadricDrawStyle(mycircle, GLU_FILL);
	}
	glPushMatrix();
	glTranslatef(first_pos_x, height - first_pos_y, 0.0);
	gluDisk(mycircle,
		r,           /* inner radius=0.0 */
		pnt_size+r,          /* outer radius=10.0 */
		1000,            /* 16-side polygon */
		3);
	glPopMatrix();

}

void clearWindow() {
	background = (unsigned char*)malloc(sizeof(unsigned char*) * width * height * 4);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (double)width, 0.0, (double)height);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glReadBuffer(GL_FRONT);             /* select front buf to read */
	glPixelStorei(GL_PACK_ALIGNMENT, 1);/* define alighment */
	/*---Read the front buffer and store its contents in image[][][]
	 * (pos_x,pos_y) is the lower-left corner. The size is SIZE X SIZE
	 */
	glReadPixels(0, 0, width, height,
		GL_RGBA, GL_UNSIGNED_BYTE,
		background);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawBuffer(GL_FRONT);              /*Select front buf as destination*/
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	/*--- Write image[][][] to the front buffer ---*/
	glRasterPos2i(0, 0);
	glDrawPixels(height, width,
		GL_RGBA, GL_UNSIGNED_BYTE,
		background);
	glFlush();
}

void type_string() {
	fprintf(stderr, "type new string end up with enter\n");
	//fscanf_s(stdin, "%s", string, 200);
	fgets(string, 200, stdin);
	len = (int)strlen(string);
	type_control = 0;
	fprintf(stderr, "%s%d\n", string, len);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	if (grid_line) {

		glColor4f(1.0, 1.0, 1.0, 0.3);
		glLineWidth(2);
		glBegin(GL_LINES);    /* Draw the line */
		for (float i = 0.0; i < width; i += distance) {
			glVertex2f(i, 0.0);
			glVertex2f(i, height);
		}
		for (float i = 0.0; i < width; i += distance) {
			glVertex2f(0.0, (float)i);
			glVertex2f(width, (float)i);
		}
		glEnd();
		glFlush();
		glColor3f(myColor[0], myColor[1], myColor[2]);
	}
	
	glDrawBuffer(GL_FRONT);              /*Select front buf as destination*/
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	/*--- Write image[][][] to the front buffer ---*/
    glRasterPos2i(0, 0);
	glDrawPixels(height,width,
		GL_RGBA, GL_UNSIGNED_BYTE,
		background);
	
	//fprintf(stderr,"backgroundreapint\n");
	switch (obj_type) {
		case POINT:
			glPointSize(pnt_size);     /*  Define point size */
			glBegin(GL_POINTS);     /*  Draw a point */
			glVertex2f(last_pos_x,height -last_pos_y);
			glEnd();
			fprintf(stderr, "object point\n");
			break;
		case LINE:
			first = 0;
			glLineWidth(pnt_size);     /* Define line width */
			glBegin(GL_LINES);    /* Draw the line */
			glVertex2f(first_pos_x, height - first_pos_y);
			glVertex2f(last_pos_x, height - last_pos_y);
			glEnd();
			fprintf(stderr, "object, line\n");
			break;
		case POLYGON:  /* Define vertices of poly */
			
			if(side>0){
				glLineWidth(pnt_size);
				glBegin(GL_LINES);
				glVertex2f(vertex[side -  1][0], height- vertex[side - 1][1]);
				glVertex2f(last_pos_x, height - last_pos_y);
				glEnd();
				fprintf(stderr, "temp poly line %d\n", side);
			}
			break;
		case CIRCLE:
			glLineWidth(pnt_size);
			draw_circle();
			break;
		case TYPE:
			first = 0;
			glRasterPos2f(last_pos_x, height - last_pos_y);
			for (int i = 0; i < len-1; i++) {
				glutBitmapCharacter(fonts[font_type], string[i]);
			}
			fprintf(stderr, "print string\n");
			break;
		default:
			break;
	}
	glFlush();
}

void reshape(int new_w, int new_h)
{
	height = new_h;
	width = new_w;
	background = (unsigned char*)malloc(sizeof(unsigned char*) * width * height * 4);
	image = (unsigned char*)malloc(sizeof(unsigned char*)*width*height * 4);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (double)width, 0.0, (double)height);
	glViewport(0, 0, width, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glFlush();
	glReadBuffer(GL_FRONT);             /* select front buf to read */
	glPixelStorei(GL_PACK_ALIGNMENT, 1);/* define alighment */
	/*---Read the front buffer and store its contents in image[][][]
	 * (pos_x,pos_y) is the lower-left corner. The size is SIZE X SIZE
	 */
	glReadPixels(0, 0, width, height,
		GL_RGBA, GL_UNSIGNED_BYTE,
		background);
	glutPostRedisplay();   /*---Trigger Display event for redisplay window*/
}

void mykeyboard(unsigned char key, int x, int y)
{
	if (type_control) return;
	fprintf(stderr, "%c key is pressed, cursor is at <%d,%d>\n",
		key, x, y);
	
	switch (key) {
		case 'Q':
		case 'q':
			exit(0);
			break;
		case 'c':
		case 'C':
			clearWindow();
			break;
		default:
			fprintf(stderr, "default for key");
			break;
	}
}

void motion_func(int x, int y)
{
	if (obj_type == CURVE && flag == 0) {
		
		glBegin(GL_LINES);
		glVertex3f(last_pos_x, height - last_pos_y, 0.0);
		glVertex3f(x, height - y, 0.0);
		glEnd();
		last_pos_x = x; last_pos_y = y;
		
	}
	else if (obj_type == LINE || obj_type == CIRCLE || obj_type == TYPE) {
		last_pos_x = x;
		last_pos_y = y;
		if (flag == 0) {
			glutPostRedisplay();
		}
	}
	else if (obj_type == POLYGON) {
		last_pos_x = x;
		last_pos_y = y;
		if(side > 0){
			glutPostRedisplay();
		}
	}
	
	glFinish();
}


void mouse_func(int button, int state, int x, int y)
{	
	
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			first_pos_x = x;
			first_pos_y = y;
			last_pos_x = x;
			last_pos_y = y;
			if (obj_type != POLYGON) {
				if (first == 0) {
					first = 1;
				}
			}
			else {
				if (side == 0) {

					vertex[side][0] = first_pos_x;
					vertex[side][1] = first_pos_y;
					side++;
				}
			}
			flag = 0;
		}else if(state == GLUT_UP){
			last_pos_x = x;
			last_pos_y = y;
			flag = 1;
			if (obj_type == POLYGON) {
				if (side != 0) {
					vertex[side][0] = last_pos_x;
					vertex[side][1] = last_pos_y;
					side++;
				}
				if (side > 2 && abs(vertex[side - 1][0] - first_pos_x) + abs(vertex[side - 1][1] - first_pos_y) < 2) {
					fprintf(stderr, "poly line %d\n", side);
					draw_polygon();
				}
			}
			glutPostRedisplay();
			glReadBuffer(GL_FRONT);             /* select front buf to read */
			glPixelStorei(GL_PACK_ALIGNMENT, 1);/* define alighment */
			/*---Read the front buffer and store its contents in image[][][]
			 * (pos_x,pos_y) is the lower-left corner. The size is SIZE X SIZE
			 */
			glReadPixels(0, 0, width, height,
				GL_RGBA, GL_UNSIGNED_BYTE,
				background);
			glFinish();
			fprintf(stderr,"background update\n");
		}
	}
		
}

void  color_func(int value)
{
	switch (value) {
	case WHITE:
		myColor[0] = myColor[1] = myColor[2] = 1.0;
		break;

	case RED:
		myColor[0] = 1.0;
		myColor[1] = myColor[2] = 0.0;
		break;

	case GREEN:
		myColor[0] = myColor[2] = 0.0;
		myColor[1] = 1.0;
		break;

	case BLUE:
		myColor[0] = myColor[1] = 0.0;
		myColor[2] = 1.0;
		break;
	case BLACK:
		backgroundColor[0] = backgroundColor[1] = backgroundColor[2] = 0.0;
		break;
	case CONSOLE:
		for (int i = 0; i < 3; i++) {
			fprintf(stderr, "please input a float between 0.0 ~1.0\n");
			fscanf_s(stdin,"%f", &myColor[i]);
			if (myColor[i] < 0 || myColor[i]> 1) {
				myColor[i] = 0.0;
			}
		}
		break;
	default:
		break;
	}
	glColor3f(myColor[0], myColor[1], myColor[2]);
}

void background_func(int value) {
	switch (value) {
	case WHITE:
		backgroundColor[0] = backgroundColor[1] = backgroundColor[2] = 1.0;
		break;

	case RED:
		backgroundColor[0] = 1.0;
		backgroundColor[1] = backgroundColor[2] = 0.0;
		break;

	case GREEN:
		backgroundColor[0] = backgroundColor[2] = 0.0;
		backgroundColor[1] = 1.0;
		break;

	case BLUE:
		backgroundColor[0] = backgroundColor[1] = 0.0;
		backgroundColor[2] = 1.0;
		break;
	case BLACK:
		backgroundColor[0] = backgroundColor[1] = backgroundColor[2] = 0.0;
		break;
	case CONSOLE:
		for (int i = 0; i < 3; i++) {
			fprintf(stderr, "please input a float between 0.0 ~1.0\n");
			fscanf_s(stdin, "%f", &backgroundColor[i]);
			if (backgroundColor[i] < 0 || backgroundColor[i]> 1) {
				backgroundColor[i] = 0.0;
			}
		}
		break;
	default:
		break;
	}
	glReadBuffer(GL_FRONT);             /* select front buf to read */
	glPixelStorei(GL_PACK_ALIGNMENT, 1);/* define alighment */
	/*---Read the front buffer and store its contents in image[][][]
	 * (pos_x,pos_y) is the lower-left corner. The size is SIZE X SIZE
	 */
	glReadPixels(0, 0, width, height,
		GL_RGBA, GL_UNSIGNED_BYTE,
		image);
	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glRasterPos2i(0, 0);
	glDrawPixels(width, height,
		GL_RGBA, GL_UNSIGNED_BYTE,
		image);
	
}


/*------------------------------------------------------------
 * Callback function for top menu.
 */
void file_func(int value)
{
	int i, j;

	if (value == MY_QUIT) exit(0);
	else if (value == MY_CLEAR) {
		clearWindow();
	}
	else if (value == MY_SAVE) { /* Save current window */
		glReadPixels(0, 0, width, height, 
			GL_RGBA, GL_UNSIGNED_BYTE,
			image);
	}
	else if (value == MY_LOAD) { /* Restore the saved image */
		glRasterPos2i(0, 0);
		glDrawPixels(width, height,
			GL_RGBA, GL_UNSIGNED_BYTE,
			image);
		glReadBuffer(GL_FRONT);             /* select front buf to read */
		glPixelStorei(GL_PACK_ALIGNMENT, 1);/* define alighment */
		/*---Read the front buffer and store its contents in image[][][]
		 * (pos_x,pos_y) is the lower-left corner. The size is SIZE X SIZE
		 */
		glReadPixels(0, 0, width, height,
			GL_RGBA, GL_UNSIGNED_BYTE,
			background);
		glFinish();
	}
	else if (value == MY_BLEND) { /* Blending current image with the saved image */
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glRasterPos2i(0, 0);
		glDrawPixels(width, height,
			GL_RGBA, GL_UNSIGNED_BYTE,
			image);
	}
	else if (value == GRID) {
		if (grid_line) {
			grid_line = 0;
			clearWindow();
		}
		else grid_line = 1;

	}
	
	glFlush();
}

void color_type(int value) {

}

void draw_type(int value) 
{
	obj_type = value;
	if (value == LINE  || value == CIRCLE)
		first = 0;
	else if (value == POLYGON) {
		polygon_end = 0;
		side = 0;
	}
	else if (value == TYPE) {
		type_control = 1;
		type_string();
	}
}

void size_func(int value)
{
	if (value == 1) {
		if (pnt_size < 10.0) pnt_size += 2.0;
	}
	else {
		if (pnt_size > 3.0) pnt_size -= 2.0;
	}
}

/*---------------------------------------------------------------
 * Callback function for top menu. Do nothing.
 */
void top_menu_func(int value)
{
}

void select_font(int value) {
	font_type = value;
}

int main(int argc, char** argv)
{
    glutInit(&argc,argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGBA);

    glutInitWindowPosition(0, 0);
    glutInitWindowSize(height, width);
    glutCreateWindow("painter");

	image = (unsigned char*)malloc(sizeof(unsigned char*)*width*height * 4);
	background = (unsigned char*)malloc(sizeof(unsigned char*) * width * height * 4);
	
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutKeyboardFunc(mykeyboard);
	
	glutMouseFunc(mouse_func);  /* Mouse Button Callback func */
	glutMotionFunc(motion_func);/* Mouse motion event callback func */
    

	paint_m = glutCreateMenu(color_func);
	glutAddMenuEntry("white", WHITE);
    glutAddMenuEntry("red", RED);
    glutAddMenuEntry("green", GREEN);
    glutAddMenuEntry("blue", BLUE);
	glutAddMenuEntry("black", BLACK);
	glutAddMenuEntry("console", CONSOLE);

	background_m = glutCreateMenu(background_func);
	glutAddMenuEntry("white", WHITE);
	glutAddMenuEntry("red", RED);
	glutAddMenuEntry("green", GREEN);
	glutAddMenuEntry("blue", BLUE);
	glutAddMenuEntry("black", BLACK);
	glutAddMenuEntry("console", CONSOLE);

    color_m = glutCreateMenu(color_type); /* Create color-menu */
	glutAddSubMenu("painter", paint_m);
	glutAddSubMenu("background", background_m);
	

    file_m = glutCreateMenu(file_func);   /* Create another menu, file-menu */
    glutAddMenuEntry("save", MY_SAVE);
    glutAddMenuEntry("load", MY_LOAD);
    glutAddMenuEntry("blend", MY_BLEND);
    glutAddMenuEntry("clear", MY_CLEAR);
	glutAddMenuEntry("grid line", GRID);
    glutAddMenuEntry("quit", MY_QUIT);

	font_m = glutCreateMenu(select_font);
	glutAddMenuEntry("9 by 15", 0);
	glutAddMenuEntry("Times Roman 10", 1);
	glutAddMenuEntry("Times Roman 24", 2);

    type_m = glutCreateMenu(draw_type);   /* Create draw-type menu */
    glutAddMenuEntry("Point", POINT);
    glutAddMenuEntry("Line", LINE);
    glutAddMenuEntry("Poly", POLYGON);
    glutAddMenuEntry("Curve", CURVE);
    glutAddMenuEntry("Circle", CIRCLE);
	glutAddMenuEntry("Tpying", TYPE);
	glutAddSubMenu("font", font_m);

    size_menu = glutCreateMenu(size_func);
    glutAddMenuEntry("smaller", 2);
    glutAddMenuEntry("bigger", 1);
	

    top_m = glutCreateMenu(top_menu_func);/* Create top menu */
    glutAddSubMenu("colors", color_m);    /* add color-menu as a sub-menu */
    glutAddSubMenu("type", type_m);
    glutAddSubMenu("Size", size_menu);
    glutAddSubMenu("file", file_m);       /* add file-menu as a sub-menu */
    glutAttachMenu(GLUT_RIGHT_BUTTON);    /* associate top-menu with right but*/

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutMainLoop();

    return 0;
}
