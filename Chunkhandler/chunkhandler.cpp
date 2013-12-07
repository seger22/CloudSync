#include "chunkhandler.h"
#include <hashlibpp.h>

DatabaseManager dbmanager;

void debug(const char* msg, ...)
{
    if( DEBUG )
    {
        va_list argp;

        va_start(argp, msg);
        vfprintf(stderr, msg, argp);
        va_end(argp);
    }
}

BOOL fileExists(const char* fname)
{
    return access( fname, F_OK ) != -1;
}

//end utility functions


void printChunkData(
        const char* msgPrefix,
        int size,
        u_int64_t fingerprint,
        u_int64_t hash)
{
    fprintf(stderr, "%s chunk hash: %016llx fingerprint: %016llx length: %d\n",
            msgPrefix,
            hash,
            fingerprint,
            size);
}

void printChunkContents(
        u_int64_t hash,
        const unsigned char* buffer,
        int size)
{
    fprintf(stderr, "Chunk with hash %016llx contains:\n");
    for(int i = 0; i < size; ++i)
    {
        int tmp = buffer[i];
        fprintf(stderr, "%x", tmp);
    }
    fprintf(stderr, "\n");
}
u_int64_t makeBitMask(int maskSize)
{
    u_int64_t retval = 0;

    u_int64_t currBit = 1;
    for(int i = 0; i < maskSize; ++i)
    {
        retval |= currBit;
        currBit <<= 1;
    }

    return retval;
}


vector<Chunk> processChunks(DataSource* ds,const char *path)
{
    const u_int64_t POLY = FINGERPRINT_PT;
    window rw(POLY);
    rabinpoly rp(POLY);

    BitwiseChunkBoundaryChecker chunkBoundaryChecker;
    PrintChunkProcessor chunkProcessor;
    vector<Chunk> chunk;

    int next;
    u_int64_t val = 0;
    //add a leading 1 to avoid the issue with rabin codes & leading 0s
    u_int64_t hash = 1;
    rw.slide8(1);

    unsigned int offset=0;
    while((next = ds->getByte()) != -1)
    {
        chunkProcessor.processByte(next);

        hash = rp.append8(hash, (char)next);
        val = rw.slide8((char)next);

        if( chunkBoundaryChecker.isBoundary(val, chunkProcessor.getSize()) )
        {
            Chunk chnk(hash,offset,chunkProcessor.getSize(),path);
            chunk.push_back(chnk);
           // chunk.push_back(new Chunk(hash,offset,chunkProcessor.getSize(),path));
            offset+=chunkProcessor.getSize();
            chunkProcessor.completeChunk(hash, val);

            hash = 1;
            rw.reset();
            rw.slide8(1);
        }

    }
   // chunk.push_back(new Chunk(hash,offset,chunkProcessor.getSize(),path));
    Chunk chnk(hash,offset,chunkProcessor.getSize(),path);
    chunk.push_back(chnk);
    chunkProcessor.completeChunk(hash, val);

    return chunk;
}

vector<Chunk> chunk_file(const char *path)
{
    FILE* is = fopen(path, "r");
    DataSource* ds= new RawFileDataSource(is);
    vector<Chunk> chunk= processChunks(ds,path);
    fclose(is);
    delete ds;
    return chunk;
}

int getLastFileID(string fullpath){
    char path[PATH_MAX];
    strcpy(path, fullpath.c_str());
    int fileID=dbmanager.insertChecksum(fileChecksum(path).c_str(),path);
    return fileID;
}

void insert_file_chunk(const char* path){
    vector<Chunk> chunk = chunk_file(path);

    int fileID=getLastFileID(path);

    int done=-1;
    cout<<"Inserting chunks\n";
    for(int x=0;x<chunk.size();x++){

        done=  dbmanager.insertChunk(chunk[x].getHash(),chunk[x].getOffset(),chunk[x].getLength(),fileID);
        if(done!=-1){
            // cout<<"inserted" <<endl;
            done=-1;
        }
        else
            cout<<"couldn't insert. duplicate" <<endl;
    }
   // deleteChunks(chunk);

}

void deleteChunks(vector<Chunk*> chunks){
    typename vector<Chunk*>::iterator i;
    for (i = chunks.begin(); i < chunks.end(); ++i) {
        delete *i;
    }
}



