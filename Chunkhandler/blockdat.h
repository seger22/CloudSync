#ifndef BLOCKDAT_H
#define BLOCKDAT_H

class BlockDat
{
public:
    enum{max_length=1000};
    char data[max_length]={0};
    int chunk_size=0;
    BlockDat(){
    }


private:

};

#endif // BLOCKDAT_H
