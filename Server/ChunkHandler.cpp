
#include "ChunkHandler.h"
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


vector<Chunk*> processChunks(DataSource* ds,const char *path)
{
  const u_int64_t POLY = FINGERPRINT_PT;
  window rw(POLY);
  rabinpoly rp(POLY);

  BitwiseChunkBoundaryChecker chunkBoundaryChecker;
  PrintChunkProcessor chunkProcessor;
  vector<Chunk*> chunk;

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

chunk.push_back(new Chunk(hash,offset,chunkProcessor.getSize(),path));
offset+=chunkProcessor.getSize();
  chunkProcessor.completeChunk(hash, val);

        hash = 1;
        rw.reset();
        rw.slide8(1);
    }

  }
chunk.push_back(new Chunk(hash,offset,chunkProcessor.getSize(),path));
  chunkProcessor.completeChunk(hash, val);

return chunk;
}

vector<Chunk*> chunk_file(const char *path)
{
    FILE* is = fopen(path, "r");
    DataSource* ds= new RawFileDataSource(is);
    vector<Chunk*> chunk= processChunks(ds,path);
 return chunk;
}


void
chunk_directory(const char *dpath)
{
  DIR *dirp;
  if ((dirp = opendir (dpath)) == 0) return;
  struct dirent *de = NULL;
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
//fprintf(stderr, "\n\n------------------------------new file starts    %s -------------------------------------\n\n",path);
      vector<Chunk*> chunk = chunk_file(path);

      if(!dbmanager.isOpen()){
     dbmanager.openDB();
      cout << "database opened" << endl;}

      int done=-1;
      for(int x=0;x<chunk.size();x++){

          done=  dbmanager.insertChunk(chunk[x]->getHash(),chunk[x]->getOffset(),chunk[x]->getLength(),chunk[x]->getPath());
        if(done!=-1){
            cout<<"inserted" <<endl;
        done=-1;
        }
        else
            cout<<"couldn't insert. duplicate" <<endl;
      }
 dbmanager.closeDB();
}
  }
  closedir(dirp);
}

void createFile(vector<u_int64_t> hashesFromClient, vector<ChunkDat> chunks,string filepath){

    int totalFileSize=0;
    int matchedChunkSize=0;
    int unmatchedChunkSize=0;

    int totalChunks=hashesFromClient.size();
    int matchedChunks=0;
    int unmatchedChunks=0;

    if(!dbmanager.isOpen()){
   dbmanager.openDB();
    cout << "database opened" << endl;}
string path = "/home/satthy/Server" + filepath;
    FILE * output = fopen (path.c_str(), "w");

bool matched =false;
int indexofunmatched=0;
Chunk chunk;
        for(int x=0;x<hashesFromClient.size();x++){
            matched = dbmanager.getChunk(hashesFromClient[x],chunk);
            if(matched){
            //  cout <<chunk.getHash() << "  is matched"<< endl;

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

matchedChunks++;
fclose(is);
            }
            else{
             //   cout <<hashesFromClient[x] << "  is unmatched"<< endl;
                ChunkDat chnk = chunks[indexofunmatched];

                for(int y=0;y<chnk.chunk_size;y++){
                           fputc(chnk.data[y],output);
                       }
              indexofunmatched++;
              unmatchedChunkSize+=chnk.chunk_size;
              totalFileSize+=chnk.chunk_size;

              unmatchedChunks++;
        }

  }
        fclose(output);
        cout << endl;
         cout << "Number of Matched Chunks   ------- " << matchedChunks  << endl;
         cout << "Number of Unmatched Chunks ------- " << unmatchedChunks  << endl;
         cout << "Total File Chunks          ------- " << totalChunks << endl;
              cout << endl;

              cout << "Matched Chunk Size    ----------- " << matchedChunkSize << " Bytes" << endl;
         cout << "Unmatched Chunk Size  ----------- " << unmatchedChunkSize << " Bytes"<< endl;
         cout << "Total File Size       ----------- " << totalFileSize  << " Bytes"<< endl;
          cout << endl;
}

vector<int> findMatch(vector<u_int64_t> hashesFromClient){

    if(!dbmanager.isOpen()){
    dbmanager.openDB();
    cout << "database opened" << endl;}

    vector<int> unmatched;
    bool matched =false;
    Chunk chunk;
        for(int x=0;x<hashesFromClient.size();x++){
            matched = dbmanager.getChunk(hashesFromClient[x],chunk);
            if(matched){}
            else
                unmatched.push_back(x);
  }
    return unmatched;
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
          std::cerr << "Error " << std::endl;
    }

     delete myWrapper;
    return checksum;
}

void DirectoryChecksum(const char *dpath)
{
    DIR *dirp;
    if ((dirp = opendir (dpath)) == 0) return;
    struct dirent *de = NULL;
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

        if(!dbmanager.isOpen()){
       dbmanager.openDB();
        cout << "database opened" << endl;}

        int done=  dbmanager.insertChecksum(hash.c_str(),path);
        if(done!=-1)
            cout << "Checksum inserted" << endl;

        dbmanager.closeDB();
        }
  }
    closedir(dirp);
 }

bool findFileChecksum(string checksum, string &value)
{

    if(!dbmanager.isOpen()){
    dbmanager.openDB();
    cout << "database opened" << endl;}

    bool matched=dbmanager.findChecksum(checksum.c_str(),value);
    return matched;
}

void copyFile(string sourcepath,string destpath){

    std::ifstream  src(sourcepath.c_str());
        std::ofstream  dst(destpath.c_str());
        dst << src.rdbuf();
}