void
chunk_directory(const char *dpath)
{
    DIR *dirp;
    if ((dirp = opendir (dpath)) == 0) return;
    struct dirent *de = NULL;
    if(!dbmanager.isOpen()){
        dbmanager.openDB();
        //  cout << "database opened" << endl;
        //   dbmanager.deleteChunkTableEntries();
    }
    while ((de = readdir (dirp))) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;
        char path[PATH_MAX];
        sprintf(path, "%s/%s", dpath, de->d_name);
        struct stat sb;
        lstat(path, &sb);
        if (S_ISDIR(sb.st_mode))
            chunk_directory(path);
        else if (S_ISREG(sb.st_mode) && !S_ISLNK(sb.st_mode)){
            //  fprintf(stderr, "\n\n------------------------------new file starts    %s -------------------------------------\n\n",path);
            insert_file_chunk(path);
        }
    }
    // dbmanager.closeDB();
    closedir(dirp);
}


void updateFileEntries(string filepath){
    if(!dbmanager.isOpen()){
        dbmanager.openDB();
      //  cout << "database opend" << endl;
    }
    dbmanager.deleteFile(QString::fromStdString(filepath));
    int fileID=getLastFileID(filepath);
    cout<<"Inserting chunks into database...\n" <<endl;
    vector<Chunk> chunks =chunk_file(filepath.c_str());
    for(int x=0;x<chunks.size();x++){
        int done=  dbmanager.insertChunk(chunks[x].getHash(),chunks[x].getOffset(),chunks[x].getLength(),fileID);
        if(done!=-1){
            // cout<<"inserted" <<endl;
            done=-1;
        }
        else
            cout<<"couldn't insert. duplicate" <<endl;
    }
    //deleteChunks(chunks);

}


void createFile(vector<u_int64_t> hashesFromClient, vector<ChunkDat> chunks,string filepath){

    int totalFileSize=0;
    int matchedChunkSize=0;
    int unmatchedChunkSize=0;
    DeltaHandler* dlthandler=new DeltaHandler();
  //  if(!dbmanager.isOpen()){
        dbmanager.openDB();
  //      cout << "database opend" << endl;}
    string path = "/home/mayuresan/Project/SyncDest" + filepath;
    FILE * output = fopen (path.c_str(), "w");
    bool matched =false;
    int indexofunmatched=0;
    Chunk chunk;
    for(int x=0;x<hashesFromClient.size();x++){
        matched = dbmanager.getChunk(hashesFromClient[x],chunk);
        //db::getchunkmatch()
        if(matched){
            // cout <<chunk.getHash() << "  is matched"<< endl;

            FILE* is = fopen(chunk.getPath(), "r");
            DataSource* ds= new RawFileDataSource(is);

            for(int x=0; x< (chunk.getOffset()+chunk.getLength());x++){
                if(chunk.getOffset()<=x){
                    char c = ds->getByte();
                    fputc ( c , output );
                }
                else
                    ds->getByte();
            }
            matchedChunkSize+=chunk.getLength();
            totalFileSize+=chunk.getLength();
            delete ds;
        }
        else{
            ChunkDat chnk = chunks[indexofunmatched];
            Bytef* sourceChunk=(Bytef*) malloc(1024*128*sizeof(Byte));
            unsigned long int chunkSize=0;
            //ChunkDat sourceChunk;
            Bytef* deltaChunk=(Bytef*) malloc(1024*128*sizeof(Byte));
            memcpy(reinterpret_cast<char *>(deltaChunk),chnk.data,chnk.chunk_size);

            cout<<"Delta decoding chunk:"<<indexofunmatched<<endl;
            dlthandler->getSourceBlock(&sourceChunk,&chunkSize,(Bytef *)deltaChunk,chnk.chunk_size);

            for(int y=0;y<chunkSize;y++){
                fputc(sourceChunk[y],output);
            }
            indexofunmatched++;
            unmatchedChunkSize+=chnk.chunk_size;
            totalFileSize+=chnk.chunk_size;
            free(sourceChunk);
            free(deltaChunk);
        }
    }
    fclose(output);
    delete dlthandler;
    cout << "Total File Size : " << totalFileSize << endl << endl;
    cout << "Matched Chunk Size : " << matchedChunkSize << endl << endl;
    cout << "Unmatched Chunk Size : " << unmatchedChunkSize << endl << endl;
}





