
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/types.h>
#include<ctype.h>
#include<netdb.h>
#include<time.h>

#define MAX_LENGTH 4096
#define MAX_PASSWORD_LENGTH 50;


void displayResult(char* result[],int choice);

void makeContact(char *reply)
{
    //the message that we pass to the server
    char prompt1[]="AUTH secretpassword";
    
    //hard-code the IP address and port of the server
    char serverIP[]="129.170.213.101";
    int serverPort=47789;
    
    //define the sockid and the serveraddr
    int sockid;
    struct sockaddr_in serveraddress;
    
    int connectionid;
    //CREATE the socket
    //the AF_INET parameter indicates that its IPv4
    sockid=socket(AF_INET, SOCK_STREAM,0);
    if(sockid<0)
    {
        perror("the socket wasn't created properly!!");
    }
    
    //clear the serveraddress and set the values
    memset(&serveraddress,0,sizeof(serveraddress));
    serveraddress.sin_family=AF_INET;
    serveraddress.sin_addr.s_addr=inet_addr(serverIP);
    serveraddress.sin_port=htons(serverPort);
    
    connectionid=connect(sockid,(struct sockaddr *)&serveraddress,sizeof(serveraddress));
    if(connectionid<0)
    {
        perror("The client couldn't connect to the server!!");
        exit(1);
    }
    
    char sendbuffer[50];
    char recievebuffer[4096];
    memset(sendbuffer,0,sizeof(sendbuffer));
    sprintf(sendbuffer,"%s\n",prompt1);
    send(sockid,sendbuffer,strlen(sendbuffer),0);
    recv(sockid,recievebuffer,4096,0);
    strcpy(reply,recievebuffer);
}


void clientAuthenticate(char *reply,char *message,int choice)
{
    int i;
    char *str;
    char *r=reply;
    char sendbuffer[1000];
    char recievebuffer[1000];


    //hard-code the IP address and port of the server
    char serverIP[]="129.170.213.101";
    int serverPort=47789;

//**************** RETREIVE PASSWORD AND NEW PORT NUMBER FROM SERVER REPLY *********************

    //Parse the values in the returned string
    char **info;
    info=malloc(sizeof(char)*4);
    for(i=0;i<4;i++)
    {
        info[i]=(char *)malloc(sizeof(char)*MAX_LENGTH);
    }
    i=0;
    while((str=strtok(r," "))!=NULL)
    {
        sprintf(info[i],"%s",str);
        r=NULL;
        i++;
    }
    
    //get the password from the returned value
    char server_password[50];
    memset(server_password,0,sizeof(server_password));
    char *temp=malloc(sizeof(char)*50);
    temp=strtok(info[3],"\n");
    strcpy(server_password,temp);
    
    //get the port number from the returned value
    char *newPort=malloc(sizeof(char)*6);
    strcpy(newPort,info[2]);
    
    //set the socket again with the new port
    struct sockaddr_in serveraddress;
    memset(&serveraddress,0,sizeof(serveraddress));
    serveraddress.sin_family=AF_INET;
    serveraddress.sin_addr.s_addr=inet_addr(serverIP);
    serveraddress.sin_port=htons(atoi(newPort));
    
    //create a new socket before you create new connection
    int sockid_new;
    sockid_new=socket(AF_INET, SOCK_STREAM,0);
    if(sockid_new<0)
    {
        perror("the socket wasn't created properly!!");
    }
    
    int connectionid;
    connectionid=connect(sockid_new,(struct sockaddr *)&serveraddress,sizeof(serveraddress));
    if(connectionid<0)
    {
        perror("The client couldn't connect to the server!!");
        exit(1);
    }
    
    
    //create the password to be sent to the server
    memset(sendbuffer,0,sizeof(sendbuffer));
    memset(recievebuffer,0,sizeof(recievebuffer));
    sprintf(sendbuffer,"AUTH %s\n",server_password);
    int n;
    n=send(sockid_new,sendbuffer,strlen(sendbuffer),0);
    if(n<0)
    {
        perror("error writing to the server!");
        exit(1);
    }
    n=recv(sockid_new,recievebuffer,4096,0);
    if(n<0)
    {
        perror("error reading from the server");
        exit(1);
    }

    //****************** SEND THE CHOICE TO THE SERVER ***********************
    
    char **result;
    result=malloc(sizeof(char)*2);
    result[0]=malloc(sizeof(char)*50);
    result[1]=malloc(sizeof(char)*25);
    memset(sendbuffer,0,sizeof(sendbuffer));
    memset(recievebuffer,0,sizeof(recievebuffer));
    sprintf(sendbuffer,"%s\n",message);
    n=send(sockid_new,sendbuffer,strlen(sendbuffer),0);
    if(n<0)
    {
        perror("error writing to the server!");
        exit(1);
    }
    
    n=recv(sockid_new,recievebuffer,4096,0);
    if(n<0)
    {
        perror("error reading from the server");
        exit(1);
    }
    
    //parse the result to get the time and measurement
    char *rtemp=recievebuffer;
    char *strtemp;
    i=0;
    while((strtemp=strtok(rtemp," "))!=NULL && i<2)
    {
        sprintf(result[i],"%s",strtemp);
        rtemp=NULL;
        i++;
    }
    
    displayResult(result,choice);
    
    //************* SEND CLOSE TO SERVER *******************
    char closer[]="CLOSE";
    memset(sendbuffer,0,sizeof(sendbuffer));
    memset(recievebuffer,0,sizeof(recievebuffer));
    sprintf(sendbuffer,"%s\n",closer);
    n=send(sockid_new,sendbuffer,strlen(sendbuffer),0);
    if(n<0)
    {
        perror("error writing close to the server!");
        exit(1);
    }
    
    n=recv(sockid_new,recievebuffer,4096,0);
    if(n<0)
    {
        perror("error reading bye from the server");
        exit(1);
    }
}



