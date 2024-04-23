#include <iostream>
#include <cmath>
#include <graphics.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#define M_PI 3.1415

using namespace std;

void DrawArrow(int arrow[][2],int arrowColor)
{
     setcolor(arrowColor);

     for (int i = 0; i < 6; i++)
     {
         line(arrow[i][0], arrow[i][1], arrow[i + 1][0], arrow[i + 1][1]);
     }
}

int main()
{
    int seconds = 30;

    do
    {
        cout << "Enter the seconds: ";
        cin>>seconds;
    }while (seconds < 0);

    double coordinatesOfArrowPoint[7][2] = {{0, 0}, {-5, 30}, {-10, 30}, {0, 45}, {10, 30}, {5, 30}, {0, 0}};
    double arrowRadius[7];
    double scalingRadius = 3;

    for (int i = 0; i < 7; i++)
    {
        arrowRadius[i] = scalingRadius * sqrt(coordinatesOfArrowPoint[i][0] * coordinatesOfArrowPoint[i][0] + coordinatesOfArrowPoint[i][1] * coordinatesOfArrowPoint[i][1]);
    }

    int width = 400;
    int height = 400;

    initwindow(width, height, "Clock", 100, 100);
    setbkcolor(WHITE);
    setfillstyle(SOLID_FILL, WHITE);

    cleardevice();

    int arrow[7][2];

    double angleRotation = 0;
    double angleStepPerMarker = M_PI / 30;

    int initialX = width / 2;
    int initialY = height / 2;

    int clockRadius = width / 2 - 50;

    setfillstyle(SOLID_FILL, BLACK);

    for(int i = 0; i < 60; i++)
    {
            int x = initialX + (int)(clockRadius * cos(angleRotation));
            int y = initialY - (int)(clockRadius * sin(angleRotation));

            fillellipse(x, y, 4 - (i % 5 ? 2 : 0), 4 - (i % 5 ? 2 : 0));

            angleRotation += angleStepPerMarker;
    }

    angleRotation = 0;

    for(int i = 0; i <= seconds && !kbhit(); i++)
    {
        if (i)
        {
            DrawArrow(arrow, WHITE);
        }

        for(int k = 0; k < 7; k++)
        {
                double a = atan2(coordinatesOfArrowPoint[k][1], coordinatesOfArrowPoint[k][0]);

                arrow[k][0] = initialX + (int)(arrowRadius[k] * cos(a - angleRotation));
                arrow[k][1] = initialY - (int)(arrowRadius[k] * sin(a - angleRotation));
        }

        DrawArrow(arrow, BLACK);

        for (int k = 0; i == seconds && k < 1 && !kbhit(); k++)
        {
            Beep(1000, 1000);
        }

        Sleep(1000);

        angleRotation += angleStepPerMarker;
    }

    if (!kbhit())
    {
        getch();
    }

    closegraph();

    return 0;
}


