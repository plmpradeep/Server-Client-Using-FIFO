/*
 *
 * Purpose: Client send the name of one job and a memory request size to the server and display frames, fragmentation results
 *
 * Input: Job name and memory request size
 *
 * Output: Frame numbers, fragmentation or error
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
  }output;

  //struct contains input
  struct value{
 	  int memoryReq;
 	  char programName[100];
 	  char fifoName[50];
   }input;

  int fda;  // to write data to server
  int pid;//used to store dynamic id
  int fdb; //to read data from server

  //Initialize input,output
  memset(&input,0,sizeof input);
  memset(&output,0,sizeof output);

  //Dynamic FIFO
  pid=getpid();
  sprintf(input.fifoName,"FIFO_%d",pid);
  printf("Client : FIFO Name %s \n",input.fifoName);

  //Read program name from user
  printf("Client: Please enter the program name: ");
  fgets( input.programName, sizeof(input.programName), stdin );

  //Read memory request from user
  printf("Client: Please enter the memory request:");

  	 int value=scanf("%d", &input.memoryReq);

  	 while(value!=1 || input.memoryReq<1)
     {
  		   while((getchar()) !='\n')
               continue;

            printf("Invalid input for memory request\nClient: Please enter the memory request:");
            value=scanf("%d", &input.memoryReq);
        }


 //creating dynamic fifo
  if ((mkfifo(input.fifoName,0666)<0 && errno != EEXIST))
   		    {
   		    perror("can't create privateFIFO");
   		    exit(-1);
   		    }


//Opening commonFIFO for writing
  if((fda=open("COMMON_FIFO", O_WRONLY))<0)//opening and validating fifo1
       printf("can't open fifo to write");
//Write the data
    write(fda, &input, sizeof(input));
    printf("Client: Got the data sent, now waiting for response\n");
//Open privateFIFO for reading data from server
    if((fdb=open(input.fifoName, O_RDONLY))<0)//opening and validating fifo1
          printf("can't open fifo to read\n");

    read(fdb, &output, sizeof(output));

    //Display the results
    printf("**************************************************************\n");
    if(output.errored==0){
    	printf("Frame\t\t\tFragmentation\t\t\t\n");
    	for(int j=0;j<output.frameCount;j++)
    		printf("%d ",output.frame[j]);
    	printf("\t\t\t%d\t\t\t\n",output.fragmentation);
    }else{
     printf("%s\n",output.error);
    }
    printf("**************************************************************\n");
    //Close
    close(fda);
    close(fdb);


    printf ("all done!\n");
    unlink(input.fifoName);

    return 0;

}
