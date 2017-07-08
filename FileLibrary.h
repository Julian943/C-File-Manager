//Francisco Julián Huerta y Munive A01323513
//Eduardo Luna Gutiérrez A01322804

#include <stdio.h> //i/o 
#include <unistd.h> //write, close, read and write methods
#include <fcntl.h> //Flags values
#include <stdlib.h> //malloc / realloc
#include <string.h> //string methods

typedef struct fileBuffer{
	//Read part (and others)
	size_t  buffsize; //Size of buffer
	int buffCap; //Counts how many bytes have been read/put
	int fileDescriptor; //Points to the file handled
	void * buffer; //Buffer
	void * lastPlaceRead; //Points to the last place read by the buffer
	int flags; //stores the permissions of the file

	//Write part
	void * writeBuffer;
	size_t writeBufferSize;
	int writeBufferCap;
} myFile;

struct fileBuffer * openFile(char * filePath, char * mode){
	struct fileBuffer * file;
	int flag, fileDes;
	mode_t permission;
	size_t sizeBuffer;
	size_t writeBufferValue;


	flag = -1; 
	permission = -1; //Handles the create file flag
	sizeBuffer = 1000; //size of buffer
	writeBufferValue = 1000;
	fileDes = 0;
	//Define flag
	if(strcmp(mode,"r")   == 0)  flag = O_RDONLY;
	if(strcmp(mode,"wa")  == 0)  flag = O_WRONLY | O_APPEND;
	if(strcmp(mode,"wt")  == 0)  flag = O_WRONLY | O_TRUNC;
	if(strcmp(mode,"w")   == 0)  flag = O_WRONLY;
	if(strcmp(mode,"rwa") == 0)  flag = O_RDWR | O_APPEND;
	if(strcmp(mode,"rwt") == 0)  flag = O_RDWR | O_TRUNC;
	if(strcmp(mode,"rw")  == 0)  flag = O_RDWR;
	if(strcmp(mode,"rc" ) == 0) {flag = O_RDONLY | O_CREAT; 
		permission = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;}
	if(strcmp(mode,"wc" ) == 0) {flag = O_WRONLY | O_CREAT; 
		permission = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;}
	if(strcmp(mode,"rwc") == 0) {flag = O_RDWR | O_CREAT; 
		permission = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;}
	//Get the file descriptor
	fileDes = open(filePath,flag, permission);
	//printf("File des: %d. Flag: %d. Mode: %s.\n",fileDes, flag,mode);
	if(flag == -1){
		printf("Flag was not set\n");
		return NULL;
	}
	if(fileDes < 0){
		printf("Error while opening the file\n");
		return NULL;
	}
	else{
		//Create the file struct
		file = (struct fileBuffer *) malloc(sizeof(struct fileBuffer)) ;
		file->buffsize = sizeBuffer;
		file->fileDescriptor = fileDes;
		file->buffer = malloc(sizeBuffer);
		file->lastPlaceRead = file->buffer;
		file->flags = flag;
		file->buffCap = 0;
		file->writeBufferSize = writeBufferValue;
		file->writeBuffer = malloc(writeBufferValue);
		file->writeBufferCap = 0;


		//printf("Created successfully\n");
		return file;
	}
}

void closeFile(struct fileBuffer * file){
	if(file == NULL) {
		printf("File is NULL\n");
		return;
	}

	if(file->writeBufferCap != 0){
		int writeRes = write(file->fileDescriptor,file->writeBuffer, file->writeBufferCap);
		if(writeRes == file->writeBufferCap){
			printf("Stuff written successfully\n");
		}
		else if(writeRes < file->writeBufferCap){
			printf("%d Some things were not written\n",writeRes );
		}
		else if(writeRes == 0){
			printf("Something went wrong\n");
		}
	}

	int cl = 10;
	cl = close(file->fileDescriptor);
	if(cl == 0) printf("");
		//printf("File closed\n");
	else
		printf("Error while closing the file\n");
	free(file);
}

