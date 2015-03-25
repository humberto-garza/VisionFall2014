#ifndef FILTERS_H
#include <stdio.h>
#include <iostream>
#define FILTERS_H

using namespace std;
using namespace cv;

void binariza(const Mat &sourceImage, Mat &destinationImage, int point);
void RGBtoYIQ(const Mat &sourceImage,Mat &destinationImage, float * valYIQ)  ;
void RGBsample(const Mat &sourceImage, int *globalXY, int *valRGB, bool &status);
void pointRGBtoYIQ(int * valRGB, float * valYIQ);
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void binariza(const Mat &sourceImage, Mat &destinationImage, int point)
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
                int suma = sourceRowPointer[x * channels + 2]+sourceRowPointer[x * channels + 1]+sourceRowPointer[x * channels + 0];
        

                if (suma >= (250+250+250))
                   destinationRowPointer[x * channels]= destinationRowPointer[x * channels + 1]= destinationRowPointer[x * channels + 2] = 0; 
                else
                    destinationRowPointer[x * channels]= destinationRowPointer[x * channels + 1]= destinationRowPointer[x * channels + 2] = 255;
                
            }

    }
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void RGBtoYIQ(const Mat &sourceImage,Mat &destinationImage, float * valYIQ)  
{   
    float valY = valYIQ[0];
    float valI = valYIQ[1];
    float valQ = valYIQ[2];



    //YIQ coefficients
    const float FLOAT_TO_BYTE = 255.0f;
    const float BYTE_TO_FLOAT = 1.0f / FLOAT_TO_BYTE;
    const float MIN_I = -0.5957f;
    const float MIN_Q = -0.5226f;
    const float Y_TO_BYTE = 255.0f;
    const float I_TO_BYTE = 255.0f / (MIN_I * -2.0f);
    const float Q_TO_BYTE = 255.0f / (MIN_Q * -2.0f);

    float fR, fG, fB;
    float fY, fI, fQ;

    bool YIQCondition;

    if (destinationImage.empty())
        destinationImage = Mat(sourceImage.rows, sourceImage.cols, sourceImage.type());
    
    int channels = sourceImage.channels();
    
    for (int y = 0; y < sourceImage.rows; ++y) 
    {
        uchar* sourceRowPointer = (uchar*) sourceImage.ptr<uchar>(y);
        uchar* destinationRowPointer = (uchar*) destinationImage.ptr<uchar>(y);


        for (int x = 0; x < sourceImage.cols ; ++x)
        {   
                // Get pixel's RGB. 
                int redTemp = sourceRowPointer[x * channels + 2];
                int greenTemp = sourceRowPointer[x * channels + 1];
                int blueTemp = sourceRowPointer[x * channels];
                
                // Typecast
                fR = redTemp * BYTE_TO_FLOAT;
                fG = greenTemp * BYTE_TO_FLOAT;
                fB = blueTemp * BYTE_TO_FLOAT;
                
                // RGB to YIQ formula.
                fY =    0.299 * fR +    0.587 * fG +    0.114 * fB;
                fI = 0.595716 * fR - 0.274453 * fG - 0.321263 * fB;
                fQ = 0.211456 * fR - 0.522591 * fG + 0.311135 * fB; 
                
/*              YIQCondition = (fY < (valY + .15)) && (fY > (valY - .15)) &&
                              (fI < (valI + .11)) && (fI > (valI - .11)) &&
                              (fQ < (valQ + .11)) && (fQ > (valQ - .11));
*/
                YIQCondition = (fY < (valY + .18)) && (fY > (valY - .18)) &&
                              (fI < (valI + .14)) && (fI > (valI - .14)) &&
                              (fQ < (valQ + .14)) && (fQ > (valQ - .14));

/*               YIQCondition = (fY < (valY + .20)) && (fY > (valY - .20)) &&
                              (fI < (valI + .16)) && (fI > (valI - .16)) &&
                              (fQ < (valQ + .16)) && (fQ > (valQ - .16));*/


                if (YIQCondition)
                {  
                    destinationRowPointer[x * channels + 2] = sourceRowPointer[x * channels +2];
                    destinationRowPointer[x * channels + 1] = sourceRowPointer[x * channels +1];      
                    destinationRowPointer[x * channels + 0] = sourceRowPointer[x * channels];
                }
                else
                {
                    destinationRowPointer[x * channels + 2] = 255;
                    destinationRowPointer[x * channels + 1] =255;
                    destinationRowPointer[x * channels + 0] =255;
                }

        }       
    }
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void RGBsample(const Mat &sourceImage, int *globalXY, int *valRGB, bool &   status)
{ 

        int channels = sourceImage.channels();
        uchar* sourceRowPointer2 = (uchar*) sourceImage.ptr<uchar>(globalXY[1]);
        if (status)
        {       

             valRGB[0] = sourceRowPointer2[(globalXY[0] ) * channels + 2];
             valRGB[1] = sourceRowPointer2[(globalXY[0] ) * channels + 1];
             valRGB[2] = sourceRowPointer2[( globalXY[0] ) * channels + 0];
             //cout << "R:" << red << " G: " << green << " B: " << blue << endl;
        }


}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void pointRGBtoYIQ(int * valRGB, float * valYIQ)
{
    const float FLOAT_TO_BYTE = 255.0f;
    const float BYTE_TO_FLOAT = 1.0f / FLOAT_TO_BYTE;
    const float MIN_I = -0.5957f;
    const float MIN_Q = -0.5226f;
    const float Y_TO_BYTE = 255.0f;
    const float I_TO_BYTE = 255.0f / (MIN_I * -2.0f);
    const float Q_TO_BYTE = 255.0f / (MIN_Q * -2.0f);

    float fR, fG, fB;

    fR = valRGB[0]*BYTE_TO_FLOAT;
    fB = valRGB[2]*BYTE_TO_FLOAT;
    fG = valRGB[1]*BYTE_TO_FLOAT;

    // RGB to YIQ formula.
    valYIQ[0] =    0.299 * fR +    0.587 * fG +    0.114 * fB;
    valYIQ[1]= 0.595716 * fR - 0.274453 * fG - 0.321263 * fB;
    valYIQ[2] = 0.211456 * fR - 0.522591 * fG + 0.311135 * fB; 

    //cout << "Y:" << valYIQ[0] << " I: " << valYIQ[1] << " Q: " << valYIQ[2] << endl;
    cout<<"Muestra lista"<<endl;

}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#endif //FILTERS_H
