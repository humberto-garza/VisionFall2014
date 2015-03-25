#ifndef RAINBOX_H
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
#include <graphs.h>
#define RAINBOX_H

using namespace std;
using namespace cv;
///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Structure//////////////////////////////////////////////
struct pixr {
  int x;
  int y;

  pixr *next=NULL;
};
///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Methods////////////////////////////////////////////////
void rainbox(Mat &sourceImage, Mat &fake, int xx, int yy);     //Returns #of pixels of this object
void trayectory(Mat &sourceImage, Mat &fake, int xx, int yy, int &xxd, int &yyd);  
void trayectorys(Mat &sourceImage, Mat &fake, int xx, int yy, int &xxd, int &yyd);
void airtrack(Mat &sourceImage, Mat &fake, int xx, int yy, int &xxd, int &yyd, string lr);
void scape(Mat &fake, int xx, int yy, int xxd, int yyd, string lr);
void rgbcode(int &r, int &g, int &b, int num);
void colorcode(int r, int g, int b, int &num);
int pixValue(int x, int y,  Mat &sourceImage);
///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////Rainbox////////////////////////////////////////////
void rainbox(Mat &sourceImage, Mat &fake, int xx, int yy)
{
    int num=0;
    int channels = sourceImage.channels();
    uchar* p;
    int ysize = sourceImage.rows;
    int xsize = sourceImage.cols;
 
    int curx;
    int cury;
    bool visited=false;
 
////Create unique color with mathematic relationship from the coordinates

    int r = 0;
    int g = 0;
    int b = 0;
 
////Create new object, same for head and tail
    pixr *head=new pixr;
    pixr *tail=head;
 
////Assign values for the first object
    head->x=xx;
    head->y=yy;
    head->next=NULL;
 
////Paint first pixel with respective color
        p = (uchar*) sourceImage.ptr<uchar>(yy);
        p[xx * channels + 2] = r;
        p[xx * channels + 1] = g;
        p[xx * channels + 0] = b;
 
    while(head!=NULL)
    {
        curx = head->x;
        cury = head->y;
 
    ////Check NORTH->WEST->SOUTH->EAST
        int sign = -1;
        for(int k=0; k<2; k++)
        {
            for(int c=1; c>=0; c--)
            {
                int arrc[2]={curx, cury};
                arrc[c]=arrc[c]+(1*sign);
 
                //If it is out of range skip it
                if(arrc[0]>0 && arrc[0]<xsize && arrc[1]>0 && arrc[1]<ysize )
                {
                    //If not painted, then create new object and add as last
                    p = (uchar*) sourceImage.ptr<uchar>(arrc[1]);
                    int compare = p[(arrc[0]) * channels + 2] + p[(arrc[0]) * channels + 1]+ p[(arrc[0]) * channels + 0];
                    if(compare== (255+255+255) )
                    {
                    ////Paint head pixel with respective color
                        p = (uchar*) sourceImage.ptr<uchar>(cury);
                        r = p[curx * channels + 2];
                        g = p[curx * channels + 1];
                        b = p[curx * channels + 0];
                        //cout<<num<<endl;
                        colorcode(r,g,b,num);
                        num++;
                        rgbcode(r,g,b,num);

                        p = (uchar*) sourceImage.ptr<uchar>(arrc[1]);
                        p[arrc[0] * channels + 2] = r;
                        p[arrc[0] * channels + 1] = g;
                        p[arrc[0] * channels + 0] = b;

                        p = (uchar*) fake.ptr<uchar>(arrc[1]);
                        p[arrc[0] * channels + 2] = ((num*255)/1500);
                        p[arrc[0] * channels + 1] = 10;
                        p[arrc[0] * channels + 0] = 255-((num*255)/1500);  
 
                        pixr *ptail=new pixr;
                        ptail->x = arrc[0];
                        ptail->y = arrc[1];
                        ptail->next = NULL;
                        //Update tail
                        tail->next = ptail;
                        tail = ptail;
                    }
                }
 
            }
            sign = sign * -1;
        }

     
    ////Move head
        pixr *toDelete = head;
        head = head->next;
        delete toDelete;
        toDelete = NULL;
    }
    circle(fake, Point(xx,yy) , 5, Scalar(255, 255, 255),-1,  8,  0);
}
///////////////////////////////////////////////////////////////////////////////////////////
void trayectory(Mat &sourceImage, Mat &fake, int xx, int yy, int &xxd, int &yyd)
{
    int next=0;
    int actual=0;
    int objective;

    int channels = sourceImage.channels();
    uchar* p;
    int ysize = sourceImage.rows;
    int xsize = sourceImage.cols;
 
    int curx = xxd;
    int cury = yyd;
/*
    int coefx[4]={-1, 0, 1, 0}; //Izquierda
    int coefy[4]={0, -1, 0, 1};
    */
    int coefx[4]={0, -1, 0, 1}; //Izquierda
    int coefy[4]={-1, 0, 1, 0};

    int dify= yy-yyd;
    int difx= xx-xxd;

    if(dify<0)
        dify=dify*-1;
    if(difx<0)
        difx=difx*-1;


 

    int c=0;
    while(curx!=xx || cury!=yy)
    {    
        actual=pixValue(curx, cury, sourceImage);
        next=pixValue(curx+coefx[c],cury+coefy[c],sourceImage);
        //cout<<"Current: "<<curx<<", "<<cury<<" next: "<<curx+coefx[c]<<", "<<cury+coefy[c]<<" valsCoef"<<coefx[c]<<" y "<<coefy[c]<<endl;
        //cout<<"Current: "<<actual<<" next: "<<next<<endl;
        if((actual-1)==next)
        {
            circle(fake, Point(curx,cury)   , 4, Scalar(50, 50, 255),1,  4,  0);
            curx+=coefx[c];
            cury+=coefy[c];
        }
        else //Remove else for shorter trayectory
            c=(c+1)%4;
        
    }

    circle(fake, Point(xx,yy)   , 5, Scalar(255, 100, 100),-1,  8,  0);
    //circle(fake, Point(xxd,yyd) , 15, Scalar(100, 100, 255),1,  8,  0);
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void airtrack(Mat &sourceImage, Mat &fake, int xx, int yy, int &xxd, int &yyd, string lr)
{
    circle(fake, Point(xx,yy) , 15, Scalar(255, 100, 100),2,  8,  0);
    circle(fake, Point(xxd,yyd) , 15, Scalar(0, 15, 255),2,  8,  0);
    if(lr == "IZQUIERDA")
    {
        int midx=xxd/2;
        scape(fake, midx, yyd,xxd, yyd, lr);

        trayectory(sourceImage, fake, xx, yy, midx, yyd); 
    }
    if(lr == "DERECHA")
    {
        int xsize = sourceImage.cols;
        int midx=xxd+((xsize-xxd)/2);
        scape(fake, midx, yyd,xxd, yyd, lr);
        //imshow("Debug", fake);
        //waitKey();
        //circle(fake, Point(midx,yyd), 15, Scalar(255, 255, 255),1,  4,  0);
        trayectory(sourceImage, fake, xx, yy, midx, yyd); 
    }
}


void scape(Mat &fake, int xx, int yy, int xxd, int yyd, string lr)
{
    int increment = -1;
    if(lr == "DERECHA")
        increment =1;

    while(xx!=xxd)
    {
        circle(fake, Point(xxd,yyd), 4, Scalar(50, 50, 255),1,  4,  0);
        xxd+=increment;
        //imshow("Debug",fake);
        //waitKey();
    }


}

void rgbcode(int &r, int &g, int &b, int num)
{
    b = num%256;
    g = (num/256)%256;
    r = ((num/256)/256)%256; //Twice /25
}
void colorcode(int r, int g, int b, int &num)
{
    num = r*513 + g*256 + b;
}
int pixValue(int x, int y,  Mat &sourceImage)
{
    int next;
    int channels = sourceImage.channels();
    uchar* p;
    p = (uchar*) sourceImage.ptr<uchar>(y);
    int r = p[x * channels + 2];
    int g = p[x * channels + 1];
    int b = p[x * channels + 0];    

    colorcode(r,g,b,next);
    return next;
}   

////////////

void trayectorys(Mat &sourceImage, Mat &fake, int xx, int yy, int &xxd, int &yyd)
{
    int next=0;
    int actual=0;
    int objective;

    int channels = sourceImage.channels();
    uchar* p;
    int ysize = sourceImage.rows;
    int xsize = sourceImage.cols;
 
    int curx = xxd;
    int cury = yyd;

    int coefx[4]={-1, 0, 1, 0}; //Izquierda
    int coefy[4]={0, -1, 0, 1};


    int dify= yy-yyd;
    int difx= xx-xxd;

    if(dify<0)
        dify=dify*-1;
    if(difx<0)
        difx=difx*-1;


    if(difx>dify)
    {        
        coefx[0] = 0;               //Abajo
        coefx[1] = -1;
        coefx[2] = 0;
        coefx[3] = 1;

        coefy[0] = 1;
        coefy[1] = 0;
        coefy[2] = -1;
        coefy[3] = 0;

    }

    int c=0;
    while(curx!=xx || cury!=yy)
    {    
        actual=pixValue(curx, cury, sourceImage);
        next=pixValue(curx+coefx[c],cury+coefy[c],sourceImage);
        //cout<<"Current: "<<curx<<", "<<cury<<" next: "<<curx+coefx[c]<<", "<<cury+coefy[c]<<" valsCoef"<<coefx[c]<<" y "<<coefy[c]<<endl;
        //cout<<"Current: "<<actual<<" next: "<<next<<endl;
        if((actual-1)==next)
        {
            p =(uchar*) fake.ptr<uchar>(cury);
            int r = p[curx * channels + 2];
            int g = p[curx * channels + 1];
            int b = p[curx * channels + 0]; 
            int plus=50;
            if(r<(255-plus))
                r+=plus;
            if(g<(255-plus))
                g+=plus;
            if(b<(255-plus))
                b+=plus;

            p = (uchar*) fake.ptr<uchar>(cury);      //Color the neighbor
            //cout<<r<<","<<g<<","<<b<<endl;

            p[curx * channels + 2] = r;
            p[curx * channels + 1] = g;
            p[curx * channels + 0] = b;  
            curx+=coefx[c];
            cury+=coefy[c];


            coefx[0] = -1;               //Abajo
            coefx[1] = 0;
            coefx[2] = 1;
            coefx[3] = 0;

            coefy[0] = 0;
            coefy[1] = -1;
            coefy[2] = 0;
            coefy[3] = 1;


        }
        //else //Remove else for shorter trayectory
            c=(c+1)%4;
        
    }

    circle(fake, Point(xx,yy)   , 5, Scalar(255, 100, 100),-1,  8,  0);
    circle(fake, Point(xxd,yyd) , 5, Scalar(100, 100, 255),-1,  8,  0);
}
#endif // RAINBOX_H

