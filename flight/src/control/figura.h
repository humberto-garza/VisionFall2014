#ifndef FIGURA_H
#include <stdio.h>
#include <iostream>

#define FIGURA_H


class figure
{
	public:		
		figure(){
			id = -1;
			area = 0;
			angle = 0;
			centerX = 0; //xtestada
			centerY = 0; //ytestada
			colorx = 0; //seed x
			colory = 0; //seed y
			phi1 = 0;
			phi2 = 0;
		}
		
		~figure(){}

		void setId(int num){ id = num;}
		void setArea(int num){ area = num;}
		void setAngle(float num){ angle = num;}
		void setCenterX(float num){ centerX = num;}
		void setCenterY(float num){ centerY = num;}
		void setPhi1(float num){ phi1 = num;}
		void setPhi2(float num){ phi2 = num;}
		void setColorX(int num){  colorx=num;}
		void setColorY(int num){  colory=num;}

		int getId(){ return id;}
		int getArea(){return area;}
		int getColorX(){ return colorx;}
		int getColorY(){ return colory;}

		float getCenterX(){ return centerX;}
		float getCenterY(){ return centerY;}
		float getAngle(){ return angle;}
		float getPhi1(){ return phi1;}
		float getPhi2(){ return phi2;}

		/*void prueba()
		{
			std::cout << "Funciona \n";
		}*/

	protected:
	private:
		int id;
		int area;
		float centerX;
		float centerY;
		float angle;
		float phi1;
		float phi2;
		int colorx;
		int colory;
};

#endif // FIGURA_H