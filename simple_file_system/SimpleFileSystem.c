#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int createSFS( char* filename, int nbytes); // returns a fileSystemId 

int readData( int disk, int blockNum, void* block); // returns the num. of bytes read and read them into a buffer pointed by block. 

int writeData(int disk, int blockNum, void* block); // writes that data in the block to the disk block blockNum. 

int writeFile(int disk, char* filename, void* block); // write the buffer pointed by block. The same data should be recoverable by the file name. 

int readFile(int disk, char* filename, void* block); // returns the data stored by the given file name. 

void print_inodeBitmaps(int fileSystemId); //prints the bitmaps of inodes 
void print_dataBitmaps(int fileSystemId); //prints the bitmaps of data block 
void print_FileList(int fileSystemId); //prints all the files in the file system 

int writeval(int disk,void* val,int size)
{
	return write(disk,val,size);
}

void print_inodeBitmaps(int fileSystemId)
{
	char* data2print = (char*) malloc(sizeof(char)*(4*1024));

	int raread,flag=0;
	int laseek = lseek(fileSystemId,4*1024,SEEK_SET);

	if(laseek<0)
	{
		printf("error in seek\n");
	} 

	else
	{
		raread = read(fileSystemId,(void*)data2print,4*1024);

		if(raread<0)
		{
			flag=1;
			printf("error in read\n");
		}

	}

	if(flag==0)
	{

		int i;

		for(i=0;i<4*1024;i++)
		{
			printf("%d",data2print[i] );
		}
	}

	printf("\n");

}

void print_dataBitmaps(int fileSystemId)
{
	char* data2print = (char*) malloc(sizeof(char)*(4*1024));

	int raread,flag=0;
	int laseek = lseek(fileSystemId,8*1024,SEEK_SET);

	if(laseek<0)
	{
		printf("error in seek\n");
	} 

	else
	{
		raread = read(fileSystemId,(void*)data2print,4*1024);

		if(raread<0)
		{
			flag=1;
			printf("error in read\n");
		}

	}

	if(flag==0)
	{

		int i;

		for(i=0;i<4*1024;i++)
		{
			printf("%d",data2print[i] );
		}
	}

	printf("\n");

}

int createSFS(char* filename, int nbytes)
{
	// creating/opening a file

	mode_t mode  = S_IRUSR | S_IWUSR;
	
	int sysID = open(filename, O_CREAT | O_RDWR, mode);

	if(sysID<0)
	{
		return -1;
	}

	else
	{
		
		// check for the space can be created for nbytes

		char buf[100] = "0";

		int lop= nbytes,error;

		int cc = 0;

		while(lop--)
		{
			cc++;
			error = writeval(sysID,buf,1);

			if(error<0)
			{
				return -2;
			}
		}

		printf("file size: %d\n",cc );

	}

	/*char * superBlock = (char*) malloc(sizeof(char)*(strlen("MySFS")+1));
	strcpy(superBlock,"MySFS");
	int checkData = writeData(sysID,0,superBlock);

	if(checkData<0)
	{
		printf("writeData gave error code: %d\n", checkData);
	}*/

	return sysID;
	
	
}

int writeData(int disk, int blockNum, void* block)
{
	int lcheck = lseek(disk,blockNum*4*1024,SEEK_SET),wcheck;

	if(lcheck<0)
	{
		return -1;
	}

	else
	{

		wcheck = writeval(disk,block,4*1024);
		
		if(wcheck<0)
		{
			return -2;
		}
	}

	return wcheck;
}

#define superOffset  0 * 1024
#define inodeBitmapOffset   4 * 1024
#define dataBitmapOffset  8 * 1024
#define inodeDataOffset  12 * 1024
#define dataOffset  ( 12 + 4 * 128 ) * 1024

