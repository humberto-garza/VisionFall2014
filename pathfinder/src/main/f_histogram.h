
#ifndef F_HISTOGRAM_H
#define F_HISTOGRAM_H

void showHistogram(Mat& img, const char* wname, const int sourceX, const int sourceY);
void showHistogram(Mat& img, const char* wname, const int sourceX, const int sourceY, const Vec3b components);

void showHistogram(Mat& img, const char* wname, const int sourceX, const int sourceY)
{
    int bins = 256;             // number of bins
    int nc = img.channels();    // number of channels

    vector<Mat> hist(nc);       // histogram arrays

    // Initalize histogram arrays
    for (unsigned int i = 0; i < hist.size(); i++)
        hist[i] = Mat::zeros(1, bins, CV_32SC1);

    // Calculate the histogram of the image
    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            for (int k = 0; k < nc; k++)
            {
                uchar val = nc == 1 ? img.at<uchar>(i,j) : img.at<Vec3b>(i,j)[k];
                hist[k].at<int>(val) += 1;
            }
        }
    }

    // For each histogram arrays, obtain the maximum (peak) value to normalize the display later
    int hmax[3] = {0,0,0};
    int hgeneralmax = 0;
    for (int i = 0; i < nc; i++)
    {
        for (int j = 0; j < bins-1; j++) {
            hmax[i] = hist[i].at<int>(j) > hmax[i] ? hist[i].at<int>(j) : hmax[i];
            hgeneralmax = hist[i].at<int>(j) > hgeneralmax ? hist[i].at<int>(j) : hgeneralmax;
        }
    }
    /*    //adding the following FOR makes the height proportional to the absolute max of all channels 
    for (int i = 0; i < nc; i++)
    {   
        hmax[i] = hgeneralmax;
    }
    */

    //const char* wname[3] = { "Channel0", "Channel1", "Channel2" };
    Scalar colors[3] = { Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255) };

    vector<Mat> canvas(nc);

    // Display each histogram in a canvas
    for (int i = 0; i < nc; i++)
    {
        canvas[i] = Mat::ones(125, bins, CV_8UC3);

        for (int j = 0, rows = canvas[i].rows; j < bins-1; j++)
        {
            line(
                canvas[i], 
                Point(j, rows), 
                Point(j, rows - (hist[i].at<int>(j) * rows/hmax[i])), 
                nc == 1 ? Scalar(200,200,200) : colors[i], 
                1, 8, 0
            );
        }

        imshow(nc == 1 ? "value" : &wname[i*2], canvas[i]);
        cvMoveWindow(nc == 1 ? "value" : &wname[i*2], sourceX+650,sourceY+i*155);
    }
} //showHistogram

void showHistogram(Mat& img, const char* wname, const int sourceX, const int sourceY, const Vec3b components)
{
    int bins = 256;             // number of bins
    int nc = img.channels();    // number of channels

    vector<Mat> hist(nc);       // histogram arrays

    // Initalize histogram arrays
    for (unsigned int i = 0; i < hist.size(); i++)
        hist[i] = Mat::zeros(1, bins, CV_32SC1);

    // Calculate the histogram of the image
    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            for (int k = 0; k < nc; k++)
            {
                uchar val = nc == 1 ? img.at<uchar>(i,j) : img.at<Vec3b>(i,j)[k];
                hist[k].at<int>(val) += 1;
            }
        }
    }


    // For each histogram arrays, obtain the maximum (peak) value
    // Needed to normalize the display later
    int hmax[3] = {0,0,0};
    int hgeneralmax = 0;
    for (int i = 0; i < nc; i++)
    {
        for (int j = 0; j < bins-1; j++) {
            hmax[i] = hist[i].at<int>(j) > hmax[i] ? hist[i].at<int>(j) : hmax[i];
            hgeneralmax = hist[i].at<int>(j) > hgeneralmax ? hist[i].at<int>(j) : hgeneralmax;
        }
    }

    //const char* wname[3] = { "Channel0", "Channel1", "Channel2" };
    Scalar colors[3] = { Scalar(255,0,0), Scalar(0,255,0), Scalar(0,0,255) };

    vector<Mat> canvas(nc);
    int alto=60;
    int ancho=35;
    // Display each histogram in a canvas
    for (int i = 0; i < nc; i++)
    {
        canvas[i] = Mat::ones(125+alto, bins+ancho, CV_8UC3);//alto,ancho

        for (int j = 0, rows = canvas[i].rows; j < bins-1; j++)
        {
            line(
                canvas[i], 
                Point(j, rows), 
                Point(j, rows - (hist[i].at<int>(j) * rows/hmax[i])), j == components[i] ? Scalar(255,255,255) : colors[i], 
                1, 8, 0
            );
        }

        char buffer[256]={0};
        sprintf(buffer, "%i", hmax[i]);

        putText(canvas[i], " 0               127            255", Point(15,180), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(250,200,200), 1);
        putText(canvas[i], buffer, Point(1,15), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(250,200,200), 1);
       
       int valorX = 0;
        for(int c = 0; c < 235; c++)
        {
            line(canvas[i], Point(10+c,182), Point(1+c,182), Scalar(2*c,2*c,2*c), 1, 8,0);
            valorX = 10 + c;
        }


        imshow(nc == 1 ? "value" : &wname[i*2], canvas[i]);
        cvMoveWindow(nc == 1 ? "value" : &wname[i*2], 100, 200+(i+1)*(155+alto));//derecha, abajo
    }
} //showHistogram

#endif