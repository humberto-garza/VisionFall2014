///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Generalncludes/////////////////////////////////////////
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "SDL/SDL.h"
#include <stdlib.h>
#include "CHeli.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream> 
#include <time.h>
         
//gdb ./heli  
//run    

///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Local Includes/////////////////////////////////////////
#include <oildrop.h>
#include <filters.h>
#include <figura.h>
#include <chain.h>
#include <tipo.h>
#include <graphs.h>
#include <rainbox.h>

using namespace std;
using namespace cv;

///////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////Global variables//////////////////////////////////////
bool navigatedWithJoystick, joypadTakeOff, joypadLand, joypadHover, useJoystick;
bool stop = false, status = false;
unsigned char keyControl = 0;
int joypadRoll, joypadPitch, joypadVerticalSpeed, joypadYaw, hover;

float pitch, roll, yaw, height;
SDL_Joystick* m_joystick;
vector<Point> points;
CRawImage *image;
CHeli *heli;
 
//Global X,Y coordinates
int globalXY[2] = {0};

//YIQ Values = Y, I, Q
float valYIQ[3] = {0.670777,-0.183753,-0.291983};

//RGB Values = R, G, B  
int valRGB[3]   = {0}; 
 
//Global Matrix (current Image)
Mat globalMat; 

//Left Click
int lclick=0;
char selection;
 