int readFile(int disk, char* filename, void* block){
	/*Return values :
	-1 : Error in lseek()
	-2 : Error in read()
	-3 : File not found (in inode table) error
	-4 : Error reading from file
	 0 : Data read successfully
	*/
	int i,found;
	int fourKB=1024*4;
	int starting,file_size,n_blocks;
	
	char *name,*word;
	
	word=(char*)malloc(sizeof(char)*16);
	name=(char*)malloc(sizeof(char)*8); 
	
	found=0;
	
	for(i=12*1024;i<( 12 + 4 * 128 ) * 1024;i+=16)
	{
		if(lseek(disk,i,SEEK_SET)<0) return -1;
		if(read(disk,(void*)word,16)!=16) return -2; //4KB Data Block
		memcpy((void*)name,(void*)word,8); //Extract file name
		
		// printf("Read : %s\n",name);
		if(strcmp(name,filename) == 0)
		{
			found=1;
			memcpy((void*)(&starting),(void*)(word+8),2);
			memcpy((void*)(&n_blocks),(void*)(word+10),2);
			memcpy((void*)(&file_size),(void*)(word+12),4);
			printf("Starting block : %d\n",starting);
			printf("Number of blocks : %d\n",n_blocks);
			printf("Size of file : %dB\n",file_size);
			break;
		}
	}	
	
	if(!found) return -3;
	int starting_block=starting+(dataOffset/fourKB); //Block 'number'
	
	// printf("Starting block %d\n",starting_block);
	
	if(lseek(disk,starting_block,SEEK_SET)<0) return -1; //Seek to starting of file
	char* one_block;
	one_block=(char*)malloc(sizeof(char)*fourKB);
	
	//Reading 4KB at a time :
	
	for(i=0;i<n_blocks;i++)
	{
		if(readData(disk,starting_block+i,(void*)one_block) == -1) return -4;
		memcpy((void*)(block+i*fourKB),(void*)one_block,fourKB); //Adding data to block, 4KB at a time
	}
	
	// read(disk,block,file_size);
	return 0;
}

int readData(int disk,int blockNum, void* block)
{
	int lcheck = lseek(disk,blockNum*4*1024,SEEK_SET),rcheck;

	if(lcheck<0)
	{
		return -1;
	}

	else
	{
		rcheck = read(disk,block,4*1024);

		if(rcheck<0)
		{
			return -2;
		}
	}

	return rcheck;
}

void print_FileList(int fileSystemId)
{	
	char* file_name;
	int starting_block,number_of_blocks,file_size;
	
	file_name=(char*)malloc(8*sizeof(char*)); //8-byte word filename
	
	char* inode_map,*inode_data;
	int fourKB=4*1024;
	int bytes_read=fourKB*128; //128 blocks for inode
	
	inode_map=(char*)malloc(sizeof(char)*fourKB);
	inode_data=(char*)malloc(sizeof(char)*bytes_read);
	
	if(lseek(fileSystemId,inodeBitmapOffset,SEEK_SET)<0) return; //Error reaching offset
	if(read(fileSystemId,(void*)inode_map,fourKB) == -1) return; //Error reading from file
	
	int i,j,k;
	
	if(lseek(fileSystemId,inodeDataOffset,SEEK_SET)<0) return ; //Error reaching second offset
	if(read(fileSystemId,(void*)inode_data,bytes_read) == -1) return; //Error reading second time from file
	
	//Print data for non-zero inode entries
	char* entry;
	unsigned char read_byte;
	entry=(char*)malloc(sizeof(char)*16); //Size of an inode entry
	
	for(i=0;i<2;i++) 
	{	
		printf("I read %d\n",(unsigned char)inode_map[i]);
		
		for(j=0;j<8;j++)
		{
			read_byte=inode_map[i];
			k=(int)read_byte;
			k=(128>>j)&k;
			
			if(k)
			{
				//Reading (8i+j)th inode data entry
				memcpy((void*)entry,(void*)(inode_data+(8*i+j)*16),16); //16 bytes per inode entry
				memcpy((void*)file_name,(void*)(entry),8); //First 8 bytes : filename
				memcpy((void*)(&starting_block),(void*)(entry+8),2); //Next 2 bytes : starting_block
				memcpy((void*)(&number_of_blocks),(void*)(entry+10),2); //Next 2 bytes : number of blocks
				memcpy((void*)(&file_size),(void*)(entry+12),4); //Next 4 bytes : file size
				printf("File name : %s\n",file_name);
				printf("Starting block of file : %d\n",starting_block);
				printf("Number of blocks : %d\n",number_of_blocks);
				printf("File size : %d\n",file_size);
				printf("\n");
			}
		}		
	}	
}

int checkActual(void* block)
{
	int i;

	for(i=0;;i++)
	{
		if(*(char*)(block+i)==0)
		{
			break;
		}
	}

	return i;
}

void memcopy(char* tocopy, void* copval,int size)
{
	memcpy(tocopy,copval,size);
}

