#ifndef CHAIN_H
#include <stdio.h>
#include <iostream>
#define CHAIN_H

class chain 
{
  public:
  chain()
  {
  	x = 0;
  	y = 0;
  	next = NULL;
  }

  float x;
  float y;
  chain *next;
};



#endif // CHAIN_H
