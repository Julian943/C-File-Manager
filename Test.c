//Francisco Julián Huerta y Munive A01323513
//Eduardo Luna Gutiérrez A01322804
#include "FileLibrary.c"

int main(){
	myFile * readFile;
	myFile * writeAppend;
	myFile * writeTruncate;
	myFile * writeFile;
	myFile * readWriteAppend;
	myFile * readWriteTruncate;
	myFile * readWrite; 
	myFile * readCreate;
	myFile * writeCreate;
	myFile * readWriteCreate;

	printf("This throws error. File won't exist.\n");
	writeFile = openFile("writeFile.txt","w");
	printf("This throws another error. File won't exist.\n");
	readWrite = openFile("readWriteCreate.txt","rw");
	
	//Creates files
	readCreate = openFile("file1.txt","rc");
	writeCreate = openFile("file2.txt","wc");
	readWriteCreate = openFile("file3.txt","rwc");
	closeFile (readCreate);
	closeFile (writeCreate);
	closeFile (readWriteCreate);


	//Will write somethings
	writeAppend = openFile("file1.txt","wa"); //Will write stuff
	
	



	writeInt(writeAppend,60); 
	writeString(writeAppend," ");
	writeDouble(writeAppend, 1123.123);
	writeString(writeAppend," ");
	writeString(writeAppend, "This is written on file1.txt");
	closeFile (writeAppend);
	

	readFile = openFile("file1.txt","r"); //Will read all things from the file
	printf("Read six: %c\n",fetchChar(readFile) );
	printf("Read zero: %c\n",fetchUChar(readFile) );
	printf("Read empty str: %s\n",fetchString(readFile) );
	printf("Read float %lf\n",fetchFloat(readFile) );
	printf("Read string until space:%s\n",fetchString(readFile) );
	closeFile (readFile);
	

	writeTruncate = openFile("file1.txt","wt"); //Will truncate all things that were written
	writeString(writeTruncate, "This will be all that is left");
	closeFile (writeTruncate);


	readFile = openFile("file1.txt","r");
	printf("%s\n",fetchStringSize(readFile, 100) );
	closeFile (readFile);
	
	


	
    return 0;
}
