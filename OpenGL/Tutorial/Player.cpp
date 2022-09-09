#include <string.h>
#include <GL/glui.h>

#define PI 3.141592

void animate(int value);
void result2(int id);
void result(int id);
void rad(int id);
void sub(int id);
void ZRateChange(int id);
void YRateChange(int id);
void XRateChange(int id);
void init();
void myGlutDisplay(void);
void drawArm(void);
void drawBody(void);
void reset(int id);
void setspin(int id);

typedef enum
{
	RED = 1,
	GREEN = 2,
	BLUE = 3
}COLORVALUE;

typedef enum
{
	TEAPOT = 0,
	SPHERE = 1,
}HEADVALUE;

int main_window;
char text[sizeof(GLUI_String)] = "abc";
int si = 0, ck;
float armAngle = 0, sf = 0.0;
GLUI_EditText* ep1;
GLUI_Spinner* sp1;
GLUI_Spinner* sp2;
GLUI_Checkbox* cp;
int i = 1, j = 0, color;
GLUI_Rotation* rp;
GLUI_RadioGroup* rdp;
GLfloat x, z, angle;
GLuint body;

float rot[16] = { 1.0, 0.0, 0.0, 0.0,
				 0.0, 1.0, 0.0, 0.0,
				 0.0, 0.0, 0.1, 0.0,
				 0.0, 0.0, 0.0, 1.0 };

GLUI_Translation* xp;
GLUI_Translation* yp;
GLUI_Translation* zp;
float x_rate = 1.0;
float y_rate = 1.0;
float z_rate = 1.0;
float xyz[3] = { 0.0, 0.0, 0.0 };

const GLfloat light_pos[] = { 0.0, 0.0, 20.0, 1.0 };
const GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
const GLfloat red[] = { 0.8f, 0.0, 0.0, 1.0 };
const GLfloat green[] = { 0.0, 0.8f, 0.0, 1.0 };
const GLfloat blue[] = { 0.0, 0.0, 0.8f, 1.0 };
const GLfloat initColor[] = { 0.0, 1.0, 1.0, 1.0 };
GLfloat animRotate = 0.0f, animSpeed = 1.0f;

void setspin(int id)
{
	if (i == 0)
		rp->set_spin(1.0);
	else
		rp->set_spin(0.0);
}

void reset(int id)
{
	if (id == 1)
		rp->reset();
	if (id == 2) {
		xp->set_x(0.0);
		yp->set_y(0.0);
		zp->set_z(0.0);
	}
}

