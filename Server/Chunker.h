#ifndef CHUNKER_H
#define CHUNKER_H

#include<boost/asio.hpp>


using namespace std;

class Chunk
{
public:
    Chunk();

private:
    u_int64_t hash;
    unsigned int off_set;
    unsigned int length;
    char path[255];

public:
Chunk(u_int64_t h, int o, int l,const char *p) {
    hash =  h;
    off_set = o;
    length =l;
    strcpy(path,p);

    };

u_int64_t getHash(){
return hash;
}

unsigned int getOffset(){
return off_set;
}

unsigned int getLength(){
return length;
}
char* getPath(){
return path;
}


void setHash(u_int64_t h){
    hash=h;
}
void setOffset(unsigned int o){
    off_set=o;
}
void setLength(unsigned int l){
    length=l;
}
void setPath(const char *p){
    strcpy(path,p);
}

};

#endif // CHUNKER_H