//Deprecated methods. Work when each element in the array is read as a byte. Might be usefull for someone
	void * fetchBuffer(struct fileBuffer * file, int size){

		if(file->buffCap == 0){ //When the buffer is newly fetched
			ssize_t readResult = read(file->fileDescriptor,file->buffer,file->buffsize);
			file->lastPlaceRead = file->buffer;
			file->buffCap = 0;
			if(readResult == 0){
				printf("Might have reached EOF\n");
			}
			else if(readResult < file->buffsize){
				printf("Might have read the last piece of the file, %d\n", (int)readResult);
			}
			else if(readResult < 0){
				printf("Something went wrong\n");
			}

		}

		int dif = file->buffsize - file->buffCap;

		void * rem = malloc(size); //Returns the segment of the specified size
		if(dif < size){ //What's left is not enough
			int left = size - dif; //Knows the missing bytes (Need to fetch left bytes more)
			void * rest = rem; //Points to start of rem
			memcpy(rem,file->lastPlaceRead,dif); //Copy to rem dif bytes
			rest+=dif; //Displaces dif places, pointing to what's left of rem
			//Read from file section
			ssize_t readResult = read(file->fileDescriptor,file->buffer,file->buffsize);
			file->lastPlaceRead = file->buffer;
			file->buffCap = 0;
			if(readResult == 0){
				printf("Might have reached EOF\n");
				return rem;
			}
			else if(readResult < file->buffsize){
				printf("Might have read the last piece of the file\n");
			}
			else if(readResult < 0){
				printf("Something went wrong\n");
			}
			//End Read from file section
			//Copy left bytes to where points rest(first empty bit of rem)
			memcpy(rest,file->lastPlaceRead,left);
			//Displace lastPlaceRead left places 
			file->lastPlaceRead+=left;
			file->buffCap +=left;
		}
		else{//There is still place in the buffer
			memcpy(rem,file->lastPlaceRead,size);//Copy were LPR was
			file->lastPlaceRead+=size; //Displace size bytes
			file->buffCap +=size;

		}
		return rem;
		/*
		memcpy(caracter,str,strlen(str));
		memcpy(aDonde,deDonde,cuanto)
		*/
	}

	char readChar(struct fileBuffer * file){
		return *(char *)fetchBuffer(file,sizeof(char));
	}

	float readFloat(struct fileBuffer * file){
		return *(float *)fetchBuffer(file,sizeof(float));
	}
	double readDouble(struct fileBuffer * file){
		return *(double *)fetchBuffer(file,sizeof(double));
	}
	short readShort(struct fileBuffer * file){
		return *(short *)fetchBuffer(file,sizeof(short));
	}
	int readInt(struct fileBuffer * file){
		return *(int *)fetchBuffer(file,sizeof(int));
	}
	long readLong(struct fileBuffer * file){
		return *(long *)fetchBuffer(file,sizeof(long));
	}
	long double readLongDouble(struct fileBuffer * file){
		return *(long double *)fetchBuffer(file,sizeof(long double));
	}
	unsigned char readUChar(struct fileBuffer * file){
		return *(unsigned char *)fetchBuffer(file,sizeof(unsigned char));
	}
	unsigned short readUShort(struct fileBuffer * file){
		return *(unsigned short *)fetchBuffer(file,sizeof(unsigned short));
	}
	unsigned int readUInt(struct fileBuffer * file){
		return *(unsigned int *)fetchBuffer(file,sizeof(unsigned int));
	}
	unsigned long readULong(struct fileBuffer * file){
		return *(unsigned long *)fetchBuffer(file,sizeof(unsigned long));
	}

	char * readString(struct fileBuffer * file, int size){
		return (char *)fetchBuffer(file,size);
	}

//End deprecated methods

//since we are going to be treating this as char array. We go element by element. which means our buffer is only going to be filled again and again, practically a read
void fillBuffer(struct fileBuffer * file){
	if(file->buffCap == 0 || file->buffCap == file->buffsize){
		file->buffer = malloc(file->buffsize);
		ssize_t readResult = read(file->fileDescriptor,file->buffer,file->buffsize);
		
		file->buffCap = 0;
		file->lastPlaceRead = file->buffer;

		if(readResult == file->buffsize){
			printf("%d. Buffer successfully filled\n", (int)readResult);
		}
		else if(readResult < file->buffsize){
			printf("%d. Some bytes were not read. Might have reached EOF\n", (int)readResult);
		}
		else if(readResult < 0){
			printf("%d. Something went wrong\n", (int)readResult);
		}
	}	
}


//Chars


char fetchChar(struct fileBuffer * file){
	fillBuffer(file);
	void * res = malloc(sizeof(char));
	memcpy(res,file->lastPlaceRead,sizeof(char));

	file->lastPlaceRead++;
	file->buffCap++;

	return *(char *)res;
}
unsigned char fetchUChar(struct fileBuffer * file){
	return (unsigned char)fetchChar(file);
}
//Returns array of char of size n
char * fetchStringSize(struct fileBuffer * file, int size){
	
	char * str = (char *)malloc(size);
	char * ch = str;
	int i;
	for(i = 0; i < size; i++){
		*ch = fetchChar(file);
		ch++;
	}


	return str;
}

//Returns array of char until space is found
char * fetchString(struct fileBuffer * file){

	char * str = (char *)malloc(sizeof(char));
	char ch = fetchChar(file);
	int size = 0;
	
	while(ch != 10 && ch != 0 && ch != 32){
		
		*(str + size) = ch;
		size++;
		str = realloc(str,strlen(str) + sizeof(char));
		ch = fetchChar(file);
		
	}

	return str;
}


