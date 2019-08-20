// gcc server.c -o server
// ./server <port>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h> 
#include <sys/stat.h>   /*for getting file size using stat()*/
#include <sys/sendfile.h>   /*for sendfile()*/
#define sv_path "./server_files/"
int error(char *err)
{
	perror(err);
	exit(EXIT_FAILURE);
}

void sending(int connfd,char *filename,int overwrite_choice)
{
    int filehandle,size;
    struct stat object;
    char filename1[50];
    strcpy(filename1,sv_path);
    strcat(filename1,filename);
    if( access( filename1, F_OK ) != -1 )
    {
	    stat(filename1, &object);
	    filehandle = open(filename1, O_RDONLY);          // open file with read only option 
	    size = object.st_size;							//getting size of the file
	    send(connfd, &size, sizeof(int), 0);       // sending the size of file 
	    recv(connfd,&overwrite_choice,sizeof(int),0);   // receiving overwrite choice 
		if(overwrite_choice == 1)
    		sendfile(connfd, filehandle, NULL, size);		//send file
	    
	}
	else
	{
		size = 0;
	    send(connfd, &size, sizeof(int), 0);
	}
}
void recieving (int connfd,char *filename,int already_exist,int overwrite_choice)
{
    int size,status,filehandle;
    char *recvfile;
    char filename1[50];
    strcpy(filename1,sv_path);
    strcat(filename1,filename);
    if( access( filename1, F_OK ) != -1 )
    {
        already_exist = 1;
        send(connfd,&already_exist,sizeof(int),0);              // exist 
    } 
    else 
    {
        already_exist = 0;
        send(connfd,&already_exist,sizeof(int),0);             // not exist 
    }
    recv(connfd,&overwrite_choice,sizeof(int),0);               // receive overwrite choice 

    if(overwrite_choice==1)
    {
    	if(already_exist==1)
    		filehandle = open(filename1, O_WRONLY | O_CREAT | O_TRUNC, 644);     // open file with all clear data(Case of overwrite i.e file already exist)
    	else
    		filehandle = open(filename1, O_CREAT | O_EXCL | O_WRONLY, 0666);     // open new file (case when file does'nt exist)
    	
    	recv(connfd, &size, sizeof(int), 0);		//receive size of the file
        recvfile = malloc(size);
        recv(connfd, recvfile, size, 0);			//recieving file
        status=write(filehandle, recvfile, size);	//writing file on server
        close(filehandle);
        send(connfd, &status, sizeof(int), 0);		//sending status
    }
}

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Invalid execution format. Use the following format\n<executable code> <Server Port number>\n");
		exit(EXIT_FAILURE);
	}
    
    int listenfd = 0, connfd = 0;
    struct sockaddr_in servaddr , client; // predefined in header file netinet/in.h
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0)  ;   // create socket 
    	
    if (listenfd == -1) { 
        error("socket creation failed...\n"); 
    } 
    
	bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);              // ip address = INADDR_ANY ("0.0.0.0")
    servaddr.sin_port = htons(atoi(argv[1]));                      // port which in input 

    if ((bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))) != 0) 		// Binding newly created socket to given IP and verification 
    { 
        error("socket bind failed...\n");  
    } 

	if ((listen(listenfd, 10)) != 0) 								 // Now server is ready to listen and verification
	{ 
        error("Listen failed...\n"); 
    } 
    	 
    while(1)
    {
	    connfd = accept(listenfd, (struct sockaddr*)NULL, NULL) ;       // accept the connection of client 
	    if (connfd < 0) 
	    { 
        	error("server acccept failed...\n"); 
   		} 
   		printf("Connected with a client\n");
	    char buffer[100],command[5],filename[20],ext[20];        // defining variables 
	    bzero(buffer,100);
	    int i;
	    int already_exist = 0;
	    int overwrite_choice = 1;
	    
	    while(1)                    
	    {   
	        
	        recv(connfd,buffer,100,0);					
	        
	        sscanf(buffer,"%s",command);               // get command with file option 
	//------------->for put command //

	        if(!strcmp(command,"put"))
	        {
	            printf("Put initiated\n");
	            sscanf(buffer, "%s%s", filename , filename);        // store filename in var  
	            printf("Recieving %s file\n",filename);
	            recieving ( connfd, filename, already_exist, overwrite_choice);  //recieve file
	            bzero(buffer,100);
	            
	        }

	//------------------>get command//
	        else if(!strcmp(command,"get"))
	        {
	            printf("Get initiated\n");
	            sscanf(buffer, "%s%s", filename, filename);			// store filename in var 
	            printf("Sending %s file\n",filename);
	            sending(connfd,filename,overwrite_choice);			//send file
	            bzero(buffer,100);
	        }


	//--------------------->mget command //
	        else if(!strcmp(command,"mget"))
	        {
	            printf("mGet initiated\n");
	            sscanf(buffer,"%s%s",ext,ext);         // get the extension
	            printf("Sending all files with %s extension\n",ext);
	            char *fname;
	            DIR *di;
	            struct dirent *dir;
	            di = opendir(sv_path);				//opening directory
	            int ready=1;
	            while ((dir = readdir(di)) != NULL)			//traversing through all files in the directory
	            {
	                fname=dir->d_name;
	                strcpy(filename,fname);
	                
	                char* fext = strrchr(filename, '.');		//getting extension of the file
	                if (fext==NULL)
	                {
	                    continue;
	                }
	                if (strcmp(ext,fext)==0)				
	                {
	                    printf("Sending %s file\n",filename);
	                    send(connfd, &ready, sizeof(int), 0);
	                    send(connfd,filename,20,0);							//send filename to client
	                    sending(connfd,filename,overwrite_choice);			//send file
	                }
	            } 
	            ready=0;
	            send(connfd, &ready, sizeof(int), 0);
	            closedir(di);										//closing directory
	            bzero(buffer,100);

	        }

	//-------------------->quit command//
	        else if(!strcmp(command, "quit"))
	        {
	            i = 1;
	            send(connfd, &i, sizeof(int), 0);           // closing the server 
	            break;
	        }


	    }
	}
}