//vector<int> findMatch(vector<u_int64_t> hashesFromClient){
vector<bool> findChunkMatch(vector<u_int64_t> hashesFromClient, vector<Chunk> *chunks, bool *allmatched, int& matchcount){
    matchcount=0;
    if(!dbmanager.isOpen()){
        dbmanager.openDB();
       // cout << "database opend" << endl;
    }
    vector<bool> unmatchdata;
    //vector<int> unmatched;
    bool matched =false;
    Chunk chunk;

    for(int x=0;x<hashesFromClient.size();x++){
        matched = dbmanager.getChunk(hashesFromClient[x],chunk);
        //db::getchunkmatch()
        if(matched){
            unmatchdata.push_back(true);
            matchcount++;
            chunks->push_back(chunk);
        }
        else
            //unmatched.push_back(x);
            unmatchdata.push_back(false);
    }
    cout<<"Total matched chunks: "<<matchcount<<endl;
    cout<<"Total unmatched chunks: "<<hashesFromClient.size()-matchcount<<endl;
    if(hashesFromClient.size()==matchcount) *allmatched=true;
    // dbmanager.closeDB();
    return unmatchdata;
    //return unmatched;
}

string fileChecksum(const char *path)
{
    hashwrapper *myWrapper = new sha1wrapper();
    string checksum;
    try
    {
        checksum   = myWrapper->getHashFromFile(path);
    }
    catch(hlException &e)
    {
        std::cerr << "Error " <<e.error_message()<< std::endl;
    }

    delete myWrapper;
    return checksum;
}

void DirectoryChecksum(const char *dpath)
{
    DIR *dirp;
    if ((dirp = opendir (dpath)) == 0) return;
    struct dirent *de = NULL;
 //   if(!dbmanager.isOpen())
   // {
        dbmanager.openDB();
   //     cout << "database opened" << endl;
   // }
    // dbmanager.deleteChecksumTableEntries();
    while ((de = readdir (dirp))) {
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
            continue;
        char path[PATH_MAX];
        sprintf(path, "%s/%s", dpath, de->d_name);
        struct stat sb;
        lstat(path, &sb);
        if (S_ISDIR(sb.st_mode))
            DirectoryChecksum(path);
        else if (S_ISREG(sb.st_mode) && !S_ISLNK(sb.st_mode)){
            string hash = fileChecksum(path);
            int done=  dbmanager.insertChecksum(hash.c_str(),path);
            // if(done!=-1)
            //  cout << "Checksum inserted" << endl;
        }
    }
    //  dbmanager.closeDB();
    closedir(dirp);
}

bool findFileChecksum(string checksum, string destinationpath)
{
    if(!dbmanager.isOpen()){
        dbmanager.openDB();
//        cout << "database opened" << endl;
    }
    string sourcepath;
    bool matched=dbmanager.findChecksum(checksum.c_str(),&sourcepath);
    if(matched){
        if(strcmp(sourcepath.c_str(),destinationpath.c_str())==0)
            return true;
        ifstream infile(sourcepath.c_str(),std::ios::binary);
        if(infile.good()){
            ofstream outfile(destinationpath.c_str(),std::ios::binary);
            outfile<<infile.rdbuf();
            infile.close();
            outfile.close();
            //to-do database update
        }
        else
            return false;
    }
    //  dbmanager.closeDB();
    return matched;
}