///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Methods////////////////////////////////////////////////
void rawToMat(Mat &destImage, CRawImage* sourceImage);
void mouseCoordinatesExampleCallback(int event, int x, int y, int flags, void* param);
void CallBackFunc(int event, int x, int y, int flags, void* userdata);
void sleepyt(int tiempos);
void sleepym(int tiempos);
///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////Main///////////////////////////////////////////////////
int main(int argc,char* argv[])
{   
	valYIQ[0] = 0.670777;
	valYIQ[1] = -0.183753;
	valYIQ[2] = -0.291983; 
    heli = new CHeli();

    //this class holds the image from the drone 
    image = new CRawImage(320,240);
 
    // Initial values for control   
    pitch = roll = yaw = height = 30.0;
    joypadPitch = joypadRoll = joypadYaw = joypadVerticalSpeed = 0.0;
 
    // Destination OpenCV Mat    
    globalMat 			= Mat(240,320, CV_8UC3);
    Mat currentImage 	= Mat(240, 320, CV_8UC3);
    Mat filtYIQ 		= Mat(240, 320, CV_8UC3);
    Mat binaryImage 	= Mat(240, 320, CV_8UC3);
    Mat eroImage 		= Mat(240, 320, CV_8UC3);
 
    //Graphs for phi-s
    Mat traingraph		= Mat(480,640,CV_8UC3);
    createGraph(traingraph);
    Mat recongrapg		= Mat(480,640,CV_8UC3);
    createGraph(recongrapg);

    //Phi arrays
    //CORAZON; NO PASE; RAYO; SHRECK
    float figure_phi1[4]={0.180866, 0.286259, 0.543154, 0.376059};
    float figure_phi2[4]={0.00337065, 0.000124718, 0.119976, 0.0497136};

    float figure_width[4]={0.0311775,0.0623227,0.0478906,0.0447636};
    float figure_height[4]={0.0302182,0.0623227,0.0436662,0.0376477};
    int timer = 0;
 

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    useJoystick = SDL_NumJoysticks() > 0;
    if (useJoystick)
    {
         SDL_JoystickClose(m_joystick);
         m_joystick = SDL_JoystickOpen(0);
    }

	float localYIQ[3] = {0.491577, -0.211053, -0.315012};
	int training_complete=0;
    while (keyControl != 27)
    {
        //Clear the console
        //printf("\033[2J\033[1;1H");

        // Copy to OpenCV Mat
        heli->renewImage(image);
        rawToMat(currentImage, image);
        rawToMat(globalMat, image);


        if (useJoystick)
        {
            SDL_Event event;
            SDL_PollEvent(&event);

            joypadRoll = SDL_JoystickGetAxis(m_joystick, 2);
            joypadPitch = SDL_JoystickGetAxis(m_joystick, 3);
            joypadVerticalSpeed = SDL_JoystickGetAxis(m_joystick, 1);
            joypadYaw = SDL_JoystickGetAxis(m_joystick, 0);
            joypadTakeOff = SDL_JoystickGetButton(m_joystick, 1);
            joypadLand = SDL_JoystickGetButton(m_joystick, 2);
            joypadHover = SDL_JoystickGetButton(m_joystick, 0);
        }
 

        // // prints the drone telemetric data, helidata struct contains drone angles, speeds and battery status
        // printf("===================== Parrot Basic Example =====================\n\n");
        // fprintf(stdout, "Angles  : %.2lf %.2lf %.2lf \n", helidata.phi, helidata.psi, helidata.theta);
        // fprintf(stdout, "Speeds  : %.2lf %.2lf %.2lf \n", helidata.vx, helidata.vy, helidata.vz);
        // fprintf(stdout, "Battery : %.0lf \n", helidata.battery);   
        // fprintf(stdout, "Hover   : %d \n", hover);    
        // fprintf(stdout, "Joypad  : %d \n", useJoystick ? 1 : 0);  
        // fprintf(stdout, "  Roll    : %d \n", joypadRoll);  
        // fprintf(stdout, "  Pitch   : %d \n", joypadPitch);
        // fprintf(stdout, "  Yaw     : %d \n", joypadYaw);   
        // fprintf(stdout, "  V.S.    : %d \n", joypadVerticalSpeed); 
        // fprintf(stdout, "  TakeOff : %d \n", joypadTakeOff);  
        // fprintf(stdout, "  Land    : %d \n", joypadLand); 
        // fprintf(stdout, "Navigating with Joystick: %d \n", navigatedWithJoystick ? 1 : 0);
   
 		//Clear Screen    
    	printf("\033[2J\033[1;1H");    
  
      	//Start the training with a number of samples    
        cout<<"Presione -m- para iniciar el mapeo"<<endl; 

        fprintf(stdout, "Battery : %.0lf \n", helidata.battery);
  		  
        // //Show Current Image  
        imshow("Current", currentImage); 
        moveWindow("Current", 5, 5);   

         //Get mouse coordinates from "Current Image"
        setMouseCallback("Current", mouseCoordinatesExampleCallback);
  
        //Apply YIQ Filter  
        RGBtoYIQ(globalMat,filtYIQ, valYIQ);
   
        //Binarize YIQ-filtered Image
        binariza(filtYIQ, binaryImage, 42);       
       
        //Erode Binarized Image 
        threshold( binaryImage, eroImage, 127, 255, 0);        
        erode(eroImage, eroImage, Mat(), Point(-1, -1), 1, 1, 1);          
        keyControl=waitKey(5); 

		if (keyControl == 'i' || keyControl == 'o' || keyControl == 'k' || keyControl == 'l' || keyControl == 'm')       
		{       	    
			selection = keyControl;
			Mat map  = imread( "../src/main/mapaobs.png", 1);  
			Mat fake = imread( "../src/main/mapaobs.png", 1);  

			imshow("Mapa Original", map);
			moveWindow("Mapa Original", 0, 0);
			waitKey(50);
			int radio = 50;
			cout<<"Radio del Paparrot: "<<endl;
			cin>>radio;


			circle(map, Point(358,275) , radio, Scalar(13, 255, 13),-1,  8,  0);  
			circle(map, Point(358,536) , radio, Scalar(13, 255, 13),-1,  8,  0);  

				circle(fake, Point(358,275) , radio, Scalar(13, 255, 13),-1,  8,  0);  
			circle(fake, Point(358,536) , radio, Scalar(13, 255, 13),-1,  8,  0);  

			imshow("Mapa Ensanchado", map);	 	
			moveWindow("Mapa Ensanchado", 700, 0);			 					
			int originx = 358;   
			int originy = 150; 

			rainbox(map, fake, originx, originy);   	
			imshow("NF1", fake);  

			/////////////////////////INICIA VUELO/////////////////////////////////
			cout<<"Inicia Etapa de Vuelo, presione cualquier tecla"<<endl;
			keyControl= waitKey();

			heli -> takeoff();
			int contador = 0;
			int stabilize=500; 
			//Despegar y estabilizar
			chain *heado = NULL;
			chain *tailo = NULL;
			int total =0;

			while(contador < stabilize)
			{
				heli->renewImage(image);
				rawToMat(currentImage, image);
				keyControl = waitKey(5);
				imshow("Vuelo", currentImage);
				moveWindow("Vuelo", 5, 5);
				//Apply YIQ Filter 
				RGBtoYIQ(currentImage,filtYIQ, localYIQ);
				//Binarize YIQ-filtered Image  
				binariza(filtYIQ, binaryImage, 42);  
				//Erode Binarized Image
				threshold( binaryImage, eroImage, 127, 255, 0);
				erode(eroImage, eroImage, Mat(), Point(-1, -1), 1, 1, 1);
				heado = NULL;
				tailo = NULL;
				total = discreteSpiral(eroImage, 5, heado, tailo);
				
				keyControl = waitKey(5);
		    	imshow("Muestra", eroImage); 
		    	moveWindow("Muestra", 400, 5);
				contador++; 
			} 

			int mov=0;

			for(int c=0; c< total; c++) 
			{
				if(heado!=NULL)  
				{
					float phi1 = heado->x;
					float phi2 = heado->y;   
					cout<<"Phi1 "<<phi1<<"_Phi2 "<<phi2<<endl;

					//cout<<c<<endl;
			        if((phi1 < figure_phi1[0] + figure_width[0])&& (phi1 > figure_phi1[0] - figure_width[0]))
			        {
						if((phi2 < figure_phi2[0] + figure_height[0])&& (phi2 > figure_phi2[0] - figure_height[0]))
						{
							cout << "CORAZON!!!!!!!" << endl;
							mov+=2;
						}
			        }
			        if((phi1 < figure_phi1[1] + figure_width[1])&& (phi1 > figure_phi1[1] - figure_width[1]))
			        {
						if((phi2 < figure_phi2[1] + figure_height[1])&& (phi2 > figure_phi2[1] - figure_height[1]))
						{
							cout << "NO PASE!!!!!!!" << endl;
							mov+=6;
						}
			        }
			        if((phi1 < figure_phi1[2] + figure_width[2])&& (phi1 > figure_phi1[2] - figure_width[2]))
			        {
						if((phi2 < figure_phi2[2] + figure_height[2])&& (phi2 > figure_phi2[2] - figure_height[2]))
						{
							cout << "VARILLA!!!!!!!" << endl;
							mov+=9;
						}
			        }
			        if((phi1 < figure_phi1[3] + figure_width[3])&& (phi1 > figure_phi1[3] - figure_width[3]))
			        {
						if((phi2 < figure_phi2[3] + figure_height[3])&& (phi2 > figure_phi2[3] - figure_height[3]))
						{
							cout << "SHREK!!!!!!!" << endl;
							mov+=14;
						}
			        }			        

					chain *aux=heado;
					heado=heado->next;
					delete aux;
				} 

			}

			char vision='n';
			if(mov==11)
			{				
				vision='i';
			}
			else if(mov==16)
			{
				vision='o';
			}
			else if(mov==15)
			{
				vision='k';
			}
			else if(mov==20)
			{
				vision='l';
			}	

			if(vision==selection)
				cout<<"INICIA MOVIMIENTO"<<endl;

			if(selection=='m')
			{
				int ohw = rand() % 3;
				if(ohw==0)
				{
					selection ='i';
				}	
				else if(ohw == 1)
				{
					selection ='o';
				}
				else if(ohw == 2)
				{
					selection ='k';
				}	
				else 
				{
					selection ='l';
				}

			}

			contador = 0;
			int destinationx = 358;
			int destinationy = 411;
			string direccion = "DERECHA";
			cout<<"TOTAL: "<<total<<selection<<endl;


			if(selection=='i')
			{
				cout << "ADELANTE POR LA IZQUIERDA" << endl;
				destinationx = 358;
				destinationy = 411;
				direccion = "IZQUIERDA";
				trayectorys(map, fake, originx, originy, destinationx, destinationy);	//Draw the possible trayectories p
				imshow("NF1", fake);
				
				//Izquierda o derecha
				airtrack(map, fake, originx, originy, destinationx, destinationy, direccion);
				imshow("NF1", fake); 
				
				moveWindow("Mapa Original", 0, 0);
				moveWindow("Mapa Ensanchado", 700, 0);
				moveWindow("NF1", 1200, 0);
				///////////////////////////////////////////////////

				printf("ADELANTE POR LA IZQUIERDA \n");
		      
		        int contador = 0;
		        int stabilize=1000;
		        int taoZ = 130;
		        int taoX = 75;
		        int taoY = 94; //80 84
		        int rot = 15;


		        //IZQUIERDA
		        //case 'a': yaw = -20000.0; hover=1; break; 	//Rota clockwise
		        roll = -20000.0; pitch=0; hover=0;                
		        heli->setAngles(pitch, roll, yaw, height, hover); 
		        while(contador < taoX)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);

		                contador++;
		        }
		        contador = 0;
		        /////////////////Estabilizar///////////////////////////////////
		        heli->setAngles(0, 0, 0, 0, 1);
		        while(contador < taoZ/2)
		        {
		            heli->renewImage(image);
		            rawToMat(currentImage, image);
		            keyControl = waitKey(5);
		            imshow("Vuelo", currentImage);
		            contador++;
		        }
		        contador = 0;
					heli->setAngles(0, 0, -20000, 0, 1); 
		        while(contador < rot)
		        {
		            heli->renewImage(image);
		            rawToMat(currentImage, image);
		            keyControl = waitKey(5);
		            imshow("Vuelo", currentImage);
		            contador++;
		        }
		        contador = 0;
					heli->setAngles(0, 0, 0, 0, 1);
				while(contador < taoZ)
				{
					heli->renewImage(image);
					rawToMat(currentImage, image); 
					keyControl = waitKey(5);
					imshow("Vuelo", currentImage);
					contador++;
				}
				contador = 0;
				////////////////////////////////////////////////////////////////


		        //Atras
		        pitch =  20000.0; roll=0; hover=0;
		        heli->setAngles(pitch, roll, yaw, height, hover);
		        while(contador < taoY)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);
		                contador++;
		        }
		        contador = 0;


		        /////////////////Estabilizar///////////////////////////////////
					heli->setAngles(0, 0, 0, 0, 1);
				while(contador < taoZ)
				{
					heli->renewImage(image);
					rawToMat(currentImage, image);
					keyControl = waitKey(5);
					imshow("Vuelo", currentImage);
					contador++;
				}
				contador = 0;
					///////////////////////////////////////////////////////////////


		        //DERECHA
		        roll = 20000.0; pitch=0; hover=0;
		        heli->setAngles(pitch, roll, yaw, height, hover);
		        while(contador < taoX+3)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);
		                contador++;
		        } 
		        contador = 0;



		        /////////////////Estabilizar///////////////////////////////////
					heli->setAngles(0, 0, 0, 0, 1);
				while(contador < taoZ)
				{
					heli->renewImage(image);
					rawToMat(currentImage, image);
					keyControl = waitKey(5);
					imshow("Vuelo", currentImage);
					contador++;
				}
				contador = 0;
					///////////////////////////////////////////////////////////////

					heli->setAngles(0, 0, 0, 20000, 1);
		        while(contador < taoZ+50)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);
		                contador++;
		        }
		        heli -> land();
				/////////////////////////////////////////////////

			}
			if(selection=='k')
			{
				cout << "ATRÁS POR LA IZQUIERDA" << endl;
				destinationx = 358;
				destinationy = 644;
				direccion = "IZQUIERDA";
				trayectorys(map, fake, originx, originy, destinationx, destinationy);	//Draw the possible trayectories p
				imshow("NF1", fake);
				
				//Izquierda o derecha
				airtrack(map, fake, originx, originy, destinationx, destinationy, direccion);
				imshow("NF1", fake); 
				
				moveWindow("Mapa Original", 0, 0);
				moveWindow("Mapa Ensanchado", 700, 0);
				moveWindow("NF1", 1200, 0);
				////////////////////////////////////////////////

		        printf("ATRAS POR LA IZQUIERDA \n");
		       
		        int contador = 0;
		        int stabilize=1000;
		        int taoZ = 130;
		        int taoX = 75;
		        int taoY = 94; //80
		        int rot = 17;
		       
		        //IZQUIERDA
		        //case 'a': yaw = -20000.0; hover=1; break; 	//Rota clockwise
		        roll = -20000.0; pitch=0; hover=0;                
		        heli->setAngles(pitch, roll, yaw, height, hover); 
		        while(contador < taoX)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);

		                contador++;
		        }
		        contador = 0;

		        /////////////////Estabilizar///////////////////////////////////
		        heli->setAngles(0, 0, 0, 0, 1);
		        while(contador < taoZ/2)
		        {
		            heli->renewImage(image);
		            rawToMat(currentImage, image);
		            keyControl = waitKey(5);
		            imshow("Vuelo", currentImage);
		            contador++;
		        }
		        contador = 0;
					heli->setAngles(0, 0, -20000, 0, 1); 
		        while(contador < rot)
		        {
		            heli->renewImage(image);
		            rawToMat(currentImage, image);
		            keyControl = waitKey(5);
		            imshow("Vuelo", currentImage);
		            contador++;
		        }
		        contador = 0;
					heli->setAngles(0, 0, 0, 0, 1);
				while(contador < taoZ)
				{
					heli->renewImage(image);
					rawToMat(currentImage, image); 
					keyControl = waitKey(5);
					imshow("Vuelo", currentImage);
					contador++;
				}
				contador = 0;
				////////////////////////////////////////////////////////////////

		        //Atras
		        pitch =  20000.0; roll=0; hover=0;
		        heli->setAngles(pitch, roll, yaw, height, hover);
		        while(contador < taoY)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);
		                contador++;
		        }
		        contador = 0;

		        /////////////////Estabilizar///////////////////////////////////
					heli->setAngles(0, 0, 0, 0, 1);
				while(contador < taoZ)
				{
					heli->renewImage(image);
					rawToMat(currentImage, image);
					keyControl = waitKey(5);
					imshow("Vuelo", currentImage);
					contador++;
				}
				contador = 0;
				///////////////////////////////////////////////////////////////

				////////////////////////////////////////////////////////////////

		        //Atras
		        pitch =  20000.0; roll=0; hover=0;
		        heli->setAngles(pitch, roll, yaw, height, hover);
		        while(contador < taoY)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);
		                contador++;
		        }
		        contador = 0;


		        /////////////////Estabilizar///////////////////////////////////
				heli->setAngles(0, 0, 0, 0, 1);
				while(contador < taoZ)
				{
					heli->renewImage(image);
					rawToMat(currentImage, image);
					keyControl = waitKey(5);
					imshow("Vuelo", currentImage);
					contador++;
				}
				contador = 0;
					///////////////////////////////////////////////////////////////

		        //DERECHA
		        roll = 20000.0; pitch=0; hover=0;
		        heli->setAngles(pitch, roll, yaw, height, hover);
		        while(contador < taoX-15)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);
		                contador++;
		        } 
		        contador = 0;



		        /////////////////Estabilizar///////////////////////////////////
					heli->setAngles(0, 0, 0, 0, 1);
				while(contador < taoZ)
				{
					heli->renewImage(image);
					rawToMat(currentImage, image);
					keyControl = waitKey(5);
					imshow("Vuelo", currentImage);
					contador++;
				}
				contador = 0;
					///////////////////////////////////////////////////////////////

					heli->setAngles(0, 0, 0, 20000, 1);
		        while(contador < taoZ+50)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);
		                contador++;
		        }
		        heli -> land();



				////////////////////////////////////////////////

			}
			if(selection=='o')
			{
				cout << "ADELANTE POR LA DERECHA" << endl;
				destinationx = 358;
				destinationy = 411;
				direccion = "DERECHA";
				trayectorys(map, fake, originx, originy, destinationx, destinationy);	//Draw the possible trayectories p
				imshow("NF1", fake);

				//Izquierda o derecha
				airtrack(map, fake, originx, originy, destinationx, destinationy, direccion);
				imshow("NF1", fake); 

				moveWindow("Mapa Original", 0, 0);
				moveWindow("Mapa Ensanchado", 700, 0);
				moveWindow("NF1", 1200, 0);
				////////////////////////////////////////////////////////


		        printf("ADELANTE POR LA DERECHA \n");
		        int contador = 0;
		        int stabilize=1000;
		        int taoZ = 130;
		        int taoX = 75;
		        int taoY = 94; 
		        int rot = 7;
		        //Derecha
		        //case 'a': yaw = -20000.0; hover=1; break; 	//Rota clockwise
		        roll =  20000.0; pitch=0; hover=0;                
		        heli->setAngles(pitch, roll, yaw, height, hover);
		        while(contador < taoX)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);

		                contador++; 
		        }
		        contador = 0;

		        /////////////////Estabilizar///////////////////////////////////
		        heli->setAngles(0, 0, 0, 0, 1);
		        while(contador < taoZ/2)
		        {
		            heli->renewImage(image);
		            rawToMat(currentImage, image);
		            keyControl = waitKey(5);
		            imshow("Vuelo", currentImage);
		            contador++;
		        }
		        contador = 0;
					heli->setAngles(0, 0, 20000, 0, 1); 
		        while(contador < rot)
		        {
		            heli->renewImage(image);
		            rawToMat(currentImage, image);
		            keyControl = waitKey(5);
		            imshow("Vuelo", currentImage);
		            contador++;
		        }
		        contador = 0;
					heli->setAngles(0, 0, 0, 0, 1);
				while(contador < taoZ)
				{
					heli->renewImage(image);
					rawToMat(currentImage, image); 
					keyControl = waitKey(5);
					imshow("Vuelo", currentImage);
					contador++;
				}
				contador = 0;
				////////////////////////////////////////////////////////////////

		        //Atras
		        pitch =  20000.0; roll=0; hover=0;
		        heli->setAngles(pitch, roll, yaw, height, hover);
		        while(contador < taoY)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);
		                contador++;
		        }
		        contador = 0;

		        /////////////////Estabilizar///////////////////////////////////
					heli->setAngles(0, 0, 0, 0, 1);
				while(contador < taoZ)
				{
					heli->renewImage(image);
					rawToMat(currentImage, image);
					keyControl = waitKey(5);
					imshow("Vuelo", currentImage);
					contador++;
				}
				contador = 0;
				///////////////////////////////////////////////////////////////

		        //Izquierda
		        roll = -20000.0; pitch=0; hover=0;
		        heli->setAngles(pitch, roll, yaw, height, hover);
		        while(contador < taoX)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);
		                contador++;
		        }
		        contador = 0;

		        /////////////////Estabilizar///////////////////////////////////
					heli->setAngles(0, 0, 0, 0, 1);
				while(contador < taoZ)
				{
					heli->renewImage(image);
					rawToMat(currentImage, image);
					keyControl = waitKey(5); 
					imshow("Vuelo", currentImage);
					contador++;
				}
				contador = 0; 
					///////////////////////////////////////////////////////////////

				heli->setAngles(0, 0, 0, 20000, 1);
		        while(contador < taoZ+50)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);
		                contador++;
		        }
		        heli -> land();


				/////////////////////////////////////////////////////////

			}
			if(selection=='l')
			{
				cout << "ATRÁS POR LA DERECHA" << endl;
				destinationx = 358; 
				destinationy = 644;
				direccion = "DERECHA";
				trayectorys(map, fake, originx, originy, destinationx, destinationy);	//Draw the possible trayectories p
				imshow("NF1", fake);
				
				//Izquierda o derecha
				airtrack(map, fake, originx, originy, destinationx, destinationy, direccion);
				imshow("NF1", fake); 
				
				moveWindow("Mapa Original", 0, 0);
				moveWindow("Mapa Ensanchado", 700, 0);
				moveWindow("NF1", 1200, 0);
				///////////////////////////////////
				 printf("ATRAS POR LA DERECHA\n");
		       
		        int contador = 0;
		        int stabilize=1000;
		        int taoZ = 130;
		        int taoX = 75;
		        int taoY = 90;
		        int rot = 3;
		       
		        //Derecha
		        //case 'a': yaw = -20000.0; hover=1; break; 	//Rota clockwise
		        roll =  20000.0; pitch=0; hover=0;                
		        heli->setAngles(pitch, roll, yaw, height, hover);
		        while(contador < taoX)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);

		                contador++;
		        }
		        contador = 0;

		        /////////////////Estabilizar///////////////////////////////////
		        heli->setAngles(0, 0, 0, 0, 1);
		        while(contador < taoZ/2)
		        {
		            heli->renewImage(image);
		            rawToMat(currentImage, image);
		            keyControl = waitKey(5);
		            imshow("Vuelo", currentImage);
		            contador++;
		        }
		        contador = 0;
					heli->setAngles(0, 0, 20000, 0, 1); 
		        while(contador < rot)
		        {
		            heli->renewImage(image);
		            rawToMat(currentImage, image);
		            keyControl = waitKey(5);
		            imshow("Vuelo", currentImage);
		            contador++;
		        }
		        contador = 0;
					heli->setAngles(0, 0, 0, 0, 1);
				while(contador < taoZ)
				{
					heli->renewImage(image);
					rawToMat(currentImage, image); 
					keyControl = waitKey(5);
					imshow("Vuelo", currentImage);
					contador++;
				}
				contador = 0;
				////////////////////////////////////////////////////////////////

		        //Atras
		        pitch =  20000.0; roll=0; hover=0;
		        heli->setAngles(pitch, roll, yaw, height, hover);
		        while(contador < taoY)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);
		                contador++;
		        }
		        contador = 0;


		        /////////////////Estabilizar///////////////////////////////////
					heli->setAngles(0, 0, 0, 0, 1);
				while(contador < taoZ)
				{
					heli->renewImage(image);
					rawToMat(currentImage, image);
					keyControl = waitKey(5);
					imshow("Vuelo", currentImage);
					contador++;
				}
				contador = 0;
				///////////////////////////////////////////////////////////////
				////////////////////////////////////////////////////////////////

		        //Atras
		        pitch =  20000.0; roll=0; hover=0;
		        heli->setAngles(pitch, roll, yaw, height, hover);
		        while(contador < taoY)
		        {
		                heli->renewImage(image);
		               rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);
		                contador++;
		        }
		        contador = 0;


		        /////////////////Estabilizar///////////////////////////////////
					heli->setAngles(0, 0, 0, 0, 1);
				while(contador < taoZ)
				{
					heli->renewImage(image);
					rawToMat(currentImage, image);
					keyControl = waitKey(5);
					imshow("Vuelo", currentImage);
					contador++;
				}
				contador = 0;
				///////////////////////////////////////////////////////////////

		        //Izquierda
		        roll = -20000.0; pitch=0; hover=0;
		        heli->setAngles(pitch, roll, yaw, height, hover);
		        while(contador < taoX)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);
		                contador++;
		        } 
		        contador = 0;

		        /////////////////Estabilizar///////////////////////////////////
				heli->setAngles(0, 0, 0, 0, 1);
				while(contador < taoZ)
				{
					heli->renewImage(image);
					rawToMat(currentImage, image);
					keyControl = waitKey(5);
					imshow("Vuelo", currentImage);
					contador++;
				}
				contador = 0;
				///////////////////////////////////////////////////////////////

				heli->setAngles(0, 0, 0, 20000, 1);
		        while(contador < taoZ+50)
		        {
		                heli->renewImage(image);
		                rawToMat(currentImage, image);
		                keyControl = waitKey(5);
		                imshow("Vuelo", currentImage);
		                contador++;
		        }
		        heli -> land();

				//////////////////////////////////////
			}
			waitKey();
		}
		///////////////////////////////////////////////////////////////////////////////////////////////
        if (keyControl == 'p')       
        {       	    
			Mat map  = imread( "../src/main/mapa.png", 1 );  
			Mat fake = imread( "../src/main/mapa.png", 1 );  
			imshow("Ruta", map);
			waitKey(50);
			int radio = 50;
			int obs=1;
			cout<<"Radio del Paparrot: "<<endl;
			cin>>radio;

			cout<<"Numero de obstaculos "<<endl;
			cin>>obs;

			while(obs>0)
			{
				namedWindow("Ruta", 1);		 
				setMouseCallback("Ruta", CallBackFunc, NULL);							//Get the coordinates in globalXY
				imshow("Ruta", map);
				moveWindow("Ruta", 700, 0);												//Show original map

				cout<<"Seleccione el OBSTACULO"<<endl;						//Get the coordinates from the origen
				lclick=0;
				while(lclick==0)
					waitKey(5);	 
				circle(map, Point(globalXY[0],globalXY[1]) , radio, Scalar(13, 255, 13),-1,  8,  0);  
				circle(map, Point(globalXY[0],globalXY[1]) , radio, Scalar(13, 255, 13),-1,  8,  0);  
	 
	 			circle(fake, Point(globalXY[0],globalXY[1]) , radio, Scalar(13, 255, 13),-1,  8,  0);  
				circle(fake, Point(globalXY[0],globalXY[1]) , radio, Scalar(13, 255, 13),-1,  8,  0);  

				obs--;
			}
  
 
			namedWindow("Ruta", 1);	   
			setMouseCallback("Ruta", CallBackFunc, NULL);	  	 					//Get the coordinates in globalXY
			imshow("Ruta", fake);
			moveWindow("Ruta", 700, 0); 	 				 						//Show original map
			
			cout<<"Seleccione el ORIGEN"<<endl;	 		 	 		//Get the coordinates from the origen
			lclick=0;    
			while(lclick==0)              
				waitKey(5);	         
     
			int originx = globalXY[0];   
			int originy = globalXY[1]; 
			rainbox(map, fake, originx, originy);   	
 			imshow("Ruta", fake);  
 			moveWindow("Ruta", 700, 0);
  
			namedWindow("Ruta", 1);		 
			setMouseCallback("Ruta", CallBackFunc, NULL);							//Get the coordinates in globalXY
			imshow("Ruta", fake);
			moveWindow("Ruta", 700, 0);												//Show original map

			cout<<"Seleccione el DESTINO"<<endl;						//Get the coordinates from the origen
			lclick=0;
			while(lclick==0)
				waitKey(5);	 

			int destinationx = globalXY[0];
			int destinationy = globalXY[1];

			trayectorys(map, fake, originx, originy, destinationx, destinationy);	//Draw the possible trayectories p
			imshow("Ruta", fake);
			moveWindow("Ruta", 700, 0);
			

			Mat left=fake.clone();
			airtrack2(map, left, originx, originy, destinationx, destinationy, "IZQUIERDA");
			imshow("Ruta IZQUIERDA", left); 
			
			
			Mat right=fake.clone();
			airtrack2(map, right, originx, originy, destinationx, destinationy, "DERECHA");
			imshow("Ruta DERECHA", right);
			
			moveWindow("Ruta IZQUIERDA", 0, 0);
			moveWindow("Ruta", 700, 0);
			moveWindow("Ruta DERECHA", 1200, 0);

        }        
		//////////////////////////////////////////////////////////////////////////////////////////////
        keyControl = waitKey(5);
        //usleep(15000);
    }   

    delete image; 
    return 0;
} 



