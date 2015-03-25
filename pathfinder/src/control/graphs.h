#ifndef GRAPHS_H
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SDL/SDL.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <figura.h>
#include <chain.h>
#define GRAPHS_H

using namespace std;
using namespace cv;

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Methods////////////////////////////////////////////////
void createGraph(Mat &sourceImage);
void putPoint(Mat &sourceImage, float xx, float yy);
void putAngle(Mat &sourceImage, float xx, float yy, int anglet, int length);
void putCross(Mat &sourceImage, float xx, float yy, int anglet, int length);
void putEllipse(Mat &sourceImage, float xx, float yy, float desvX, float desvY);
//void putPoint(Mat &sourceImage, float xx, float yy);
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
 void createGraph(Mat &sourceImage)
 {

    
    line(sourceImage, cvPoint(10,10), cvPoint(10, 470), Scalar(0,0,255), 1, 8, 0);
    line(sourceImage, cvPoint(10,470), cvPoint(630, 470), Scalar(0,0,255), 1, 8, 0);
    putText(sourceImage, "Phi1", cvPoint(600, 460), FONT_HERSHEY_PLAIN, 1, Scalar(255,255,255));
    putText(sourceImage, "Phi2", cvPoint(15, 15), FONT_HERSHEY_PLAIN, 1, Scalar(255,255,255));

    return;
 }

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void putAngle(Mat &sourceImage, float xx, float yy, int anglet, int length)
{

    Point P1(xx , yy);
    Point P2;

    P2.x = (int)round(P1.x + length * cos(anglet * CV_PI / 180.0));
    P2.y = (int)round(P1.y + length * sin(anglet * CV_PI / 180.0));

    line (sourceImage, P1, P2, Scalar(0,0,255), 1, 8, 0);

    return ;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void putCross(Mat &sourceImage, float xx, float yy, int anglet, int length)
{

    putAngle(sourceImage, xx,yy, anglet-90, length+10);
    for(int c=0; c<4; c++)
    {
        int compensation=90*c;
        anglet = anglet -compensation;
        int lengthn = 10;
        putAngle(sourceImage, xx,yy, anglet, length);

    }
    return;
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void putEllipse(Mat &sourceImage, float xx, float yy, float desvX, float desvY)
{
    //Factores de escalado
    int xsize = sourceImage.cols;
    int ysize = sourceImage.rows;
    int factorx = ((xsize-10)/1.5);
    int factory = ((ysize-10)/0.5);

    //Fit the coordinates in the Graph and since y grows the other way, we need to adjust the change
    int x = ((xx*factorx))+10;
    int y = ((ysize)-(yy*factory))-10;

    int width2 = ((desvX*factorx));
    int height2 = ((desvY*factory));

    //cout<<"width2"<<width2<<" "<<endl;
    //cout<<"height2"<<height2<<" "<<endl;

    ellipse(sourceImage, cvPoint(x,y), Size(width2,height2),0,0,360,255, 1);    
    return;
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void putPoint(Mat &sourceImage, float xx, float yy)
{
    //Factores de escalado
    int xsize = sourceImage.cols;
    int ysize = sourceImage.rows;
 
    int factorx = ((xsize-10)/1.5);
    int factory = ((ysize-10)/0.5);
 
    //Fit the coordinates in the Graph and since y grows the other way, we need to adjust the change
    int x = ((xx*factorx))+10;
    int y = ((ysize)-(yy*factory))-10;

    //cout << "x: " << x << endl;
    //cout << "y: " << y << endl;
    circle(sourceImage, cvPoint(x,y), 3, Scalar(0,0,255), -1);
    return;
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
#endif // GRAPHS_H

