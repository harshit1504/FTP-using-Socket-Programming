// gcc client.c -o client
// ./client <ip_add> <port>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <sys/stat.h>
#include <dirent.h> 
#include <sys/sendfile.h>
#include <fcntl.h>
#define cl_path "./client_files/"
int error(char *err)
{
    perror(err);
    exit(EXIT_FAILURE);
}

void sending(int sockfd,char *filename,int already_exist,int overwirte_choice)
{
    int filehandle,size,status;
    struct stat object;
    char filename1[50];
    strcpy(filename1,cl_path);
    strcat(filename1,filename);
    filehandle = open(filename1,O_RDONLY);			// open file with read only option 
    recv(sockfd,&already_exist,sizeof(int),0);		//recieving whether file already exist on server or not
    if(already_exist)
    {
        printf("%s file already exist in server\nPress 1 to Overwirte\nPress 2 for NO Overwirte\n\n",filename); // file is already exist
        l:
        scanf("%d",&overwirte_choice);   		//scanning overwrite choice
        getchar();
        if(overwirte_choice!=1 && overwirte_choice!=2)
        {
            printf("Invalid Choice. Please enter again\n\n");
            goto l;
        }
    }
    send(sockfd,&overwirte_choice,sizeof(int),0);           // sending overwrite choice 
    if(overwirte_choice==1)
    {

        stat(filename1, &object);
        size = object.st_size;					//getting size of the file
        send(sockfd, &size, sizeof(int), 0);		//sending file size
        sendfile(sockfd, filehandle, NULL, size);               // sending file 
        recv(sockfd, &status, sizeof(int), 0);				//recieving status
        if(status)
            printf("%s File stored successfully\n\n",filename);                           //status 
        else
            printf("%s File failed to be stored to remote machine\n\n" , filename); 
    }
    
}

void recieving (int sockfd,char *filename,int already_exist,int overwirte_choice)
{
    int size,filehandle,status;
    char *recvfile;
    char filename1[50];
    strcpy(filename1,cl_path);
    strcat(filename1,filename);
    recv(sockfd, &size, sizeof(int), 0);
    if(!size)
    {
        printf("No such file on the remote directory\n\n");         //file doesn't exist
        
    }
    else
    {
        if( access( filename1, F_OK ) != -1 ) 
        {
            already_exist = 1;
            printf("%s file already exist in client\nPress 1 to Overwirte\nPress 2 for NO Overwirte\n\n",filename);     // file already exist
            l:
            scanf("%d",&overwirte_choice);			//scanning overwrite choice
            getchar();
            if(overwirte_choice!=1 && overwirte_choice!=2)
            {
                printf("Invalid Choice. Please enter again\n\n");
                goto l;
            }

        }
        send(sockfd,&overwirte_choice,sizeof(int),0);                  //send overwrite choice

        if(overwirte_choice==1)
        {
        	if(already_exist==1)
        		filehandle = open(filename1, O_WRONLY | O_CREAT | O_TRUNC, 644);     // open file with all clear data(Case of overwrite i.e file already exist)
        	else
        		filehandle = open(filename1, O_CREAT | O_EXCL | O_WRONLY, 0666);     // open new file (case when file does'nt exist) 

	        recvfile = malloc(size);
            recv(sockfd, recvfile, size, 0);			//recieving file
            status=write(filehandle, recvfile, size);	//writing file on client side
            close(filehandle);
            if(status)
                printf("%s File recieved successfully\n\n",filename);                           //status 
            else
                printf("%s File failed to recieve \n\n" , filename);

        }
        
    }           
}
int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr; 

    if(argc != 3)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);		// checking argument 
        exit(EXIT_FAILURE);
    } 
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error("Failed to create a socket. Exiting! ");

    bzero(&servaddr, sizeof(servaddr)); 			//assigning  server address

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2])); 				// assigning port

    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr)<=0)
        error("\nError: Invalid address. Address not supported. ");

    if( connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)			// connect to the server
        error("\nError: Failed to connect to the remote host. ");

    char filename[20],buffer[100],ext[20],command[20];
    int status;
    int already_exist=0;
    int overwirte_choice = 1;
    while(1)
    {
        printf("Enter a choice:\nput <filename>\nget <filename>\nmput <extention>\nmget <extention>\nquit\n\n");
        bzero(buffer,100);
        fgets(buffer,100,stdin);				//getting input from user
        bzero(command,20);
        sscanf(buffer,"%s",command);			//getting command
 
//-------------->put file in server//
            if(strcmp(command,"put")==0)
            {														
                printf("put initiated\n\n");							
                sscanf(buffer, "%s%s", filename, filename);
                char filename1[50];
    			strcpy(filename1,cl_path);
    			strcat(filename1,filename);        // read the file name 
                if( access( filename1, F_OK ) == -1 )				//file does'nt exist on client side
                {
                    printf("%s does not exist in client side \n\n",filename );
                    continue;
                } 
                send(sockfd,buffer,100,0);				//send put command with file name to server
                sending( sockfd, filename, already_exist, overwirte_choice);		//sending file
                printf("put operation completed\n\n");                
             }
//---------------->get file from server//
            else if(strcmp(command,"get")==0)
            {
                printf("get initiated\n\n");                          
                sscanf(buffer, "%s%s", filename, filename);				// read the file name
                send(sockfd, buffer, 100, 0);				//send the get command with file name
                recieving ( sockfd, filename, already_exist, overwirte_choice); //receive file
                printf("get operation completed\n\n");           
            }
//------------------>mput server //
            else if(strcmp(command,"mput")==0)
            {
            	printf("mput initiated\n\n");					
                sscanf(buffer,"%s%s",ext,ext);         //  take the extionsion 
                DIR *di;
                struct dirent *dir;
                di = opendir(cl_path);			//opening directory
                while ((dir = readdir(di)) != NULL)			//traversing through directory
                {
                    char *fname=dir->d_name;
                    strcpy(filename,fname);
                    char* fext = strrchr(filename, '.');			//getting extension of file
                    if (fext==NULL)
                    {
                        continue;
                    }
                    if (strcmp(ext,fext)==0)
                    {
                        bzero(buffer,100);
                        strcpy(buffer,"put ");
                        strcat(buffer,filename);		
                        send(sockfd,buffer,100,0);         // send put command with filename
                        sending( sockfd, filename, already_exist, overwirte_choice);		//sending file
                        overwirte_choice = 1;
                    }
                }
                closedir(di);					//closing directory
                printf("mput operation completed\n\n");						
            }
//---------------->mget server//
            else if(strcmp(command,"mget")==0)
            {
            	printf("mget initiated\n\n");
                send(sockfd,buffer,100,0);									// sending buffer with choice and extension 
                int ready;
                while(1)
                {
                    recv(sockfd,&ready,sizeof(int),0);
                    if(ready==0)
                        break;

                    recv(sockfd,filename,20,0);							// recv file name 
                    recieving ( sockfd, filename, already_exist, overwirte_choice);  //receive file
                    overwirte_choice = 1;
                    already_exist = 0;
                }
                printf("mget operation completed\n\n");
                
            }
//------------------>quit the server//
            else if(strcmp(command,"quit")==0)
            {
                // strcpy(buffer, "quit");
                send(sockfd, buffer, 100, 0);					// sending quit command
                recv(sockfd, &status, 100, 0);					//receive status
                if(status)
                {
                    printf("Quitting..\n");
                    exit(0);
                }
                printf("Server failed to close connection\n");		
            }

//---------------> default choice//
            else
            	printf("choose the vaild option\n");

    }

    return 0;
}