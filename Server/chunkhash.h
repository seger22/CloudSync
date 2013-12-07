#ifndef CHUNKHASH_H
#define CHUNKHASH_H
#include<iostream>

class chunkhash
{
public:
    enum{max_length=8};
    unsigned char data[max_length]={0};
    chunkhash(){

    }
};
#endif // CHUNKHASH_H