void drawBody(void)
{
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	for (angle = 0.0f; angle < (2.0f * PI); angle += (PI / 8.0f))
	{
		x = 0.0f + 1.0f * sin(angle);
		z = 0.0f + 1.0f * cos(angle);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(x, 0.0f, z);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (angle = 0.0f; angle < (2.0f * PI); angle += (PI / 8.0f))
	{
		x = 0.0f + 1.0f * sin(angle);
		z = 0.0f + 1.0f * cos(angle);
		glNormal3f(sin(angle), 0.0f, cos(angle));
		glVertex3f(x, 0.0f, z);
		glVertex3f(x, 3.0f, z);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 3.0f, 0.0f);
	for (angle = (2.0f * PI); angle > 0.0f; angle -= (PI / 8.0f))
	{
		x = 0.0f + 1.0f * sin(angle);
		z = 0.0f + 1.0f * cos(angle);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(x, 3.0f, z);
	}
	glEnd();	
}

void drawArm(void)
{
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	for (angle = 0.0f; angle < (2.0f * PI); angle += (PI / 8.0f))
	{
		x = 0.0f + 0.3f * sin(angle);
		z = 0.0f + 0.3f * cos(angle);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(x, 0.0f, z);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (angle = 0.0f; angle < (2.0f * PI); angle += (PI / 8.0f))
	{
		x = 0.0f + 0.3f * sin(angle);
		z = 0.0f + 0.3f * cos(angle);
		glNormal3f(sin(angle), 0.0f, cos(angle));
		glVertex3f(x, 0.0f, z);
		glVertex3f(x, 1.5f, z);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 1.5f, 0.0f);
	for (angle = (2.0f * PI); angle > 0.0f; angle -= (PI / 8.0f))
	{
		x = 0.0f + 0.3f * sin(angle);
		z = 0.0f + 0.3f * cos(angle);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(x, 1.5f, z);
	}
	glEnd();
}

void myGlutDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glShadeModel(GL_SMOOTH);
	gluLookAt(0, 2, 10, 0, 0, 0, 0, 0.5, 0);
	glColor3f(0.0, 0.0, 0.0);
	glTranslatef(xyz[0], xyz[1], xyz[2]);
	glMultMatrixf(rot);
	glPushMatrix();
		glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
		
		switch (color)
		{
			case RED:
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
				break;
			case GREEN:
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
				break;
			case BLUE:
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
				break;
			default:
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
				break;
		}

		
		drawBody(); // ¸ö
		glPushMatrix(); // ¾ó±¼
		glTranslatef(0.0f, 3.5f, 0.0f);
		switch (j)
		{
			case TEAPOT:
				glRotatef(90.0f, 0.0f, -1.0f, 0.0f);
				glutSolidTeapot(0.5f);
				break;
			case SPHERE:
				glutSolidSphere(0.5f, 20, 20);
				break;
			default:
				break;
		}
		glPopMatrix();
		
		glPushMatrix();  // ¿ÞÂÊ ¾î±ú
		glTranslatef(-1.0f, 2.7f, 0.0f);
		glRotatef(180.0f - animRotate, 0, 0, 1.0f);
		drawArm();
			glPushMatrix(); // ¿ÞÂÊ ÆÈ
			glTranslatef(0.0f, 1.5f, 0.0f);
			drawArm();
			glPopMatrix();
		glPopMatrix();
		
		glPushMatrix();  // ¿À¸¥ÂÊ ¾î±ú
		glTranslatef(1.0f, 2.7f, 0.0f);
		glRotatef(-180 + animRotate, 0, 0, 1.0f);
		drawArm();
			glPushMatrix(); // ¿À¸¥ÂÊ ÆÈ
			glTranslatef(0.0f, 1.5f, 0.0f);
			drawArm();
			glPopMatrix();
		glPopMatrix();

		glPushMatrix();  // ¿ÞÂÊ Çã¹÷Áö
		glTranslatef(-0.7f - (animRotate/ 90), -1.5f, 0.0f);
		glRotatef(animRotate / 2, 0, 0, -1.0f);
		drawArm();
			glPushMatrix(); // ¿ÞÂÊ ´Ù¸®
			glTranslatef(0.0f, -1.5f, 0.0f);
			
			drawArm();
			glPopMatrix();
		glPopMatrix();
		
		glPushMatrix();  // ¿À¸¥ÂÊ Çã¹÷Áö
		glTranslatef(0.7f+(animRotate / 90), -1.5f, 0.0f);
		glRotatef(-animRotate / 2, 0, 0, -1.0f);
		drawArm();
			glPushMatrix(); // ¿À¸¥ÂÊ ´Ù¸®
			glTranslatef(0.0f, -1.5f, 0.0f);
			
			drawArm();
			glPopMatrix();
		glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
}

void init()
{
	body = glGenLists(1);
	glClearColor(.9f, .9f, .9f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0, 1.0, 3.0, -3.0);
	glMatrixMode(GL_MODELVIEW);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
}

void XRateChange(int id)
{
	xp->set_speed(x_rate);
}

void YRateChange(int id)
{
	yp->set_speed(y_rate);
}

void ZRateChange(int id)
{
	zp->set_speed(z_rate);
}

void sub(int id)
{
	printf("%d \n", rdp->get_int_val());
}

void rad(int id) 
{
	if (j < 1) j++;
	else if (j == 1) j = 0;
	rdp->set_int_val(j);
	GLUI_Master.sync_live_all();
	printf("%d \n", rdp->get_int_val());
}

void result(int id) 
{
	printf("%d \n", color);
}

void result2(int id) 
{
	printf("%s \n", text); // ep1->get_text());   // text
	printf("%f \n", animSpeed); // ep2->get_int_val());  // i
	printf("%f \n", armAngle); // ep3->get_float_val());  // f
}

void animate(int value)
{
	animRotate += animSpeed;
	if (animRotate > 89.0f || animRotate < 0.0f)
	{
		animSpeed *= -1;
	}

	if(cp->get_int_val() == 1)
	{
		cp->set_int_val(0);
		cp->set_int_val(1);
		glutTimerFunc(30, animate, 1);
	}
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	main_window = glutCreateWindow("GLUT");
	init();
	glutDisplayFunc(myGlutDisplay);
	GLUI* glui = GLUI_Master.create_glui("GLUI");

	rp = glui->add_rotation("Rotate", rot, -1, (GLUI_Update_CB)NULL);
	rp->reset();
	glui->add_button("R-Reset", 1, reset);
	glui->add_button("T-Reset", 2, reset);

	GLUI_Rollout* panel1 = glui->add_rollout("Translate", 1);
	
	xp = glui->add_translation_to_panel(panel1, "X", GLUI_TRANSLATION_X, &xyz[0]);
	GLUI_Spinner* sp = glui->add_spinner_to_panel(panel1, "X", GLUI_SPINNER_FLOAT, &x_rate, 0, XRateChange);
	sp->set_float_limits(0.01, 2.0, GLUI_LIMIT_CLAMP);
	glui->add_column_to_panel(panel1, 1);
	yp = glui->add_translation_to_panel(panel1, "Y", GLUI_TRANSLATION_Y, &xyz[1]);
	sp = glui->add_spinner_to_panel(panel1, "Y", GLUI_SPINNER_FLOAT, &y_rate, 0, YRateChange);
	sp->set_float_limits(0.01, 2.0, GLUI_LIMIT_CLAMP);
	glui->add_column_to_panel(panel1, 1);
	zp = glui->add_translation_to_panel(panel1, "Z", GLUI_TRANSLATION_Z, &xyz[2]);
	sp = glui->add_spinner_to_panel(panel1, "Z", GLUI_SPINNER_FLOAT, &z_rate, 0, ZRateChange);
	sp->set_float_limits(0.01, 2.0, GLUI_LIMIT_CLAMP);

	glui->add_separator();
	cp = glui->add_checkbox("Animation", &ck, 0, animate);
	cp->set_alignment(GLUI_ALIGN_CENTER);
	glui->add_separator();

	GLUI_Rollout* panel2 = glui->add_rollout("head", 2);
	rdp = glui->add_radiogroup_to_panel(panel2, &j, 2, sub);
	glui->add_radiobutton_to_group(rdp, "Teapot");
	glui->add_radiobutton_to_group(rdp, "Sphere");
	glui->add_button_to_panel(panel2, "Radio", 2, rad);
	glui->add_separator();

	GLUI_Listbox* lp = glui->add_listbox("Color ", &color, 3, result);
	lp->add_item(1, "Red");
	lp->add_item(2, "Green");
	lp->add_item(3, "Blue");
	lp->set_alignment(GLUI_ALIGN_CENTER);

	glui->add_separator();

	GLUI_StaticText* st = glui->add_statictext("Editable Text Boxes");
	st->set_alignment(GLUI_ALIGN_CENTER);
	ep1 = glui->add_edittext("AnimSpeed", GLUI_EDITTEXT_FLOAT, &animSpeed, -1, result2);
	ep1->set_float_limits(0.1, 10, GLUI_LIMIT_CLAMP);
	ep1->set_alignment(GLUI_ALIGN_CENTER);
	glui->add_separator();

	glui->add_button("Quit", 0, (GLUI_Update_CB)exit);

	glui->set_main_gfx_window(main_window);
	GLUI_Master.set_glutIdleFunc(NULL);

	glutMainLoop();
	return EXIT_SUCCESS;
}