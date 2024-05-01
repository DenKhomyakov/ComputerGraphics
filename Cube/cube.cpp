#include <iostream>
#include <cmath>
#include <graphics.h>
#include <conio.h>
#include <windows.h>
#include <stdio.h>

using namespace std;

/* The vertices of the cube */
double cubeVertices[8][4] =
{
	{1,-1, 1,1},{-1,-1, 1,1},{-1,1, 1,1},{1,1, 1,1},
	{1,-1,-1,1},{-1,-1,-1,1},{-1,1,-1,1},{1,1,-1,1}
};

/* Edges of the cube */
int cubeEdges[12][2] =
{
	   {0,1},{1,2},{2,3},{3,0},
	   {4,5},{5,6},{6,7},{7,4},
	   {0,4},{1,5},{2,6},{3,7}
};

/* The center of the screen by X and Y */
int screenCenterX;
int screenCenterY;

/* Scaling factor */
double scaleCoefficient;

void Print(double* arrayData, int numRows, int numCols)
{
	for (int i = 0; i < numRows; i++)
	{
		for (int k = 0; k < numCols; k++)
		{
			printf("%6.2lf", arrayData[i * numCols + k]);
		}

		cout << endl;
	}

	cout << endl;
}

double* MatrixMultiplication(double matrixA[][4], int rowsA, double matrixB[][4], int rowsB)
{
	double* resultMatrix = new double[rowsA * 4];

	for (int i = 0; i < rowsA; i++)
	{
		for (int k = 0; k < 4; k++)
		{
			resultMatrix[i * 4 + k] = 0;

			for (int l = 0; l < 4; l++)
			{
				resultMatrix[i * 4 + k] += matrixA[i][l] * matrixB[l][k];
			}
		}
	}

	return resultMatrix;
}

void CalculateRotationMatrix(double xCoordinate, double yCoordinate, double zCoordinate, double rotationMatrix[][4])
{
	double temporaryMatrices[5][4][4];

	for (int i = 0; i < 5; i++)
	{
		memset(temporaryMatrices[i], 0, sizeof(double) * 4 * 4);

		for (int k = 0; k < 4; k++)
		{
			temporaryMatrices[i][k][k] = 1;
		}

		if (!i)
		{
			temporaryMatrices[i][3][0] = -xCoordinate;
			temporaryMatrices[i][3][1] = -yCoordinate;
			temporaryMatrices[i][3][2] = -zCoordinate;
		}
		else if (i == 1)
		{
			temporaryMatrices[i][0][0] = -1;
		}
		else if (i == 2)
		{
			temporaryMatrices[i][1][1] = temporaryMatrices[i][2][2] = 0;
			temporaryMatrices[i][1][2] = -1;
			temporaryMatrices[i][2][1] = 1;
		}
		else if (i == 3)
		{
			double distance = sqrt(xCoordinate * xCoordinate + yCoordinate * yCoordinate);
			double cosine = yCoordinate / distance;
			double sine = xCoordinate / distance;

			temporaryMatrices[i][0][0] = temporaryMatrices[i][2][2] = cosine;
			temporaryMatrices[i][0][2] = sine;
			temporaryMatrices[i][2][0] = -sine;
		}
		else
		{
			double sum = sqrt(xCoordinate * xCoordinate + yCoordinate * yCoordinate + zCoordinate * zCoordinate);
			double cosine = sqrt(xCoordinate * xCoordinate + yCoordinate * yCoordinate) / sum;
			double sine = zCoordinate / sum;

			temporaryMatrices[i][1][1] = temporaryMatrices[i][2][2] = cosine;
			temporaryMatrices[i][1][2] = -sine;
			temporaryMatrices[i][2][1] = sine;
		}
	}

	double* resultMatrix;

	for (int i = 1; i < 5; i++)
	{
		resultMatrix = MatrixMultiplication(temporaryMatrices[0], 4, temporaryMatrices[i], 4);

		memmove(temporaryMatrices[0], resultMatrix, sizeof(double) * 4 * 4);

		delete[] resultMatrix;
	}

	memmove(rotationMatrix, temporaryMatrices[0], sizeof(double) * 4 * 4);
}

void DrawCube(double transformedVertices[][4])
{
	setcolor(BLACK);

	for (int i = 0; i < 12; i++)
	{
		int vertexIndex0 = cubeEdges[i][0];
		int vertexIndex1 = cubeEdges[i][1];

		int xCoordinate0 = screenCenterX + (int)(transformedVertices[vertexIndex0][0] * scaleCoefficient);
		int yCoordinate0 = screenCenterY - (int)(transformedVertices[vertexIndex0][1] * scaleCoefficient);
		int xCoordinate1 = screenCenterX + (int)(transformedVertices[vertexIndex1][0] * scaleCoefficient);
		int yCoordinate1 = screenCenterY - (int)(transformedVertices[vertexIndex1][1] * scaleCoefficient);

		line(xCoordinate0, yCoordinate0, xCoordinate1, yCoordinate1);
	}
}

int main()
{
	double initialX = 2;
	double initialY = 3;
	double initialZ = 1;

	double rotationMatrix[4][4];

	int screenWidth = 500;
	int screenHeight = 500;

	screenCenterX = screenWidth / 2;
	screenCenterY = screenHeight / 2;
	scaleCoefficient = 100;

	int steps = 100;
	double increment = 1.0 / steps;

	initwindow(screenWidth, screenHeight, "Cube", 100, 100);
	setbkcolor(WHITE);
	setfillstyle(SOLID_FILL, WHITE);

	int activePage = 1;

	for (int i = 0; i <= steps && !kbhit(); i++)
	{
		CalculateRotationMatrix(initialX, initialY, initialZ, rotationMatrix);

		double* temporaryPointer = MatrixMultiplication(cubeVertices, 8, rotationMatrix, 4);
		double transformedCubeVertices[8][4];

		memmove(transformedCubeVertices, temporaryPointer, sizeof(double) * 8 * 4);
		delete[] temporaryPointer;

		setactivepage(activePage);
		cleardevice();

		DrawCube(transformedCubeVertices);
		setvisualpage(activePage);

		if (!kbhit())
		{
			Sleep(100);
		}

		initialX -= increment;
		initialY -= increment;
		activePage = 1 - activePage;
	}

	if (!kbhit())
	{
		getch();
	}

	closegraph();

	return 0;
}


