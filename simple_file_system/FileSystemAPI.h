//File System APIs: 

int createSFS( char* filename, int nbytes); //– returns a fileSystemId 

int readData( int disk, int blockNum, void* block); //– returns the num. of bytes read and read them into a buffer pointed by block. 

int writeData(int disk, int blockNum, void* block); //– writes that data in the block to the disk block blockNum. 

int writeFile(int disk, char* filename, void* block); //: write the buffer pointed by block. The same data should be recoverable by the file name. 

int readFile(int disk, char* filename, void* block); //: returns the data stored by the given file name. 