//Floats
double fetchDouble(struct fileBuffer * file){
	return atof(fetchString(file));
}
float fetchFloat(struct fileBuffer * file){
	return (float)fetchDouble(file);
}
//Ints
long fetchLong(struct fileBuffer * file){
	return atol(fetchString(file));
}
unsigned long fetchULong(struct fileBuffer * file){
	return (unsigned long)fetchLong(file);
}
int fetchInt(struct fileBuffer * file){
	return (int)fetchLong(file);
}
unsigned int fetchUInt(struct fileBuffer * file){
	return (unsigned int)fetchLong(file);
}
short fetchShort(struct fileBuffer * file){
	return (short)fetchLong(file);
}
unsigned short fetchUShort(struct fileBuffer * file){
	return (unsigned short)fetchLong(file);
}

//Write

/*
void * writeBuffer;
size_t writeBufferSize;
int writeBufferCap;
if(int or float)
	turn to string.
	writeString(file)
if(string)
	writeString()


*/

//Write int, write float, write string

char * reverseString(char * str){
	char * res = (char * ) malloc(strlen(str));
	char * ptr = res;
	int offset = strlen(str)-1;
	//printf("%c\n",*(str+offset) );
	while(offset >= 0){
		*ptr = *(str+offset);
		offset--;
		ptr++;
	}
	return res;
}

void writeString(struct fileBuffer * file, char * string){
	void * ptr = string; //Points to the first unused char of the string
	void * lastWritten = file->writeBuffer + file->writeBufferCap; //Goes to the last unused char of the buffer
	int writeRes;

	int dif = file->writeBufferSize - file->writeBufferCap;
	if(dif > 0 ){ //Theres space in the buffer
		if(strlen(string) < dif){ //String fits in buffer
			memcpy(lastWritten,string,strlen(string)); //Put whole string in there
			file->writeBufferCap+=strlen(string);//Increase the bufferCap
		}
		else{ //String wont fit
			int restOfString = strlen(string) - dif;
			memcpy(lastWritten, string, dif); //Copy what you can in the buffer

			writeRes = write(file->fileDescriptor,file->writeBuffer,file->writeBufferSize); //now that buffer is full, write it to file

			if(writeRes == file->writeBufferCap){
				printf("Stuff written successfully\n");
			}
			else if(writeRes < file->writeBufferCap){
				printf("%d Some things were not written\n",writeRes );
			}
			else if(writeRes == 0){
				printf("Something went wrong\n");
			}

			file->writeBuffer = malloc(file->writeBufferSize); //Get new memory for buffer
			file->writeBufferCap = 0; //Reset the cap
			ptr+=dif; //Displace de ptr to what's left of the string
			void * left = malloc(restOfString); //Get space for the rest of the string
			memcpy(left,string+dif, restOfString); //Copy what's left to it;
			writeString(file, left);

		}
	}
	else{ //There is no space in buffer
		writeRes = write(file->fileDescriptor,file->writeBuffer,file->writeBufferSize); 
		if(writeRes == file->writeBufferCap){
			printf("Stuff written successfully\n");
		}
		else if(writeRes < file->writeBufferCap){
			printf("%d Some things were not written\n",writeRes );
		}
		else if(writeRes == 0){
			printf("Something went wrong\n");
		}
		file->writeBuffer = malloc(file->writeBufferSize); //Get new memory for buffer
		file->writeBufferCap = 0; //Reset the cap
	}
}

void writeInt(struct fileBuffer * file, long num){
	char * str = (char *)malloc(sizeof(char)); //int to string
	char * ptr = str; //last used char in str
	int res = 0; //Stores last digit
	int offset = 0; //Distance between 1st char and current

	while(num != 0){
		res = num%10; //Get last digit
		num/=10; //Ditch last digit from num
		*(ptr+offset) = (res + 48); //Store it in the string as char
		offset++; //Increase offset
		str = realloc(str,strlen(str) + sizeof(char)); //Realloc memory to str in case more numbers come
		ptr = str; //makes ptr point to str in case str is reasigned to different memory address
	}

	
	writeString(file,reverseString(str));
}
//Note: Unexpected behavior when tried writing floatings point of only one decimal that value is 1 and 3 and 6 (ie: 10.1)
void writeDouble(struct fileBuffer * file, double num){

	//Write integer part and dot
	writeInt(file,(long)num);
	writeString(file,".");

	double decPart = num - (long)num; //Get decimal part
	long res = 0;
	int streak = 0; //If this thing reaches 3, we assume the double number has only zeroes left on its decimal part
	int mod = 0;

	//This thing multiplies by 10 the decimal part, extracting the first digit to the int side of the number. We build a long number using this iteration until we find 3 zeroes in a row. Then we write it as integer

	while(streak != 3){
		res*=10; 	//res = 0		 	   
		decPart*=10; //1.0	
		mod = ((int)decPart)%10;	
		res+=mod;			

		if(mod == 0) streak++; 	
		decPart = decPart - mod;
	}
	writeInt(file,res);
}