void writeChunks(vector<u_int64_t> hashesFromClient, queue<ChunkDat> *chunks,string filepath, string filename){
    string tmpfile=ConfigurationManager::getTempFileLocation();
    string temp=ConfigurationManager::getLocation()+filepath+filename;
    int totalFileSize=0;
    int matchedChunkSize=0;
    int unmatchedChunkSize=0;
    DeltaHandler dlthandler;
    cout<<"Decompressing and writing chunks...\n";
    if(!dbmanager.isOpen()){
        dbmanager.openDB();
      //  cout << "database opened" << endl;
    }

    ofstream outfile(tmpfile.c_str(),ios::binary);
    //  cout<<"File opened\n";
    bool matched =false;
    int indexofunmatched=0;
    Chunk chunk;
    Bytef deltaChunk[1024*128];
    for(int x=0;x<hashesFromClient.size();x++){
        matched = dbmanager.getChunk(hashesFromClient[x],chunk);
        if(matched){
            ifstream infile(chunk.getPath(),ios::binary);
            if(infile.good()){
                infile.seekg(chunk.getOffset(),infile.beg);
                char buffer[chunk.getLength()];
                infile.read(buffer,chunk.getLength());
                outfile.write(buffer,chunk.getLength());
                infile.close();
                matchedChunkSize+=chunk.getLength();
                totalFileSize+=chunk.getLength();

            }
        }
        else{
            if(chunks->empty()){
                while(chunks->empty()){}
            }
            ChunkDat chnk=chunks->front();
            chunks->pop();
            unsigned long int chunkSize=0;
            Bytef* sourceChunk;
            memcpy(reinterpret_cast<char *>(deltaChunk),chnk.data,chnk.chunk_size);
           // cout<<"Delta decoding chunk:"<<indexofunmatched<<endl;
            bool deltaresult=dlthandler.getSourceBlock(&sourceChunk,&chunkSize,(Bytef *)deltaChunk,chnk.chunk_size);
            if(deltaresult);

            else{
                cout<<"Error in delta decoding, so system reset\n";

                sourceChunk=NULL;


                break;
            }
            outfile.write(reinterpret_cast<char *>(sourceChunk),chunkSize);
            indexofunmatched++;
            unmatchedChunkSize+=chunkSize;
            totalFileSize+=chunkSize;

            sourceChunk=NULL;


        }
    }

    //int fileID=getLastFileID(tmpfile);
    outfile.close();

    ifstream tempfile(tmpfile.c_str(),ios::binary);
    ofstream newfile(temp.c_str(),ios::binary);
    newfile<<tempfile.rdbuf();
    tempfile.close();
    newfile.close();
    insert_file_chunk(temp.c_str());
    cout<<"Total file size: "<<totalFileSize<<" bytes"<<endl;
    cout<<"Total matched size: "<<matchedChunkSize<<" bytes"<<endl;
    cout<<"Total unmatched size: "<<unmatchedChunkSize<<" bytes"<<endl;
    //fclose(output);
}


vector<bool> findBlockMatch(vector <BlockChecksumSerial> block_checksums, string filepath,vector<int> *offsets){

    chain_hash_map<unsigned int, unsigned int> blockChecksumHashtable(block_checksums.size());
    chain_hash_map<unsigned int,unsigned int>::const_iterator hashiterator;
    vector<bool>  matchedindexes;
    int blocksize=ConfigurationManager::getBlockSize();
    int matchcount=0;
    for(int j=0;j<block_checksums.size();j++){
        blockChecksumHashtable.insert(block_checksums[j].getWeeksum(),j);
        matchedindexes.push_back(false);
        offsets->push_back(-1);
    }

    std::ifstream is(filepath.c_str());

    cout<<"Calculating rolling checksums\n";
    Rollsum rolling;
    RollsumInit(&rolling);
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);
    char c = is.get();
    std::queue<char> window;
    rs_strong_sum_t strongsum;
    char buffer[blocksize];
    int i=0,offsetCount=0, prevOffset=0;
    //while(is.good()){
    while(offsetCount<length){
        if(i<blocksize){
            RollsumRollin(&rolling,c);
            window.push(c);
            c = is.get();
            i++;
            continue;
        }
        else if(length>i){
            RollsumRollin(&rolling,c);
            window.push(c);
            RollsumRollout(&rolling,window.front());
            window.pop();
            //    cout<<i<<" Window: "<<window.size()<<" length: "<<length<<" and i: "<<i<<endl;
            offsetCount++;

        }
        else{
            RollsumRollout(&rolling,window.front());
            window.pop();
            offsetCount++;
            // cout<<i<<" Window: "<<window.size()<<endl;
        }

        hashiterator=blockChecksumHashtable.find(RollsumDigest(&rolling));

        if(hashiterator!=blockChecksumHashtable.end()){
            int key=hashiterator.key();

            prevOffset=is.tellg();
            is.seekg(offsetCount,is.beg);
            is.read(buffer,(length>i) ? blocksize : length-offsetCount);
            is.seekg(prevOffset,is.beg);

            if(length>i){
                rs_calc_strong_sum(&buffer,blocksize,&strongsum);

            }
            else
                rs_calc_strong_sum(&buffer,length-offsetCount,&strongsum);

            for ( hashiterator=blockChecksumHashtable.find(key);hashiterator != blockChecksumHashtable.end(); ++hashiterator){
                if(key!=hashiterator.key())
                    break;

                if(memcmp(strongsum,block_checksums[hashiterator.value()].strongsum,sizeof(strongsum))==0){

                    if(offsets->at(hashiterator.value())==-1){
                        offsets->at(hashiterator.value())=offsetCount;                        
                        matchedindexes[hashiterator.value()] = true;
                        matchcount++;
                    }
                }
            }

        }
        if(length>i){
            c = is.get();
        }
        i++;

    }
    is.close();
    cout<<"Total blocks matched: "<<matchcount<<endl;
    return matchedindexes;
}

