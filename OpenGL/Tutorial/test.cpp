#include <gl/glut.h>
GLfloat GDel = 0.0, TDel = 0.0; int anim = 0;
void MyDisplay() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 0.0, 0.0);
    glRectf(-1.0 + TDel, 0.25, -0.5 + TDel, 0.75);
    glColor3f(0.0, 0.0, 1.0);
    glRectf(-1.0 + GDel, -0.25, -0.5 + GDel, -0.75);
    glutSwapBuffers();
}
void MyTimer(int Value) {
    TDel = TDel + 0.005;
    glutPostRedisplay();
    if (anim == 1)
        glutTimerFunc(40, MyTimer, 0);
}
void MyTimer2(int Value) {
    GDel = GDel + 0.005;
    glutPostRedisplay();
    if (anim == 1)
        glutTimerFunc(80, MyTimer2, 0);
}
void MyMouse(int b, int s, int x, int y) {
    if (b == GLUT_LEFT_BUTTON && s == GLUT_DOWN && anim != 1) {
        anim = 1;
        glutTimerFunc(40, MyTimer, 1);
        glutTimerFunc(80, MyTimer2, 0);
    }
    if (b == GLUT_RIGHT_BUTTON && s == GLUT_DOWN)
        anim = 0;
}
void MyInit() {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0, 1.0, -1.0, 1.0, 1.0, -1.0);
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(300, 300);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Timer Callback");
    MyInit();
    glutDisplayFunc(MyDisplay);
    glutMouseFunc(MyMouse);
    glutMainLoop();
    return 0;
}
