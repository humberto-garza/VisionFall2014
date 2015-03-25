#ifndef OILDROP_H
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
#define OILDROP_H

using namespace std;
using namespace cv;

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Structure//////////////////////////////////////////////
struct pix {
  int x;
  int y;

  pix *next=NULL;
};
///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Methods////////////////////////////////////////////////
figure discreteSpiral(const Mat &sourceImage, int spiral_width);    //Training
int discreteSpiral( Mat &sourceImage, int spiral_width, chain *&head, chain *&tail);       //Reconnaissance
figure waterDrop(const Mat &sourceImage, int xx, int yy);           //Returns #of pixels of this object
figure waterDrop2( Mat &sourceImage, int xx, int yy);           //Returns #of pixels of this object

///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////Reconnaissance////////////////////////////////////////////
int discreteSpiral( Mat &sourceImage, int spiral_width, chain *&head, chain *&tail)
{   
    int totalObjects=0;

    int channels = sourceImage.channels();                                                                                    
    uchar* p;


    int ysize = sourceImage.rows;
    int xsize = sourceImage.cols;
    int areacond= ysize*xsize/16;

    int cont  = 1;
    int sign  = -1;

    int coor[2]  = {ysize/2,xsize/2};
    int coorM[2] = {ysize,xsize};

    int corners4 = 0;

    while (corners4 < 4)
    {
        for(int k=0; k<2; k++)
        {

            for(int c=0; c<cont;c++)
            {
                int aux = coor[k]+(sign*(2+spiral_width));
                coor[k]=aux;

                if (aux < 0)
                    coor[k] = 0;
                else if (aux>= coorM[k])
                    coor[k]=coorM[k]-1;

                int x = coor[1];
                int y = coor[0];

                p = (uchar*) sourceImage.ptr<uchar>(y);
                if( p[x * channels + 2] == 255)
                {
                    figure currentObject = waterDrop2(sourceImage, x, y);
                    if(currentObject.getArea()>areacond)
                    {                   
                        if(head==NULL)
                        {
                            head=new chain;
                            tail = head;
                            tail->next = NULL;
                            head->x=currentObject.getPhi1();
                            head->y=currentObject.getPhi2();

                        }
                        else
                        {
                            tail->next = new chain;
                            tail = tail->next;
                            tail->x=currentObject.getPhi1();
                            tail->y=currentObject.getPhi2();
                        }
                        totalObjects++; 
                    }
                    
                }

                /*Paint Pixel     
                int channels = sourceImage.channels();                                                                                    
                uchar* p = (uchar*) sourceImage.ptr<uchar>(y);
                p[x * channels + 2] = 255;
                p[x * channels + 1] = 255;
                p[x * channels + 0] = 0;
                */
                if( (x==0 && y==0) || (x==(xsize-1) && y==0) || (x==0 && y==(ysize-1)) || (x==(xsize-1) && y==(ysize-1)) ) //If reacherd any corner
                {
                    corners4++; 
                    //If a corner is reached, do not increase further
                    break;
                    
                }
            }
            //Change Sign every two steps of K
            if(k==0)
            sign = sign * (-1);
        }
        
        cont++;
    }

    //Output the discrete Spiral
    imwrite( "/home/asus/Desktop/test.jpg", sourceImage);

    return totalObjects;
}
///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////Training///////////////////////////////////////////////////
figure discreteSpiral(const Mat &sourceImage, int spiral_width)
{   
    figure auxo;
    int totalObjects=0;
    int channels = sourceImage.channels();
    figure bigaux;       
    int maxarea=0;                                                                             
    uchar* p;

    int ysize = sourceImage.rows;
    int xsize = sourceImage.cols;
    //int condarea = ysize*xsize/16;

    int cont  = 1;
    int sign  = -1;

    int coor[2]  = {ysize/2,xsize/2};
    int coorM[2] = {ysize,xsize};

    int corners4 = 0;

    while (corners4 < 4)
    {
        for(int k=0; k<2; k++)
        {

            for(int c=0; c<cont;c++)
            {
                int aux = coor[k]+(sign*(2+spiral_width));
                coor[k]=aux;

                if (aux < 1)
                    coor[k] = 1;
                else if (aux>= coorM[k])
                    coor[k]=coorM[k]-2;
 
                int x = coor[1];
                int y = coor[0];

                p = (uchar*) sourceImage.ptr<uchar>(y);

                //If seed found (white)
                if( p[x * channels + 2] == 255)
                {
                    auxo =  waterDrop(sourceImage, x, y);
                    //if it returns -1 it means that the object may be just noise (too small) see method to modify the conditional size
                    
                    if(auxo.getArea()>maxarea)
                        bigaux=auxo;

                }

                /*Paint Pixel     
                int channels = sourceImage.channels();                                                                                    
                uchar* p = (uchar*) sourceImage.ptr<uchar>(y);
                p[x * channels + 2] = 255;
                p[x * channels + 1] = 255;
                p[x * channels + 0] = 0;
                */
                if( (x==1 && y==1) || (x==(xsize-2) && y==1) || (x==1 && y==(ysize-2)) || (x==(xsize-2) && y==(ysize-2)) ) //If reacherd any corner
                {
                    corners4++; 
                    //If a corner is reached, do not increase further
                    break;
                    
                }
            }
            //Change Sign every two steps of K
            if(k==0)
            sign = sign * (-1);
        }
        
        cont++;
    }

    //Output the discrete Spiral

    imwrite( "/home/asus/Desktop/test.jpg", sourceImage);

    return bigaux;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
figure waterDrop(const Mat &sourceImage, int xx, int yy)
{
    int channels = sourceImage.channels();
    uchar* p;
    int ysize = sourceImage.rows;
    int xsize = sourceImage.cols;
 
    int totalcond = xsize*ysize;
    totalcond = totalcond/64;
 
    int curx;
    int cury;
    bool visited=false;
 
////Create unique color with mathematic relationship from the coordinates
    int r = (xx*100)%255;
    int g = (yy*100)%255;
    int b = (100+(xx*yy))%255;
 
////Create new object, same for head and tail
    pix *head=new pix;
    pix *tail=head;
 
////Assign values for the first object
    head->x=xx;
    head->y=yy;
    head->next=NULL;
 
////Paint first pixel with respective color
        p = (uchar*) sourceImage.ptr<uchar>(yy);
        p[xx * channels + 2] = r;
        p[xx * channels + 1] = g;
        p[xx * channels + 0] = b;
 
        int finalCounter = 1;// m00
        int m10 = xx;        // Z xi
        int m01 = yy;        // Z yi
        int m20 = xx*xx;     // Z (xi)^2
        int m02 = yy*yy;     // Z (yi)^2
        int m11 = xx*yy;     // ZZ (xi*yi)
 
 
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
                        p = (uchar*) sourceImage.ptr<uchar>(arrc[1]);
                        p[arrc[0] * channels + 2] = r;
                        p[arrc[0] * channels + 1] = g;
                        p[arrc[0] * channels + 0] = b;
                        //cout<<"Just painted: ("<<curx<<","<<cury<<")"<<endl;
                        finalCounter++;
                        m10 += arrc[0];
                        m01 += arrc[1];
                        m20 += arrc[0]*arrc[0];
                        m02 += arrc[1]*arrc[1];
                        m11 += arrc[0]*arrc[1];
 
 
                        pix *ptail=new pix;
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
        pix *toDelete = head;
        head = head->next;
        delete toDelete;
        toDelete = NULL;
    }
 
    if(finalCounter>totalcond)
    {
        figure auxo;
        //cout<<"m10: "<< m10 << endl;
        //cout<<"m01: "<< m01 << endl;
        //cout<<"m20: "<< m20 << endl;
        //cout<<"m02: "<< m02 << endl;
        //cout<<"m11: "<< m11 << endl;
        float xtestada = (m10/(finalCounter*1.0));
        float ytestada = (m01/(finalCounter*1.0));

        //cout<<"Centroide X: "<<xtestada<<endl;
        //cout<<"Centroide Y: "<<ytestada<<endl;
   
        //Primer Orden
        int u10 = 0;
        int u01 = 0;
   
        //Segundo Orden
        float u20 = m20 - m10 * xtestada;
        float u02 = m02 - m01 * ytestada;
        float u11 = m11 - m10 * ytestada;
   
        //cout << "u20 " << u20 << endl;
        //cout << "u02 " << u02 << endl;
        //cout << "u11 " << u11 << endl;
       
        float n20 = (u20*1.0)/(finalCounter*finalCounter);
        float n11 = (u11*1.0)/(finalCounter*finalCounter);
        float n02 = (u02*1.0)/(finalCounter*finalCounter);
       
        //cout << "n20 " << n20 << endl;
        //cout << "n02 " << n02 << endl;
        //cout << "n11 " << n11 << endl;
   
        float phi1 = n20 + n02;
        float phi2 = (n20 - n02)*(n20 - n02) + 4*(n11)*(n11);
   
        float angle = 0.5*atan2(2*u11, u20-u02)*180/3.1416;
   
        auxo.setArea(finalCounter);
        auxo.setAngle(angle);
        auxo.setCenterX(xtestada);
        auxo.setCenterY(ytestada);
        auxo.setColorX(xx);
        auxo.setColorY(yy);
        auxo.setPhi1(phi1);
        auxo.setPhi2(phi2);

        //cout << "phi1: " << phi1 << endl;
        //cout << "phi2: " << phi2 << endl;
        //cout << "angle: " << angle << endl;


        if((phi1 < 0.1892 + 0.02)&& (phi1 > 0.1892 - 0.02))
        {
            //cout << "Corazón " << endl;
            auxo.setId(1);
        }
   
        if((phi1 < 0.2318 + 0.007)&& (phi1 > 0.2318 - 0.007))
        {
            //cout << "Letra R " << endl;
            auxo.setId(2);
        }

        if((phi1 < 1.1097 + 0.07)&& (phi1 > 1.1097 - 0.07))
        {
            //cout << "Letra V " << endl;
            auxo.setId(3);
        }

        if((phi1 < 0.5983 + 0.04)&& (phi1 > 0.5983 - 0.04))
        {
            //cout << "Letra I " << endl;
            auxo.setId(4);
        }

        // if(finalCounter>3000)
        // {
        //     for(int c=ytestada-25; c<ytestada+25; c++)
        //     {
        //         p = (uchar*) sourceImage.ptr<uchar>(c);
        //         p[xtestada * channels + 2] = 255;
        //         p[xtestada * channels + 1] = 0;
        //         p[xtestada * channels + 0] = 0;
        //     }
       
        //     for(int c=xtestada-25; c<xtestada+25; c++)
        //     {
        //         p = (uchar*) sourceImage.ptr<uchar>(ytestada);
        //         p[c * channels + 2] = 255;
        //         p[c * channels + 1] = 0;
        //         p[c * channels + 0] = 0;
        //     }
        // }
        return auxo;
    }    
        else
        {
            figure auxo;
            return auxo;
        }
           
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
figure waterDrop2( Mat &sourceImage, int xx, int yy)
{
    int channels = sourceImage.channels();
    uchar* p;
    int ysize = sourceImage.rows;
    int xsize = sourceImage.cols;
 
    int totalcond = xsize*ysize;
    totalcond = totalcond/64;
 
    int curx;
    int cury;
    bool visited=false;
 
////Create unique color with mathematic relationship from the coordinates
    int r = (xx*100)%255;
    int g = (yy*100)%255;
    int b = (100+(xx*yy))%255;
 
////Create new object, same for head and tail
    pix *head=new pix;
    pix *tail=head;
 
////Assign values for the first object
    head->x=xx;
    head->y=yy;
    head->next=NULL;
 
////Paint first pixel with respective color
        p = (uchar*) sourceImage.ptr<uchar>(yy);
        p[xx * channels + 2] = r;
        p[xx * channels + 1] = g;
        p[xx * channels + 0] = b;
 
        int finalCounter = 1;// m00
        int m10 = xx;        // Z xi
        int m01 = yy;        // Z yi
        int m20 = xx*xx;     // Z (xi)^2
        int m02 = yy*yy;     // Z (yi)^2
        int m11 = xx*yy;     // ZZ (xi*yi)
 
 
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
                        p = (uchar*) sourceImage.ptr<uchar>(arrc[1]);
                        p[arrc[0] * channels + 2] = r;
                        p[arrc[0] * channels + 1] = g;
                        p[arrc[0] * channels + 0] = b;
                        //cout<<"Just painted: ("<<curx<<","<<cury<<")"<<endl;
                        finalCounter++;
                        m10 += arrc[0];
                        m01 += arrc[1];
                        m20 += arrc[0]*arrc[0];
                        m02 += arrc[1]*arrc[1];
                        m11 += arrc[0]*arrc[1];
 
 
                        pix *ptail=new pix;
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
        pix *toDelete = head;
        head = head->next;
        delete toDelete;
        toDelete = NULL;
    }
 
    if(finalCounter>totalcond)
    {
        figure auxo;
        //cout<<"m10: "<< m10 << endl;
        //cout<<"m01: "<< m01 << endl;
        //cout<<"m20: "<< m20 << endl;
        //cout<<"m02: "<< m02 << endl;
        //cout<<"m11: "<< m11 << endl;
        float xtestada = (m10/(finalCounter*1.0));
        float ytestada = (m01/(finalCounter*1.0));

        //cout<<"Centroide X: "<<xtestada<<endl;
        //cout<<"Centroide Y: "<<ytestada<<endl;
   
        //Primer Orden
        int u10 = 0;
        int u01 = 0;
   
        //Segundo Orden
        float u20 = m20 - m10 * xtestada;
        float u02 = m02 - m01 * ytestada;
        float u11 = m11 - m10 * ytestada;
   
        //cout << "u20 " << u20 << endl;
        //cout << "u02 " << u02 << endl;
        //cout << "u11 " << u11 << endl;
       
        float n20 = (u20*1.0)/(finalCounter*finalCounter);
        float n11 = (u11*1.0)/(finalCounter*finalCounter);
        float n02 = (u02*1.0)/(finalCounter*finalCounter);
       
        //cout << "n20 " << n20 << endl;
        //cout << "n02 " << n02 << endl;
        //cout << "n11 " << n11 << endl;
   
        float phi1 = n20 + n02;
        float phi2 = (n20 - n02)*(n20 - n02) + 4*(n11)*(n11);
   
        float angle = 0.5*atan2(2*u11, u20-u02)*180/3.1416;
   
        auxo.setArea(finalCounter);
        auxo.setAngle(angle);
        auxo.setCenterX(xtestada);
        auxo.setCenterY(ytestada);
        auxo.setColorX(xx);
        auxo.setColorY(yy);
        auxo.setPhi1(phi1);
        auxo.setPhi2(phi2);


        putCross(sourceImage, xtestada, ytestada, angle, 10); 

        //cout << "phi1: " << phi1 << endl;
        //cout << "phi2: " << phi2 << endl;
        //cout << "angle: " << angle << endl;


        if((phi1 < 0.1892 + 0.02)&& (phi1 > 0.1892 - 0.02))
        {
            //cout << "Corazón " << endl;
            auxo.setId(1);
        }
   
        if((phi1 < 0.2318 + 0.007)&& (phi1 > 0.2318 - 0.007))
        {
            //cout << "Letra R " << endl;
            auxo.setId(2);
        }

        if((phi1 < 1.1097 + 0.07)&& (phi1 > 1.1097 - 0.07))
        {
            //cout << "Letra V " << endl;
            auxo.setId(3);
        }

        if((phi1 < 0.5983 + 0.04)&& (phi1 > 0.5983 - 0.04))
        {
            //cout << "Letra I " << endl;
            auxo.setId(4);
        }

        // if(finalCounter>3000)
        // {
        //     for(int c=ytestada-25; c<ytestada+25; c++)
        //     {
        //         p = (uchar*) sourceImage.ptr<uchar>(c);
        //         p[xtestada * channels + 2] = 255;
        //         p[xtestada * channels + 1] = 0;
        //         p[xtestada * channels + 0] = 0;
        //     }
       
        //     for(int c=xtestada-25; c<xtestada+25; c++)
        //     {
        //         p = (uchar*) sourceImage.ptr<uchar>(ytestada);
        //         p[c * channels + 2] = 255;
        //         p[c * channels + 1] = 0;
        //         p[c * channels + 0] = 0;
        //     }
        // }
        return auxo;
    }    
        else
        {
            figure auxo;
            return auxo;
        }
           
}
///////////////////////////////////////////////////////////////////////////////////////////





#endif // OILDROP_H