int writeFile(int disk, char* filename, void* block)
{
	// superBlock = 0*1024 bytes
	// inodeBitmap starts from 4*1024 block
	// dataBitmap starts from 8*1024 block
	// data = 16*128*1024
	// Errors: -1 = seek error, -2 = read error, -3 inode space , -4 data space, -5 final wirteData error 

	int actual_size,block_size,sixteenB; 
	int j,i,k,blockNum,fourKB,data_space,inode_space;
	fourKB=4*1024;
	sixteenB=16;

	//Needs to be checked :
	actual_size=0;
	while(*(char*)(actual_size+block)!=0) actual_size++; 
	block_size=actual_size/fourKB;
	if(actual_size%fourKB != 0) block_size++;

	// //Temporary :
	// block_size=1;
	// actual_size=fourKB;

	data_space=-1;
	char inode_space_block,data_space_block;
	inode_space=-1;
	char* dataBitmap;
	blockNum=dataBitmapOffset;
	dataBitmap=(char*)malloc(sizeof(char)*fourKB); //4KB data bitmap
	if(lseek(disk,blockNum,SEEK_SET)<0) return -1;
	if(read(disk,(void*)dataBitmap,fourKB) == -1){
		// printf("Data Bitmap%s\n", (char*)dataBitmap);
		return -2;
	}
	// printf("First checkpoint\n");
	//Find space in data_bitmap
	int maxContFound = 0;
	for(i = 0; i < fourKB; i++)
	{
		for(j = 0;j < 8; j++)
		{
			k = (unsigned char)dataBitmap[i];
			// printf("Data bitmap is %d\n",k);
			int isSet = (128 >> j) & k;
			// printf("IsSet is %d\n",isSet);
			if (isSet == 0) maxContFound++;
			else maxContFound = 0;
			if(maxContFound==block_size)
			{
				data_space = (8 * i + j) - ( maxContFound -1);
				data_space_block = dataBitmap[ data_space / 8];
				goto heaven;
			}
		}
	}
	heaven:
	if(data_space == -1) return -3; //No space for data
	//Find space for inode entry in inode_bitmap
	char* inodeBitmap;
	blockNum = inodeBitmapOffset;
	inodeBitmap = (char*)malloc(sizeof(char)*fourKB); //4KB inode bitmap
	if(lseek(disk,blockNum,SEEK_SET) < 0) return -1;
	if(read(disk,(void*)inodeBitmap,fourKB) == -1) return -2;
	// printf("Second checkpoint\n");
	for(i=0;i<fourKB;i++)
	{
		for(j=0;j<8;j++)
		{
			k=(unsigned char)inodeBitmap[i];
			// printf("I read %d\n",k);
			k=(128>>j)&k;
			if(k==0)
			{
				inode_space=(8 * i + j); 
				inode_space_block=inodeBitmap[inode_space / 8]; //Get 8 bytes
				goto hell; 
			}
		}
	}	
	unsigned char yoda;
	int jedi;
	hell:
	if(inode_space==-1) return -4; //No space for inode entry
	//TESTED:
		//Set inode bitmap (inode_space)  to one:
		yoda=inode_space_block;
		jedi=(int)yoda;
		// printf("Before : %d, want to %dth bit\n",jedi,(inode_space%8));
		jedi=((128)>>(inode_space%8)) | jedi; //Setting 'inode_space%8'th bit
		// printf("After : %d\n",jedi);
		// printf("Inode_space/8 is %d\n",inode_space/8);
		if(lseek(disk,inodeBitmapOffset+(inode_space/8),SEEK_SET)<0) return -1; 
		yoda=(char)jedi;
		if(write(disk,(void*)(&yoda),1)!=1) return -2; //Rewriting that whole byte (as it is tedious to rewrite individual bit)
		// printf("Third checkpoint\n");

	//Checking if inode bit was actually set
	// unsigned char into;
	// lseek(disk,inodeBitmapOffset+(inode_space/8),SEEK_SET);
	// read(disk,(void*)(&into),1);
	// printf("Verified : %d\n",into);

	//TESTED:
		//Set data bitmap (data_space) to one {for now..need to set all data maps for input data}:
		yoda=data_space_block;
		jedi=(int)yoda;
		// printf("Before : %d, want to %dth bit\n",jedi,(inode_space%8));
		jedi=((128)>>(data_space%8)) | jedi; //Setting 'data_space%8'th bit
		// printf("After : %d\n",jedi);
		if(lseek(disk,dataBitmapOffset+(data_space/8),SEEK_SET)<0) return -1;
		// printf("Data_space/8 is %d\n",data_space/8); 
		yoda=(char)jedi;
		if(write(disk,(void*)(&yoda),1)!=1) return -2; ////Rewriting that whole byte (as it is tedious to rewrite individual bit)
		// printf("Fourth checkpoint\n");

	// // Checking if inode bit was actually set
	// unsigned char into;
	// lseek(disk,inodeBitmapOffset+(inode_space/8),SEEK_SET);
	// read(disk,(void*)(&into),1);
	// printf("was there %d\n",into);

	//TESTED : 
		//Write metadata to inode table
		if(lseek(disk,inodeDataOffset+inode_space*sixteenB,SEEK_SET)<0) return -1;
		if(write(disk,(void*)filename,8)!=8) return -2; //Setting 8 byte filename
		// printf("Fifth checkpoint\n");

	// //Checking if filename was actually written:
	// lseek(disk,inodeDataOffset+inode_space*fourKB,SEEK_SET); 
	// char* holla;
	// holla=(char*)malloc(sizeof(char)*8);
	// read(disk,(void*)holla,8);
	// int yoy=0;
	// printf("%s WTAF\n",holla);

	char* ex;
	ex=(char*)malloc(sizeof(char)*2);
	memcpy((void*)ex,(void*)(&data_space),2); //Copying starting block of file (data)
	if(write(disk,(void*)ex,2)!=2) return -2; //Setting 2 byte starting block of file
	// printf("Sixth checkpoint\n");
	//free(ex);
	memcpy((void*)ex,(void*)(&block_size),2); //Copying number of blocks for file (data)
	if(write(disk,(void*)ex,2)!=2) return -2; //Setting 2 byte size of file (in blocks)
	// printf("Seventh checkpoint\n");
	//free(ex);
	ex=(char*)malloc(sizeof(char)*4);
	memcpy((void*)ex,(void*)(&actual_size),4); //Copying actual file size (data)
	if(write(disk,(void*)ex,4)!=4) return -2; //Setting 4 blocks for file size
	// printf("Eigth checkpoint\n");

	//Write data
	char* one_block;
	one_block=(char*)malloc(sizeof(char)*fourKB);
	// if(lseek(disk,dataOffset+(data_space*fourKB),SEEK_SET)<0) return -1; //Seek pointer to location where data is to be written
	// printf("Ninth checkpoint\n");

	//Write 4KB at at time
	for(i=0;i<block_size;i++)
	{
		memcpy(one_block,((char*)block),fourKB);
		if(writeData(disk,(dataOffset/fourKB)+i+data_space,(void*)one_block)<0) return -5; //Error writing to file
		block=block+fourKB; //4KB at a time
	}
	return 0;

}



