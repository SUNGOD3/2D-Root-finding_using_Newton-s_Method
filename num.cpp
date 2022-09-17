#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <math.h>
#include "GL/gl.h"
#include <GL/glut.h>
#define holoEN 0.000001
using namespace std;
struct ans {
	int it;
	double xv, yv, xi, yi, maxev, minev, C, evp[50][2];
	bool con;
};
double tx, ty, bigj, smaj;
int tot = 0, cot = 0;
ans a[505];
double gpx(double x, double y) {
	return y - 4;
}
double gpy(double x, double y) {
	return x - 3;
}
double fpx(double x, double y) {
	return x / 32 - (double)3 / 32;
}
double fpy(double x, double y) {
	return y / 32 - (double)4 / 32;
}
double g(double x, double y) {
	return (y - 4) * (x - 3) - 1;
}
double f(double x, double y) {
	return (x - 3) * (x - 3) / 64 + (y - 4) * (y - 4) / 64 - 1;
}
void eigenvalues(double fx, double fy, double gx, double gy) {
	double a = 1, b = (gy + fx), c = (fx * gy - fy * gx), d = b * b - 4 * a * c;
	bigj = (-b + sqrt(d)) / 2;
	smaj = (-b - sqrt(d)) / 2;
	if (bigj < smaj) {
		swap(bigj, smaj);
	}
}
int newton2D(double* x, double* y, int type) {
	int i = 0;
	double  err, xnew, ynew, xold, yold;
	double  fdx, fdy, gdx, gdy, Delta;
	double  fold, gold;
	double	stx = sqrt(abs(tx)), sty = sqrt(abs(ty));

	//Set up initial conditions.
	xold = *x;
	yold = *y;
	//Function values & derivatives of the functions
	fold = f(xold, yold);
	gold = g(xold, yold);
	fdx = fpx(xold, yold);
	fdy = fpy(xold, yold);
	gdx = gpx(xold, yold);
	gdy = gpy(xold, yold);

	eigenvalues(fdx, fdy, gdx, gdy);
	a[tot].maxev = bigj;
	a[tot].minev = smaj;
	a[tot].C = abs(bigj) / abs(smaj);

	Delta = fdx * gdy - fdy * gdx;

	if (type == 2) {
		xnew = xold - ((stx) / Delta) * (gdy * fold - fdy * gold);
		ynew = yold - ((sty) / Delta) * (-gdx * fold + fdx * gold);
	}
	else {
		xnew = xold - (gdy * fold - fdy * gold) / Delta;
		ynew = yold - (-gdx * fold + fdx * gold) / Delta;
	}
	err = sqrt((xnew - xold) * (xnew - xold) + (ynew - yold) * (ynew - yold));
	if (type) {
		printf("      i          xn                   yn                    error\n");
		printf("------------------------------------------------------------\n");
		printf("     %3d \t%lf \t%lf \t---\n", i, xold, yold);
	}
	i = 1;
	while (err > holoEN) {
		if (type)
			printf("     %3d \t%lf \t%lf \t%lf\n", i, xnew, ynew, err);

		a[tot].evp[i][0]=xnew;
		a[tot].evp[i][1]=ynew;
		
		xold = xnew; // Save current value.
		yold = ynew;
		//Update the root.
		fold = f(xold, yold);
		gold = g(xold, yold);
		fdx = fpx(xold, yold);
		fdy = fpy(xold, yold);
		gdx = gpx(xold, yold);
		gdy = gpy(xold, yold);

		Delta = fdx * gdy - fdy * gdx;

		if (type == 2) {
			xnew = xold - ((stx) / Delta) * (gdy * fold - fdy * gold);
			ynew = yold - ((sty) / Delta) * (-gdx * fold + fdx * gold);
		}
		else {
			xnew = xold - (gdy * fold - fdy * gold) / Delta;
			ynew = yold - (-gdx * fold + fdx * gold) / Delta;
		}
		//Compute the 1-norm of the error vector.
		err = sqrt((xnew - xold) * (xnew - xold) + (ynew - yold) * (ynew - yold));
		
		i++;
	}
	
	a[tot].evp[i][0]=xnew;
	a[tot].evp[i][1]=ynew;
	
	if (type) {
		printf("------------------------------------------------------------\n");
		printf("     %3d \t%lf \t%lf \t%lf\n", i, xnew, ynew, err);
	}
	*x = xnew;
	*y = ynew;
	return i;
}

