/*
 *
 * Purpose: The server acts as a memory manager and allocates memory for as many clients as possible using paging as its memory allocation scheme.
 *
 * Input: Number of clients and total frames
 *
 * Output: Map of allocated memory
 *
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

int clean_stdin()
{
    while (getchar()!='\n');
    return 1;
}

int main ()
{

	//Struct contains output
  struct values{
	  int fragmentation;
	  int frame[10];
	  int frameCount;
	  char error[50];//Error string
	  int errored;  //0 means ,no error and 1 is error
  };

  //struct contains input
  struct value{
 	  int memoryReq;
 	  char programName[100];
 	  char fifoName[50];
   };

   int fcom;  // to read from client
   int no_of_clients,i; //No of clients
   int frameSize=256; //Each frame size
   int no_of_frames; //Total frames
   int fda;//to write to client
   static int frame_no=0; //Frame number

   //Read no of clients from user
    printf("Please enter the number of clients[atleast 3]:");
    int val=scanf("%d",&no_of_clients);

	 while(val!=1 || no_of_clients<1)
    {
 		   while((getchar()) !='\n')
              continue;

           printf("Invalid input for number of clients\nPlease enter the number of clients:");
           val=scanf("%d", &no_of_clients);
       }

	 //Read no of frames from user
    printf("Please enter the number of frames [Frame size is 256 bytes]:");
       val=scanf("%d",&no_of_frames);

       while(val!=1 || no_of_frames<12)
          {
       		   while((getchar()) !='\n')
                    continue;

                 printf("Invalid input for number of frames\nPlease enter the number of frames[Frame size is 256 bytes]:");
                 val=scanf("%d", &no_of_frames);
             }

  struct value input[no_of_clients];//input array of structs to read data from client
  struct values output[no_of_clients]; //output array of structs to send data to client
  int maxMemorySize=frameSize*no_of_frames;  //Maximum Memory Size

  printf("Maximum memory is %d\n",maxMemorySize);

  printf("Waiting for clients..\n");
  /* Create the fifos and open them  */
  if ((mkfifo("COMMON_FIFO",0666)<0 && errno != EEXIST))
    {
    perror("can't create COMMON_FIFO");
    exit(-1);
    }

    //Opening common FIFO for reading
  if((fcom=open("COMMON_FIFO", O_RDONLY))<0)
     printf("can't open fifo to read");

  //Reading the data from clients
  for(i=0;i<no_of_clients;i++){
	  read(fcom, &input[i], sizeof(input));   //read the integer
	  printf("Server: Program name:%s",input[i].programName);
	  printf("Server: Fifo name:%s\n",input[i].fifoName);
	  printf("Server: Memory Request:%d\n",input[i].memoryReq);
  }
  printf("*******************************************************************\n");
  printf("Name\t\t\tFrames\t\t\tFragmentation\n");
  //Calculating frames,fragmentation for each client
  for(i=0;i<no_of_clients;i++){
	  size_t n = strlen( input[i].programName );
	 printf("%.*s\t\t\t",n-1,input[i].programName);
  if(input[i].memoryReq<maxMemorySize){
	  int reminder,frames;

	   reminder=input[i].memoryReq%frameSize;
	  // printf("Reminder %d\n",reminder);
	   if(reminder==0){
	 	  frames=input[i].memoryReq/frameSize;
	 }else{
	 	  output[i].fragmentation=frameSize-reminder;
	 	  frames=(input[i].memoryReq-reminder)/frameSize;
	 	  frames+=1;
	   }
	   output[i].errored=0;
	   for(int j=0;j<frames;j++){
		   output[i].frameCount=frames;
		   output[i].frame[j]=frame_no;
		   printf("%d ",frame_no++);
	   }

	   printf("\t\t\t%d\t\t\t\n",output[i].fragmentation);
	   maxMemorySize-=frames*frameSize;
  }else{
	 output[i].errored=1;
	 printf("Insufficient memory\n");
	 strcpy(output[i].error, "Insufficient memory");
  }

  //Writing data to client
  if((fda=open(input[i].fifoName,O_WRONLY))<0)
	  printf("can't open fifo to write");

	  write(fda,&output[i],sizeof(output[i]));

  }

  printf("*******************************************************************\n");
 
  printf("Server: I am ready to close\n");

  close(fda);
  close(fcom);
  unlink("COMMON_FIFO");

  return 0;
}
