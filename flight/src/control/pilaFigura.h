#ifndef PILAFIGURA_H
#define PILAFIGURA_H
#include "figura.h"

class pilaFigura
{
    public:

        //Constructors
        pilaFigura()
        {
            figura fig;
            dataSet = new figura[40];
            limit = -1;
            max = 30;
        }
        pilaFigura(int n)
        {
            dataSet = new figura[n];
            limit = -1;
            max = n;
        }
        pilaFigura(const pilaFigura &p)
        {
            max = p.max;
            dataSet = new figura[max];
            limit = p.limit;
            for (int k = 0; k <= limit; k++)
                dataSet[k] = p.dataSet[k];
        }
        
        ~pilaFigura() {delete [] dataSet;}

        pilaFigura operator = (const pilaFigura &p)
        {
            if(this != &p)
            {
                if (p.dataSet != NULL)
                {
                    delete [] dataSet;
                    max = p.max;
                    dataSet = new figura [max];
                    limit = p.limit;
                    for (int k = 0; k <= limit; k++)
                        dataSet[k] = p.dataSet[k];
                }
            }
        }

        figura peek(int n) {return dataSet[n];}        

        void insert(figura fig)
        {
            if(limit > 30){
                limit = 0;
                dataSet[limit] = fig;
                limit++;
            }
            else{
            limit++;
            dataSet[limit] = fig;    
            }
            
        }
        void remove()
        {
            limit--;
        }

        void erase()
        {
            limit = -1;
        }
        
        int getLimit()
        {
            return limit;
        }
	
    protected:
    private:
    figura* dataSet; //dynamic array
    int limit; //last element location
    int max; //capacity
    figura fig;
};

#endif // PILAFIGURA_H