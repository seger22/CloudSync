#ifndef CHUNKDAT_H
#define CHUNKDAT_H

class ChunkDat
{
public:
    enum{max_length=66000};
    char data[max_length];
    int chunk_size;
    ChunkDat();

private:
};

#endif // CHUNKDAT_H
