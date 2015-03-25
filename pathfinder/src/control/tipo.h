#ifndef TIPO_H
#include <stdio.h>
#include <iostream>
#define TIPO_H

class tipo 
{
  public:
  tipo()
  {
  	mediaPhi1 = 0;
  	mediaPhi2 = 0;
  	maxPhi1	  = 0;
    maxPhi2   = 0;
  }

  ~tipo(){}

  void setmediaPhi1(float num){ mediaPhi1 = num;}
  void setmediaPhi2(float num){ mediaPhi2 = num;}
  void setmaxPhi1(float num){ maxPhi1 = num;}
  void setmaxPhi2(float num){ maxPhi2 = num;}

  float getmediaPhi1(){ return mediaPhi1;}
  float getmediaPhi2(){ return mediaPhi2;}
  float getmaxPhi1(){ return maxPhi1;}
  float getmaxPhi2(){ return maxPhi2;}


  protected:
  private:
  float mediaPhi1;
  float mediaPhi2;
  float maxPhi1;
  float maxPhi2;
};


#endif // TIPO_H