//畫圖func----------------------------------------------------------
void display_func() {
	glClearColor(1, 1, 1, 1);//the color of eraser
	glClear(GL_COLOR_BUFFER_BIT);//擦乾淨

	//畫x,y軸
	glBegin(GL_LINES);
	glColor3f(0.5, 0.5, 0.5);
	glVertex2f(-1.0, 0.0);
	glVertex2f(1.0, 0.0);
	glVertex2f(0.0, 1.0);
	glVertex2f(0.0, -1.0);
	glEnd();

	//畫格子
	glBegin(GL_LINES);
	glColor3f(0.8,0.8,0.8);
	for (float i = -20; i < 20; ++i) {
		glVertex2f(-1.0, i/20);
		glVertex2f(1.0, i/20);
		glVertex2f(i/20, 1.0);
		glVertex2f(i/20, -1.0);
	}
	glEnd();

	//畫g函式
	float x, y;
	glBegin(GL_LINE_STRIP);////選擇畫連續的線
	glColor3f(0, 0, 0);//the color of dot
	for (x = -6.0; x <= 12.0 ; x += 0.05) {
		if (abs(x-3)>0.0001) {
			y = (4 * x - 11) / (x - 3);
			glVertex2f(x / 20, y / 20);
		}
		else {
			glEnd();
			glBegin(GL_LINE_STRIP);
			glColor3f(0, 0, 0);
		}
	}
	glEnd();

	//畫f函式
	glBegin(GL_LINE_STRIP);
	glColor3f(0, 0, 0);
	float R = 8;
	for (int i = 0; i < 200; i++) {
		glVertex2f((R * cos(i/3.141592) + 3) / 20, (R * sin(i/3.141592) + 4) / 20);
	}
	glEnd();

	//畫收斂path
	for(int i=0;i<tot;++i){
		glBegin(GL_LINE_STRIP);
		glColor3f(0.5, 0.5, 1);
		for(int j=0;j<a[i].it;++j){
			glVertex2f(a[i].evp[j][0] / 20, a[i].evp[j][1] / 20);
		}
		glEnd();
	}

	//畫發散path
	glBegin(GL_LINES);
	glColor3f(0.5, 0, 0);
	glVertex2f(-6.0/20, -5.0/20);
	glVertex2f(12.0/20, 13.0/20);
	glVertex2f(-6.0/20, 13.0/20);
	glVertex2f(12.0/20, -5.0/20);
	glEnd();

	//點塗色
	glPointSize(5);    //點的大小
	glBegin(GL_POINTS); //選擇畫點

	for (int i = 0; i < tot; ++i) {
		float x = a[i].xi, y = a[i].yi;
		if (a[i].con == 0) {//發散點塗紅
			glColor3f(1, 0, 0);
			glVertex2f(x / 20, y / 20);
		}
		else {
			glColor3f(0, 1, 0);
			glVertex2f(x / 20, y / 20);
		}
	}
	glEnd();


	//畫單位點
	glPointSize(3);    //點的大小
	glBegin(GL_POINTS); //選擇畫點
	glColor3f(0, 0, 0);
	for (float i = -20; i < 20; i++) {
		glVertex2f(0.0, i / 20);
		glVertex2f(i / 20, 0.0);
	}
	glEnd();

	

	glFlush();
}
void reshape(int w, int h) {
	glViewport(0, 0, w, h);    //調整視角
}
//----------------------------------------------------------------------
void test(double x, double y) {
	cout << gpx(x, y) << " ";
	cout << gpy(x, y) << " ";
	cout << fpx(x, y) << " ";
	cout << fpy(x, y) << " ";
	cout << g(x, y) << " ";
	cout << f(x, y) << " ";
	cout << fpx(x, y) * gpy(x, y) - fpy(x, y) * gpx(x, y) << "\n";
}
int main(int argc, char** argv) {
	
	double x = 2, y = -4;
	
	//畫圖-------------------------------------------------------------------------------
	glutInit(&argc, argv);
	glutInitWindowPosition(500, 0);    //初始位置
	glutInitWindowSize(700, 700);    //大小
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);    //設定模式
	glutCreateWindow("Test1");
	glutReshapeFunc(reshape);
	glutDisplayFunc(display_func);
	//-------------------------------------------------------------------------------------
	printf("HW1-1-(a) 2 initial values x0 = 2, y0 = -4 :\n");
	newton2D(&x, &y, 1);
	tx = x;//answer of x
	ty = y;//answer of y
	printf("HW1-1-(b) :\n");
	for (int i = -6; i <= 12; ++i) {
		for (int j = -5; j <= 13; ++j, ++tot) {
			x = i;
			y = j;
			a[tot].it = newton2D(&x, &y, 0);
			a[tot].xi = i;
			a[tot].yi = j;
			a[tot].xv = x;
			a[tot].yv = y;
			if (isnan(x) && isnan(y)) {
				++cot;
				a[tot].con = 0;
				printf("%d %d\n", i, j);
			}
			else {
				a[tot].con = 1;
			}
		}
	}
	//printf("%d %d\n",tot,cot);
	printf("HW1-1-(c) :\n");
	for (int i = 0; i < tot; ++i) {
		if (a[i].con) {
			printf("%lf %lf %d\n", a[i].xi, a[i].yi, a[i].it);
		}
	}
	printf("HW1-1-(d) :\n");
	for (int i = 0; i < tot; ++i) {
		if (a[i].con == 0) {
			x = a[i].xi + sqrt(abs(tx));
			y = a[i].yi + sqrt(abs(ty));
			newton2D(&x, &y, 1);
			if (isnan(x) && isnan(y)) {
				printf("%d %d is still disconverge\n", (int)a[i].xi, (int)a[i].yi);
			}
			else {
				printf("%d %d converge\n", (int)a[i].xi, (int)a[i].yi);
				printf("ans: %lf %lf\n", x, y);
			}
		}
	}
	printf("HW4(B) :\n");
	for (int i = 0; i < tot; ++i) {
		printf("%d %d %lf ", (int)a[i].xi, (int)a[i].yi, a[i].C);
		if (a[i].con) {
			printf("converge\n");
		}
		else {
			printf("disconverge\n");
		}
	}
	glutMainLoop();
}
