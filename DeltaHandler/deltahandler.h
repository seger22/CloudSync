#ifndef DELTAHANDLER_H
#define DELTAHANDLER_H

#include <iostream>
#include "zd_mem.h"
#include "zdlib.h"

using namespace std;

class DeltaHandler
{
public:
    DeltaHandler();
    Bytef* getDeltaBlock(Bytef sourceBlock[], int block_size);
    //Bytef* getSourceBlock(Bytef deltaBlock[],int deltablock_size);
    bool getSourceBlock(Bytef** sourceBlock, unsigned long int* sourcesize, Bytef *deltaBlock, int deltablock_size);
};

#endif // DELTAHANDLER_H
