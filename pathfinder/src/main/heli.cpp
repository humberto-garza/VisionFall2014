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
float valYIQ[3] = {0};

//RGB Values = R, G, B  
int valRGB[3]   = {0}; 
 
//Global Matrix (current Image)
Mat globalMat; 

//Left Click
int lclick=0;
 
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
    float figure_phi1[4]={0};
    float figure_phi2[4]={0};
    float figure_width[4]={0};
    float figure_height[4]={0};
    int timer = 0;
 

    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
    useJoystick = SDL_NumJoysticks() > 0;
    if (useJoystick)
    {
         SDL_JoystickClose(m_joystick);
         m_joystick = SDL_JoystickOpen(0);
    }

	float localYIQ[3] = {0};
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
		cout<<"Presione -t- para iniciar el entrenamiento  TERRESTRE"<<endl; 
		cout<<"Presione -r- para iniciar el reconocimiento TERRESTRE"<<endl;
        cout<<"Presione -V- para iniciar el reconocimiento AEREO"<<endl;
        cout<<"Presione -m- para iniciar el mapeo"<<endl; 
  		 
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
       
        if (keyControl == 'm' && training_complete==0)       
        {       	    
			Mat map  = imread( "../src/main/mapaobs.png", 1 );  
			Mat fake = imread( "../src/main/mapaobs.png", 1 );    
			namedWindow("Ruta", 1);	   
			setMouseCallback("Ruta", CallBackFunc, NULL);	  	 					//Get the coordinates in globalXY
			imshow("Ruta", fake);
			moveWindow("Ruta", 700, 0); 	 				 						//Show original map
			
			cout<<"Seleccione el origen con un click"<<endl;	 		 	 		//Get the coordinates from the origen
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

			cout<<"Seleccione el origen con un click"<<endl;						//Get the coordinates from the origen
			lclick=0;
			while(lclick==0)
				waitKey(5);	

			int destinationx = globalXY[0];
			int destinationy = globalXY[1];

			trayectorys(map, fake, originx, originy, destinationx, destinationy);	//Draw the possible trayectories p
			imshow("Ruta", fake);
			moveWindow("Ruta", 700, 0);
			

			Mat left=fake.clone();
			airtrack(map, left, originx, originy, destinationx, destinationy, "IZQUIERDA");
			imshow("Ruta IZQUIERDA", left); 
			
			
			Mat right=fake.clone();
			airtrack(map, right, originx, originy, destinationx, destinationy, "DERECHA");
			imshow("Ruta DERECHA", right);
			
			moveWindow("Ruta IZQUIERDA", 0, 0);
			moveWindow("Ruta", 700, 0);
			moveWindow("Ruta DERECHA", 1200, 0);
        }        
         
         
         
        //Freeze Image and show objects using 'o' key
        if (keyControl == 't' && training_complete==0)
        {
        	//Clear Screen
        	printf("\033[2J\033[1;1H");

        	//Start the training with a number of samples
 			cout<<"///////////////////////////////////////////////////////////////////////////////////////////"<<endl;
 			cout<<"INICIA ETAPA DE ENTRENAMIENTO!"<<endl;
 			
 			//Take the sample of the colour
 			cout<<"Indique con un CLICK la muestra de color"<<endl;
 			cout<<"Presione cualquier tecla para continuar..."<<endl;
 			cout << "Y:" << valYIQ[0] << " I: " << valYIQ[1] << " Q: " << valYIQ[2] << endl;
 			waitKey();

 			//Assign the values to the local YIQ to be used   
 			localYIQ[0]=valYIQ[0];       
 			localYIQ[1]=valYIQ[1];      
 			localYIQ[2]=valYIQ[2];            
                                
 			//How many figures        
 			cout<<"///////////////////////////////////////////////////////// /////////////////////////////////"<<endl;
  			int kinds=1;  
 			cout<<"Cuántas FIGURAS desea enseñar?             (Maximo 4)"<<endl;
 			cin>>kinds;
 
 			//How many samples
 			int samples=4;   
 			cout<<"Cuantas MUESTRAS por figura desea? (Se recomiendan 10)"<<endl; 
 			cin>>samples; 
 
 			//Training cycle 
 			for(int j=0; j<kinds; j++) 
 			{
 				cout<<"///////////////////////////////////////////////////////////////////////////////////////////"<<endl;
  				cout<<"Entrene la FIGURA numero: "<<j+1<<endl;

				float mediaPhi1	= 0;
				float mediaPhi2	= 0;

				float minPhi1	= 1000;
				float maxPhi1	= 0;

				float minPhi2	= 1000; 
				float maxPhi2	= 0;

				for(int i=0; i<samples; i++)   
	 			{
		 			//Set the chains 
		            chain *firstC = NULL; 
		            chain *lastC  = NULL; 


	 				cout<<"Presione la tecla -n- despues de colocar la muestra: "<<i+1<<endl;
	 				keyControl = 0;
	 				while(keyControl!='n' && keyControl!= 27)
	 				{
						// Copy to OpenCV Mat
						heli->renewImage(image);
						rawToMat(currentImage, image);
						rawToMat(globalMat, image);
						//Show Current Image
						imshow("Current", currentImage);
						moveWindow("Current", 5, 5);
	 					keyControl = waitKey(5);
	 				}		 		 
 					 
				    //Apply YIQ Filter 
				    RGBtoYIQ(globalMat,filtYIQ, localYIQ);
				    //Binarize YIQ-filtered Image  
				    binariza(filtYIQ, binaryImage, 42); 
				    //Erode Binarized Image
				    threshold(binaryImage, eroImage, 127, 255, 0);
				    erode(eroImage, eroImage, Mat(), Point(-1, -1), 1, 1, 1);
				    imshow("Erode", eroImage);
  
				    //Find the objects
				    figure node = discreteSpiral(eroImage, 5); 

				    //Update the respective values
	            	mediaPhi1 += node.getPhi1(); 
	            	mediaPhi2 += node.getPhi2();

	            	if(minPhi1 > node.getPhi1())
	            		minPhi1 = node.getPhi1();

	            	if(maxPhi1 < node.getPhi1())
	            		maxPhi1 = node.getPhi1();

	            	if(minPhi2 > node.getPhi2())
	            		minPhi2 = node.getPhi2();
	            	if(maxPhi2 < node.getPhi2()) 
	            		maxPhi2 = node.getPhi2();

  
	            	//Show Current Segmented Image   
	                putCross(eroImage, node.getCenterX(),node.getCenterY(), node.getAngle(), 10);               
	            	imshow("Muestra", eroImage);
	            	moveWindow("Muestra", 400, 5);


	            	//Show Phis plotted traingraph
	            	putPoint(traingraph, node.getPhi1(), node.getPhi2());		
	            	imshow("Training Graph", traingraph);
	            	moveWindow("Training Graph", 5,350);
	 			} 

	 			mediaPhi1=mediaPhi1/samples;
	 			mediaPhi2=mediaPhi2/samples;

	 			float disti, dista; 
	 			float elipsew, elipseh;

	 			disti = mediaPhi1 - minPhi1;
	 			dista = maxPhi1   - mediaPhi1; 
	 			//cout<<"disti"<<disti<<endl;
	 			//cout<<"dista"<<dista<<endl;
	 			elipsew = disti;
	 			if(dista>disti)
	 				elipsew=dista;	
 
	 			disti = mediaPhi2 - minPhi2;
	 			dista = maxPhi2   - mediaPhi2;
	 			//cout<<"disti"<<disti<<endl;
	 			//cout<<"dista"<<dista<<endl;
	 			elipseh = disti;
	 			if(dista>disti) 
	 				elipseh=dista;	
 
		    	figure_phi1[j]	 = mediaPhi1;
		    	figure_phi2[j]	 = mediaPhi2;
		    	figure_width[j]	 = elipsew+0.03; 
		    	figure_height[j] = elipseh+0.03;

		    	//cout<<"mediaPhi1"<<mediaPhi1<<endl;
		    	//cout<<"mediaPhi2"<<mediaPhi2<<endl;
		    	//cout<<"elipsew"<<elipsew<<endl;
		    	//cout<<"elipseh"<<elipseh<<endl;

		    	putEllipse(traingraph, figure_phi1[j], figure_phi2[j], figure_width[j], figure_height[j]);
 				imshow("Training Graph", traingraph);
 				moveWindow("Training Graph", 5,350);
 			}

 			//Wait key to exit
 			training_complete = 1;

			cout<<"///////////////////////////////////////////////////////////////////////////////////////////"<<endl;  
            cout<<"FIN del entrenamiento, presione cualquier tecla para continuar"<<endl;
			cout<<"///////////////////////////////////////////////////////////////////////////////////////////"<<endl;
           
            waitKey();
            printf("\033[2J\033[1;1H");
        }

        if (keyControl == 'r'  )//&& training_complete==1
        {
        	//Clear Screen
        	printf("\033[2J\033[1;1H");
			//Start the training with a number of samples
 			cout<<"///////////////////////////////////////////////////////////////////////////////////////////"<<endl;
 			cout<<"INICIA ETAPA DE RECONOCIMIENTO TERRESTRE!"<<endl;
        	cout<<"Presione cualquier tecla para iniciar el proceso y ESC para terminar"<<endl;	
        	cout<<endl;
        	int points = 0;

	 		keyControl = 0;
			while(keyControl!='n' && keyControl!= 27)
			{
				//Clear Screen
				printf("\033[2J\033[1;1H");
				//Start the training with a number of samples
				cout<<"///////////////////////////////////////////////////////////////////////////////////////////"<<endl;
				cout<<"ETAPA DE RECONOCIMIENTO TERRESTRE!"<<endl;
				cout<<endl;

				// Copy to OpenCV Mat
				heli->renewImage(image);
				rawToMat(currentImage, image);
				rawToMat(globalMat, image);
				//Show Current Image
				imshow("Current", currentImage);
				moveWindow("Current", 5, 5);

			 	//Apply YIQ Filter 
			    RGBtoYIQ(globalMat,filtYIQ, localYIQ);
			    //Binarize YIQ-filtered Image  
			    binariza(filtYIQ, binaryImage, 42);  
			    //Erode Binarized Image
			    threshold( binaryImage, eroImage, 127, 255, 0);
			    erode(eroImage, eroImage, Mat(), Point(-1, -1), 1, 1, 1);



				chain *heado = NULL;
				chain *tailo = NULL;
				int total = discreteSpiral(eroImage, 5, heado, tailo);

            	imshow("Muestra", eroImage);
            	moveWindow("Muestra", 400, 5);
   
				for(int c=0; c< total; c++) 
				{
					if(heado!=NULL)  
					{
						float phi1 = heado->x;
						float phi2 = heado->y;  

						putEllipse(recongrapg, figure_phi1[0], figure_phi2[0], figure_width[0], figure_height[0]);
						putEllipse(recongrapg, figure_phi1[1], figure_phi2[1], figure_width[1], figure_height[1]);
						putEllipse(recongrapg, figure_phi1[2], figure_phi2[2], figure_width[2], figure_height[2]);
						putEllipse(recongrapg, figure_phi1[3], figure_phi2[3], figure_width[3], figure_height[3]);
						if(points++ > 50)
						{
							points =0; 
							recongrapg = Scalar(0,0,0);
							createGraph(recongrapg);
							putEllipse(recongrapg, figure_phi1[0], figure_phi2[0], figure_width[0], figure_height[0]);
							putEllipse(recongrapg, figure_phi1[1], figure_phi2[1], figure_width[1], figure_height[1]);
							putEllipse(recongrapg, figure_phi1[2], figure_phi2[2], figure_width[2], figure_height[2]);
							putEllipse(recongrapg, figure_phi1[3], figure_phi2[3], figure_width[3], figure_height[3]);
						} 

						putPoint(recongrapg,phi1,phi2);
						imshow("Reconocimiento", recongrapg);
						moveWindow("Reconocimiento", 5,350);
						moveWindow("Training Graph", 750,350);

						//cout<<c<<endl;
				        if((phi1 < figure_phi1[0] + figure_width[0])&& (phi1 > figure_phi1[0] - figure_width[0]))
				        {
							if((phi2 < figure_phi2[0] + figure_height[0])&& (phi2 > figure_phi2[0] - figure_height[0]))
							{
								cout << "CORAZON!!!!!!!" << endl;
							}
				        }
				        if((phi1 < figure_phi1[1] + figure_width[1])&& (phi1 > figure_phi1[1] - figure_width[1]))
				        {
							if((phi2 < figure_phi2[1] + figure_height[1])&& (phi2 > figure_phi2[1] - figure_height[1]))
							{
								cout << "NO PASE!!!!!!!" << endl;
							}
				        }
				        if((phi1 < figure_phi1[2] + figure_width[2])&& (phi1 > figure_phi1[2] - figure_width[2]))
				        {
							if((phi2 < figure_phi2[2] + figure_height[2])&& (phi2 > figure_phi2[2] - figure_height[2]))
							{
								cout << "VARILLA!!!!!!!" << endl;
							}
				        }
				        if((phi1 < figure_phi1[3] + figure_width[3])&& (phi1 > figure_phi1[3] - figure_width[3]))
				        {
							if((phi2 < figure_phi2[3] + figure_height[3])&& (phi2 > figure_phi2[3] - figure_height[3]))
							{
								cout << "SHREK!!!!!!!" << endl;
							}
				        }				        

						chain *aux=heado;
						heado=heado->next;
						delete aux;
					} 

				}
				cout<<"///////////////////////////////////////////////////////////////////////////////////////////"<<endl;  
	            cout<<"FIN del reconocimiento, presione ESC para terminar"<<endl;
				cout<<"///////////////////////////////////////////////////////////////////////////////////////////"<<endl;
				keyControl = waitKey(100);
			}	
        }
 
        if (keyControl == 'v')
        {
		   	//Clear Screen
        	printf("\033[2J\033[1;1H");
			//Start the training with a number of samples
 			cout<<"///////////////////////////////////////////////////////////////////////////////////////////"<<endl;
 			cout<<"INICIA ETAPA DE VUELO!"<<endl;
 			fprintf(stdout, "Battery : %.0lf \n", helidata.battery);
 			int cuantos=1;
 			cout<<"Cuantas instrucciones?"<<endl;
 			cin>>cuantos;
        	cout<<"Introduzca el password de vuelo (d)"<<endl;	
        	fprintf(stdout, "Battery : %.0lf \n", helidata.battery);
        	char key = waitKey();
 
	        if(key == 'd')
	        { 

	            heli->takeoff();
	            heli->setAngles(0, 0, 0, 0, 1);
				hover = 1;
	        	pitch = roll = yaw = height = 0.0;


				bool stop = TRUE;
				while (stop)
				{  

					int flightdistance = 100;

					heli->renewImage(image);
					rawToMat(currentImage, image);
					//Show Flight Image
					imshow("Current", currentImage);
					moveWindow("Current", 1750, 5);
					timer = 0;

					char key = 'p';
			        key = waitKey(5);
					switch (key) {
						case 'a': yaw = -20000.0; hover=1; break; 	//Rota clockwise
						case 'd': yaw = 20000.0; break;	 			//Rota counterclockwise

						case 'w': height = -20000.0; break;			//Sube
						case 's': height = 20000.0; break;			//Baja

						//case 'q': heli->takeoff(); break;
						//case 'e': heli->land(); break;

						case 'z': heli->switchCamera(0); break;
						case 'x': heli->switchCamera(1); break;
						case 'c': heli->switchCamera(2); break;
						case 'v': heli->switchCamera(3); break;

						case 'j': roll = -20000.0; pitch=0; hover=0; break;			//Izquierda
						case 'l': roll =  20000.0; pitch=0; hover=0; break;			//Derecha

						case 'i': pitch = -20000.0; roll=0; hover=0; break;			//Adelante
						case 'k': pitch =  20000.0; roll=0; hover=0; break;			//Atras

			            case 'h': hover = (hover + 1) % 2; break;
			            case 'n': hover = 0; break;
 
			            case 27: stop = FALSE; break;
			            //default:  yaw = height = 0.0;
			            default: pitch = roll = yaw = height = 0.0;			            
					}
					cout<<"Roll: "<<roll<<endl;
					cout<<"Pitch: "<<pitch<<endl; 
					heli->setAngles(pitch, roll, yaw, height, hover);
		        }
 
				heli->land();
	            ////////////////////////////////////////////////////////////////////////////////////////
 
	        }


 			cout<<"///////////////////////////////////////////////////////////////////////////////////////////"<<endl;  
            cout<<"FIN DEL VUELO"<<endl;
			cout<<"///////////////////////////////////////////////////////////////////////////////////////////"<<endl;
			waitKey();
        }




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




      