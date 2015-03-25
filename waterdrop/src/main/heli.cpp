#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SDL/SDL.h"
#include <stdlib.h>
#include "CHeli.h"
#include <unistd.h>
#include <stdio.h>

#include <string>
#include <sstream>

#include <iostream>
#include <iomanip>

#include <time.h>
#include <ctime>

using namespace cv;
using namespace std;

struct pix {
  int x;
  int y;

  pix *next=NULL;
};

void blackAndWhite(const Mat &sourceImage, Mat &destinationImage);
void binarize(const Mat &sourceImage, Mat &destinationImage);
int discreteSpiral(const Mat &sourceImage, int spiral_width);
int waterDrop(const Mat &sourceImage, int xx, int yy); //Returns #of pixels of this object
void printlist(pix *head);

int main(int argc, char** argv)
{   
	time_t start = 0;
	time_t end = 0;
	long elapsed = 0;

	start = time(NULL);  // grab start time
	// show the question

    Mat frame = imread( "/home/asus/vision/gotaf2/src/main/d.png", 1 );
	cout<<"Objetos encontrados: "<<discreteSpiral(frame, 0)<<endl;

	end = time(NULL);  // grab the end time
	elapsed = start - end;  // compute elapsed time

	cout<<elapsed<<endl;
	cout<<ctime(&end)<<endl;
    return 0;
}

void binarize(const Mat &sourceImage, Mat &destinationImage)
{
    if (destinationImage.empty())
        destinationImage = Mat(sourceImage.rows, sourceImage.cols, sourceImage.type());   

    int channels = sourceImage.channels();

    for (int y = 0; y < sourceImage.rows; ++y)
    {
        uchar* sourceRowPointer = (uchar*) sourceImage.ptr<uchar>(y);
        uchar* destinationRowPointer = (uchar*) destinationImage.ptr<uchar>(y);
            for (int x = 0; x < sourceImage.cols ; ++x)
            {
                int promedio = sourceRowPointer[x * channels + 2];
                if (promedio >= 127)
                   destinationRowPointer[x * channels]= destinationRowPointer[x * channels + 1]= destinationRowPointer[x * channels + 2] = 255; 
                else
                    destinationRowPointer[x * channels]= destinationRowPointer[x * channels + 1]= destinationRowPointer[x * channels + 2] = 0;
            }
    }
    return;
}

void blackAndWhite(const Mat &sourceImage, Mat &destinationImage)
{
    if (destinationImage.empty())
        destinationImage = Mat(sourceImage.rows, sourceImage.cols, sourceImage.type());   

    int channels = sourceImage.channels();

    for (int y = 0; y < sourceImage.rows; ++y)
    {
        uchar* sourceRowPointer = (uchar*) sourceImage.ptr<uchar>(y);
        uchar* destinationRowPointer = (uchar*) destinationImage.ptr<uchar>(y);
            for (int x = 0; x < sourceImage.cols ; ++x)
            {
                int redTemp = sourceRowPointer[x * channels + 2];
                int greenTemp = sourceRowPointer[x * channels + 1];
                int blueTemp = sourceRowPointer[x * channels];
                int promedio = (redTemp + greenTemp + blueTemp)/3;
                destinationRowPointer[x * channels]= destinationRowPointer[x * channels + 1]= destinationRowPointer[x * channels + 2] = promedio;
            }
    }
    return ;
}

int discreteSpiral(const Mat &sourceImage, int spiral_width)
{   
    int totalObjects=0;
    int channels = sourceImage.channels();                                                                                    
    uchar* p;

    int ysize = sourceImage.rows;
    int xsize = sourceImage.cols;

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
                    cout <<waterDrop(sourceImage, x, y)<<endl;
                    totalObjects++; 
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

int waterDrop(const Mat &sourceImage, int xx, int yy)
{
    int channels = sourceImage.channels();
    uchar* p;
    int ysize = sourceImage.rows;
    int xsize = sourceImage.cols;
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
        int m10	= xx; 		 // Z xi
        int m01	= yy; 		 // Z yi
        int m20	= xx*xx; 	 // Z (xi)^2
        int m02	= yy*yy; 	 // Z (yi)^2
        int m11	= xx*yy; 	 // ZZ (xi*yi)


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
                    m11	+= arrc[0]*arrc[1];


                    pix *ptail=new pix;
                    ptail->x = arrc[0];
                    ptail->y = arrc[1];
                    ptail->next = NULL;
                    //Update tail
                    tail->next = ptail;
                    tail = ptail;
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

    cout<<"m10: "<< m10 << endl;
    cout<<"m01: "<< m01 << endl;
    cout<<"m20: "<< m20 << endl;
    cout<<"m02: "<< m02 << endl;
    cout<<"m11: "<< m11 << endl;
    int xtestada = (m10/finalCounter);
    int ytestada = (m01/finalCounter);
    cout<<"Centroide X: "<<xtestada<<endl;
    cout<<"Centroide Y: "<<ytestada<<endl;

    //line(canvas[i], Point(10+c,182), Point(1+c,182), Scalar(2*c,2*c,2*c), 1, 8,0);
    //line(sourceImage, Point(xtestada-20,ytestada),Point(xtestada+20,ytestada),Scalar(0,0,255),1,8,0);
    //line(sourceImage, Point(xtestada,ytestada-20),Point(xtestada,ytestada+20),Scalar(0,0,255),1,8,0);
    return finalCounter;
}


void printlist(pix *head)
{
    pix *checker = head;

    while(checker!=NULL)
    {
        cout<<"("<<(checker->x)<<","<<(checker->y)<<")->";
        checker=checker->next;
    }

    cout<<"NULL"<<endl;
    return;
}