int main()
{
    
    //store the users choice
    int choice;
    
    char *reply=malloc(sizeof(char)*1000);
    char message[50];
    memset(message,0,50);
    
    while(1)
    {
    
    //PRINT THE OPTIONS MENU :
    printf("\n\n\nWELCOME TO THE THREE MILE ISLAND SENSOR NETWORK\n");
    printf("\n\nWhich sensor would you like to read : ");
    printf("\n\n(1) Water Temperature\n(2) Reactor Temperature\n(3) Power Level\n");
    printf("Selection : ");
    scanf("%d",&choice);
        switch(choice)
        {
            case 1 :
            {
                strcpy(message,"WATER TEMPERATURE");
                break;
            }
            case 2 :
            {
                strcpy(message,"REACTOR TEMPERATURE");
                break;
            }
            case 3:
            {
                strcpy(message,"POWER LEVEL");
                break;
            }
        }

        makeContact(reply);
        clientAuthenticate(reply,message,choice);
    }
    
    exit(0);
}


//******************************** HELPER FUNCTIONS **********************************

void displayResult(char* result[],int choice)
{
    
    //Convert the epoch time to human readable time here
    time_t current_time;
    char *timeString;
    current_time=strtoul(result[0],NULL,0);
    timeString=ctime(&current_time);
    memcpy(timeString,timeString,strlen(timeString)-3);
    
    //remove the \n at the end of the c
    char *timer=timeString;
    char *r;
    r=strtok(timer,"\n");
    
    if(choice==1)
    {
        printf("The last WATER TEMPERATURE was taken at %s and was %s F",r,result[1]);
    }
    else if(choice==2)
    {
        printf("The last REACTOR TEMPERATURE was taken at %s and was %s F",r,result[1]);
    }
    else if(choice==3)
    {
        printf("The last POWER LEVEL was taken at %s and was %s Mw",r,result[1]);
    }
}