///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////RawToMat///////////////////////////////////////////////
void rawToMat( Mat &destImage, CRawImage* sourceImage)
{   
    uchar *pointerImage = destImage.ptr(0);
    
    for (int i = 0; i < 240*320; i++)
    {
        pointerImage[3*i] = sourceImage->data[3*i+2];
        pointerImage[3*i+1] = sourceImage->data[3*i+1];
        pointerImage[3*i+2] = sourceImage->data[3*i];
    }
}
 
///////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////ClickEvent/////////////////////////////////////////////
void mouseCoordinatesExampleCallback(int event, int x, int y, int flags, void* param)
{

    switch (event)
    {
        case CV_EVENT_LBUTTONDOWN:
            /*  Draw a point */
            points.push_back(Point(x, y));
            globalXY[0] = x;
            globalXY[1] = y;
            //cout<<globalXY[0]<<","<<globalXY[1]<<endl;

            status = true; 

            RGBsample(globalMat, globalXY, valRGB, status);

            pointRGBtoYIQ(valRGB,valYIQ);
            break;
        case CV_EVENT_MOUSEMOVE:
            break;
        case CV_EVENT_LBUTTONUP:
            break;
    }  
}
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
void sleepyt(int tiempos)
{
	int ciclo = 0;
	time_t original = time(0);
	time_t current = time(0);

	while((current-original)<tiempos)
	{
		current = time(0);
	}
	return;
}
void sleepym(int tiempos)
{
	int ciclo = 0;
	time_t original = time(0);
	time_t current = time(0);

	while((current-original)<tiempos)
	{
		current = time(0);
	}
	return;
}   

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	globalXY[0]=x; 
	globalXY[1]=y; 

     if  ( event == EVENT_LBUTTONDOWN )
     {
        //cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     	lclick=1;	
     }
     
/*     else if  ( event == EVENT_RBUTTONDOWN )
     {
          cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
     else if  ( event == EVENT_MBUTTONDOWN )
     {
          cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
     }
     else if ( event == EVENT_MOUSEMOVE )
     {
          cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;

     }*/
}




      