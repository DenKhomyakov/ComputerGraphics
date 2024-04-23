#include <iostream>
#include <cmath>
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <windows.h>

using namespace std;

double cubeVertices[8][4] =
{
	{1,-1, 1,1}, {-1,-1, 1,1}, {-1,1, 1,1}, {1,1, 1,1},
	{1,-1,-1,1}, {-1,-1,-1,1}, {-1,1,-1,1}, {1,1,-1,1}
};

int cubeEdges[6][5] =
{ {0,1,2,3,0}, {4,5,6,7,4}, {0,1,5,4,0},
  {1,2,6,5,1}, {3,2,6,7,3}, {0,3,7,4,0}
};

int screenCenterX;
int screenCenterY;

double scaleCoefficient;

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
		else
		{
			if (i == 1)
			{
				temporaryMatrices[i][0][0] = -1;
			}
			else
			{
				if (i == 2)
				{
					temporaryMatrices[i][1][1] = temporaryMatrices[i][2][2] = 0;
					temporaryMatrices[i][1][2] = -1;
					temporaryMatrices[i][2][1] = 1;
				}
				else
				{
					if (i == 3)
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
			}
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

void DrawCube(double vertexCoordinates[][4], int* cubeFaces, int cubeColor)
{
	int color[] = { RED,BLUE,YELLOW,GREEN,BROWN,MAGENTA };
	setcolor(BLACK);
	setfillstyle(SOLID_FILL, color[cubeColor]);

	int newCubeVertices[10];

	for (int i = 0; i < 5; i++)
	{
		newCubeVertices[i * 2] = screenCenterX + (int)(vertexCoordinates[cubeFaces[i]][0] * scaleCoefficient);
		newCubeVertices[i * 2 + 1] = screenCenterY - (int)(vertexCoordinates[cubeFaces[i]][1] * scaleCoefficient);
	}

	fillpoly(5,newCubeVertices);
	//drawpoly(5, newCubeVertices);
}

void CalculateABCD(double vertexCoordinates[][4], int* faceIndices, double* ABCD)
{
	int firstVertexIndex = faceIndices[0];
	int secondVertexIndex = faceIndices[1];
	int thirdVertexIndex = faceIndices[2];

	ABCD[0] = ((vertexCoordinates[thirdVertexIndex][1] - vertexCoordinates[firstVertexIndex][1]) * (vertexCoordinates[secondVertexIndex][2] - vertexCoordinates[firstVertexIndex][2]) - (vertexCoordinates[secondVertexIndex][1] - vertexCoordinates[firstVertexIndex][1]) * (vertexCoordinates[thirdVertexIndex][2] - vertexCoordinates[firstVertexIndex][2]));
	ABCD[1] = ((vertexCoordinates[secondVertexIndex][0] - vertexCoordinates[firstVertexIndex][0]) * (vertexCoordinates[thirdVertexIndex][2] - vertexCoordinates[firstVertexIndex][2]) - (vertexCoordinates[thirdVertexIndex][0] - vertexCoordinates[firstVertexIndex][0]) * (vertexCoordinates[secondVertexIndex][2] - vertexCoordinates[firstVertexIndex][2]));
	ABCD[2] = ((vertexCoordinates[thirdVertexIndex][0] - vertexCoordinates[firstVertexIndex][0]) * (vertexCoordinates[secondVertexIndex][1] - vertexCoordinates[firstVertexIndex][1]) - (vertexCoordinates[secondVertexIndex][0] - vertexCoordinates[firstVertexIndex][0]) * (vertexCoordinates[thirdVertexIndex][1] - vertexCoordinates[firstVertexIndex][1]));
	ABCD[3] = -(ABCD[0] * vertexCoordinates[firstVertexIndex][0] + ABCD[1] * vertexCoordinates[firstVertexIndex][1] + ABCD[2] * vertexCoordinates[firstVertexIndex][2]);
}

int main()
{
	double initialX = 2;
	double initialY = 3;
	double initialZ = 1.5;

	double rotationMatrix[4][4];

	int screenWidth = 500;
	int screenHeight = 500;

	screenCenterX = screenWidth / 2;
	screenCenterY = screenHeight / 2;
	scaleCoefficient = 100;

	int steps = 100;
	double increment = 1.0 / steps;

	int numVertices = 8;

	double centerX;
	double centerY;
	double centerZ;

	centerX = centerY = centerZ = 0;

	for (int i = 0; i < numVertices; i++)
	{
		centerX += cubeVertices[i][0];
		centerY += cubeVertices[i][1];
		centerZ += cubeVertices[i][2];
	}

	centerX /= numVertices;
	centerY /= numVertices;
	centerZ /= numVertices;

	initwindow(screenWidth, screenHeight, "Cube without invisible faces", 100, 100);

	int activePage = 1;

	for (int i = 0; i <= steps && !kbhit(); i++)
	{
		CalculateRotationMatrix(initialX, initialY, initialZ, rotationMatrix);

		double* resultMatrix = MatrixMultiplication(cubeVertices, 8, rotationMatrix, 4);
		double transformedVertices[8][4];

		memmove(transformedVertices, resultMatrix, sizeof(double) * 8 * 4);

		delete[] resultMatrix;

		setactivepage(activePage);
		setbkcolor(WHITE);
		setfillstyle(SOLID_FILL, WHITE);
		cleardevice();

		int numEdges = 6;
		double ABCD[4];

		for (int k = 0; k < numEdges; k++)
		{
			CalculateABCD(cubeVertices, cubeEdges[k], ABCD);

			double signedDistance = ABCD[0] * centerX + ABCD[1] * centerY + ABCD[2] * centerZ + ABCD[3];

			if (signedDistance < 0)
			{
				for (int j = 0; j < 4; j++)
				{
					ABCD[j] = -ABCD[j];
				}
			}
			if (ABCD[0] * initialX + ABCD[1] * initialY + ABCD[2] * initialZ < 0)
			{
				DrawCube(transformedVertices, cubeEdges[k], k);
			}
		}

		char infoString[100];

		//sprintf(infoString, "x0=%.3lf  y0=%.3lf  z0=%.3lf", initialX, initialY, initialZ);

		//outtextxy(0, 0, infoString);
		setvisualpage(activePage);

		if (!kbhit())
		{
			Sleep(10000);
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