void createFileWithBlocks(vector<bool> matchdata, vector<int> offsets, vector<ChunkDat> blocks, vector<int> all_block_offsets,string filepath,string filename){
    string oldfile=filepath+"/temp";
    int blocksize=ConfigurationManager::getBlockSize();
    ifstream infile((filepath+filename).c_str(),std::ifstream::binary);
    ofstream outfile(oldfile.c_str(),std::ofstream::binary);
    int offsetCount=0,inblockCount=0,file_size=0;
    for(int i=0;i<matchdata.size();i++){
        if(matchdata[i]){
            //cout<<"Writing matched data: "<<i<<endl;
            infile.seekg(offsets[offsetCount],infile.beg);
            char* buffer = new char[blocksize];
            infile.read(buffer,blocksize);
            outfile.write(buffer,blocksize);
            cout<<"Read : 700 bytes at: "<<offsets[offsetCount]<<" at "<<file_size<<endl;
            file_size+=blocksize;
            offsetCount++;
        }
        else{
            //cout<<"Writing unmatched data: "<<i<<endl;
            outfile.write(blocks[inblockCount].data,blocks[inblockCount].chunk_size);
            cout<<"wrote : "<<blocks[inblockCount].chunk_size<<" bytes at: "<<file_size<<endl;
            file_size+=blocks[inblockCount].chunk_size;
            inblockCount++;

        }
    }
    cout<<"Wrote totally: "<<file_size<<" bytes!!!!!!\n";
    remove((filepath+filename).c_str());

    rename(oldfile.c_str(),(filepath+filename).c_str());
    infile.close();
    outfile.close();
    cout<<"File modified successfully!!!\n";
}


void writeChunkFile(ChunkDat chunk, string filepath){
   ofstream outfile(filepath.c_str(),ios::binary);
   outfile.write(chunk.data,chunk.chunk_size);
   outfile.close();
}


void writeBlocks(vector<ChunkDat> blocks, vector<int> matchedOffsets, vector<int> allNewOffsets, string filepath, string filename){
    string tmpfile="/home/mayuresan/Project/RSync/TEMP/temp";
    string matchfilepath=filepath+filename;
    int block_size=ConfigurationManager::getBlockSize();
    cout<<"Match file path: "<<matchfilepath<<endl;
    cout<<"Opening temp file\n";
    ofstream outfile(tmpfile.c_str(),ios::binary);
    cout<<"Opening new file\n";
    ifstream infile(matchfilepath.c_str(),ios::binary);

    int blockCount=0;
    if(matchedOffsets.size()==0 || allNewOffsets.size()==0) return;
    for(int i=0;i<allNewOffsets.size();i++){
        if(matchedOffsets[i]!=-1){
            cout<<"Writting matched\n";
            char buffer[block_size];
            cout<<"Reading at: "<<matchedOffsets[i]<<endl;
            infile.seekg(matchedOffsets[i],infile.beg);
            infile.read(buffer,block_size);
            cout<<"Writing to offset: "<<allNewOffsets[i]<<endl;
            outfile.seekp(allNewOffsets[i]);
            outfile.write(buffer,block_size);
        }
        else{
            outfile.seekp(allNewOffsets[i]);
            outfile.write(blocks[blockCount].data,blocks[blockCount].chunk_size);
            blockCount++;
        }
        cout<<"Wrote block:"<<i<<endl;
    }
    outfile.close();
    infile.close();
    ifstream tempfile(tmpfile.c_str(),ios::binary);
    ofstream newfile(matchfilepath.c_str(),ios::binary);
    newfile<<tempfile.rdbuf();
    tempfile.close();
    newfile.close();
    //remove(tmpfile.c_str());
}