int main()
{
	/*printf("\n");
	printf("Creating file system named test----\n");
	char fl[100] = "test.dat"; 
	int id = createSFS(fl,5*1024);

	printf("file system id: %d\n",id);

	char * filename=(char*)malloc(sizeof(char*)*4*1024); 
	char * content=(char*)malloc(sizeof(char*)*4*1024);

	printf("\n");
	printf("Enter name of the file to be created\n");
	scanf("%s",filename);
	printf("Enter content of the file\n");
	scanf("%s",content);
	int ret=writeFile(id,filename,(void*)content);
	
	if(ret==0)
	{
		printf("\n");
		printf("write file successfull\n");
	}

	else
	{
		printf("\n");
		printf("write file failed\n");
	}

	printf("\n");
	printf("inode bitmaps----\n");
	print_inodeBitmaps(id);
	printf("\n");
 	printf("data bitmaps----\n");
	print_dataBitmaps(id);


	return 0;*/

	int option,size,ret;
	char *data,*beta;
	char* filename=(char*)malloc(sizeof(char*)*1000); 
	printf("Enter name of SFS to be created\n");
	scanf("%s",filename);
	strcat(filename,".dat");
	printf("Enter file size of desired SFS (in MB)\n");
	scanf("%d",&size);
	size*=1024;
	int file_descriptor=createSFS(filename,size);
	if(file_descriptor<0)
	{
		printf("Error : Error creating SFS.Exiting..\n");
		return -10;
	}
	else
	{
		printf("SFS created.\n");
	}
	//Change from 4KB to something else
	data=(char*)malloc(sizeof(char*)*4*1024); //4KB block
	beta=(char*)malloc(sizeof(char*)*4*1024); //4KB block
	printf("Welcome to eyeNode UI\n");
	do
	{
		printf("1.Create new file\n");
		printf("2.Open file\n");
		printf("3.Display inode bitmap\n");
		printf("4.Display data bitmap\n");
		printf("5.Display information of all files on disk\n");
		printf("6.Exit\n");
		scanf("%d",&option);
		switch(option)
		{
			case 6: printf("Exiting.\n");
					break;
			case 5 : print_FileList(file_descriptor);
					break;
			case 4 : print_dataBitmaps(file_descriptor);
					break;
			case 3 : print_inodeBitmaps(file_descriptor);
					break;
			case 1 : printf("Enter desired filename\n");
					 scanf("%s",beta);
					 printf("Enter file content\n");
					 scanf("%s",data);
					 ret=writeFile(file_descriptor,beta,(void*)data);
					 printf("Returned %d\n",ret);
					 //Error handling for ret
					 break;
			case 2 : printf("Enter filename\n");
					 scanf("%s",beta);
					 ret=readFile(file_descriptor,beta,(void*)data);
					 //Error handling for ret
					 printf("Data read : \n%s\n",data);
					 printf("Returned %d\n",ret);
					 break;
			default : printf("Invalid option\n");

		}
	}while(option!=6);

}