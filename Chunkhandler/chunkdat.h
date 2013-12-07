#ifndef CHUNKDAT_H
#define CHUNKDAT_H

class ChunkDat
{
public:
    enum{max_length=66000};
    char data[max_length]={0};
    int chunk_size=0;
    ChunkDat();

private:

};

#endif // CHUNKDAT_H