void writeData(vector<Chunk> chunks, vector<ChunkDat> blocks, vector<int> matchedBlockOffsets, vector<ChunkBlockOrderer> chunk_block_offsets,vector<int> lengths, string filename, string filepath){
    //  string tmpfile="/home/mayuresan/Project/RSync/TEMP/temp";
    cout<<"Writing blocks...\n";
    string tmpfile=ConfigurationManager::getTempFileLocation();
    int block_size=ConfigurationManager::getBlockSize();
    vector<BlockDat> indBlocks=getUncompressedBlocks(blocks,lengths,matchedBlockOffsets);
    ofstream outfile(tmpfile.c_str(),ios::binary);
    ifstream inblockfile((filepath+filename).c_str(),ios::binary);
    int chunk_order=0, block_order=0,block_data_order=0;   
    for(int i=0;i<chunk_block_offsets.size();i++){
        ChunkBlockOrderer chunkBlock=chunk_block_offsets[i];
        if(chunkBlock.isChunk()){
            Chunk chunk=chunks[chunk_order];
            ifstream infile(chunk.getPath(),ios::binary);
            infile.seekg(chunk.getOffset(),infile.beg);
            char buffer[chunk.getLength()];
            infile.read(buffer,chunk.getLength());
            outfile.write(buffer,chunk.getLength());
            infile.close();
            chunk_order++;
        }
        else{
            if(matchedBlockOffsets[block_order]==-1){
                BlockDat block=indBlocks[block_data_order];

                outfile.write(block.data,block.chunk_size);
                block_data_order++;

            }
            else{
                inblockfile.seekg(matchedBlockOffsets[block_order],ios::beg);
                char buffer[block_size];
                inblockfile.read(buffer,block_size);
                outfile.write(buffer,block_size);
            }
            block_order++;
        }
    }
    inblockfile.close();
    outfile.close();
    ifstream tempfile(tmpfile.c_str(),ios::binary);
    ofstream newfile((filepath+filename).c_str(),ios::binary);
    newfile<<tempfile.rdbuf();
    tempfile.close();
    newfile.close();
    updateFileEntries(filepath+filename);

}


vector<BlockDat> getUncompressedBlocks(vector<ChunkDat> mergedBlocks, vector<int> lengths, vector<int> matchedOffsets){
    vector<BlockDat> blocks;
    DeltaHandler deltahandler;
    int arrlength=0,k=0;
    for(int i=0;i<mergedBlocks.size();i++){
        arrlength=0;
        ChunkDat block=mergedBlocks[i];      
        Bytef* sourceblock;
        unsigned long int blockSize=0;       
        Bytef deltablock[1024*65];
        memcpy(reinterpret_cast<char *>(deltablock),block.data,block.chunk_size);
        //cout<<"Delta decoding merged block:"<<i+1<<endl;
        deltahandler.getSourceBlock(&sourceblock,&blockSize,(Bytef *)deltablock,block.chunk_size);
        memcpy(block.data,reinterpret_cast<char *>(sourceblock),blockSize);
        for(int j=k;j<lengths.size();j++){
            if(matchedOffsets[j]==-1){
                BlockDat resblock;
                if(arrlength+lengths[j]>= 65536){
                    k=j;
                    break;
                }                
                memcpy(resblock.data,&block.data[arrlength],lengths[j]);
                resblock.chunk_size=lengths[j];
              //  cout<<"Unmatched block: "<<j<<" vals: "<<(int)resblock.data[0]<<" : "<<(int)resblock.data[1]<<" : "<<(int)resblock.data[lengths[j]-2]<<" : "<<(int)resblock.data[lengths[j]-1]<<endl;
                blocks.push_back(resblock);
                arrlength+=lengths[j];
            }
        }
    }
    return blocks;
}
