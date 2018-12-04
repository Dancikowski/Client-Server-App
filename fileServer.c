#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#define PORT htons(4445)

void sendFilesList(int sckt){
    DIR *mydir;
    struct dirent *myfile;
    struct stat mystat;
    int counter = 0;
    char buf[512];
    mydir = opendir("/home/damian/Downloads/");

    while((myfile = readdir(mydir)) != NULL)
    {
        if(myfile->d_type != 4)
        counter++;
    }
    
    int i = 0;
    closedir(mydir);
    mydir = opendir("/home/damian/Downloads/");

    char **names;
    names = malloc(counter * sizeof(char*));
    
    while((myfile = readdir(mydir)) != NULL)
    {
        if(myfile->d_type != 4){
            size_t fileNameLenght = strlen(myfile->d_name);
            names[i] = malloc((fileNameLenght+1) * sizeof(char));
            strcpy(names[i], myfile->d_name);
            i++;
        }
    }
    closedir(mydir);

    send(sckt, &counter, sizeof(int),0 );

    
    for (int i=0; i<counter; i++){
            int len = strlen(names[i]);
            send(sckt, &len, sizeof(int),0);
            send(sckt, names[i], strlen(names[i]),0 );
    }

}

void handleConnection(int sckt)
{
    char sciezka[512];
    long long file_len, sentBytes, totalSendBytes, readBytes;
    struct stat fileinfo;
    FILE* file;
    unsigned char bufor[1024];
    unsigned char myFileSize[1024];
    
    memset(sciezka, 0, 512);
    if (recv(sckt, sciezka, 512, 0) <= 0)
    {
        return;
    }
    
    printf("Klient chce pobrać plik o następującej ścieżce: %s\n", sciezka);
    
    if (stat(sciezka, &fileinfo) < 0)
    {
        printf("Nie mozna pobrac informacji o pliku\n");
        return;
    }
    
    if (fileinfo.st_size == 0)
    {
        printf("Potomny: rozmiar pliku 0\n");
        return;
    }
    
    printf("Dlugosc pliku: %d B\n\n", fileinfo.st_size);
    
    file_len = (long) fileinfo.st_size;

    if (send(sckt, &file_len, sizeof(long), 0) != sizeof(long))
    {
        printf("Blad przy wysylaniu wielkosci pliku\n");
        return;
    }
    
    file_len = fileinfo.st_size;
    totalSendBytes = 0;
    file = fopen(sciezka, "rb");
    if (file == NULL)
    {
        printf("Blad przy otwarciu pliku\n");
        return;
    }

    int divide = fileinfo.st_size / 100;
    int i =1;

    fprintf(stderr, "Wysyłanie pliku: [");
    
    while (totalSendBytes < file_len)
    {
        readBytes = fread(bufor, 1, 1024, file);
        sentBytes = send(sckt, bufor, readBytes, 0);
        if (readBytes != sentBytes){
            break;
        }
        totalSendBytes += sentBytes;
        if (totalSendBytes >= divide * i){

            for (int k=0; k < totalSendBytes / (divide * i); k++){
                fprintf(stderr, "#");   
            }
                i++;
            }
        }
    
    if (totalSendBytes == file_len){

        fprintf(stderr, "]");
        printf("\n");
        printf("Plik wyslany poprawnie!\n\n");
    }
    else
        printf("Blad przy wysylaniu pliku\n");
    fclose(file);

   
    return;    
}


int main(void)
{
    int sckt_listen, sckt_client;
    struct sockaddr_in adr;
    socklen_t adr_len = sizeof(struct sockaddr_in);
    char command[10];
    sckt_listen = socket(PF_INET, SOCK_STREAM, 0);
    adr.sin_family = AF_INET;
    adr.sin_port = PORT;
    adr.sin_addr.s_addr = INADDR_ANY;
    memset(adr.sin_zero, 0, sizeof(adr.sin_zero));
    
    if (bind(sckt_listen, (struct sockaddr*) &adr, adr_len) < 0)
    {
        printf("Bind nie powiodl sie\n");
        return 1;
    }
    
    listen(sckt_listen, 10);
    
    while(1)
    {
        adr_len = sizeof(struct sockaddr_in);
        sckt_client = accept(sckt_listen, (struct sockaddr*) &adr, &adr_len);
        if (sckt_client < 0)
        {
            printf("Accept zwrocil blad\n");
            continue;
        }
        printf("Polaczenie od %s:%u\n\n", 
            inet_ntoa(adr.sin_addr),
            ntohs(adr.sin_port)
            );
        if (fork() == 0)
        {
            while(1){
            memset(command, 0, 10);
            if (recv(sckt_client, command, 10, 0) <= 0)
            {
                exit(1);
            }

            else {
                if(strcmp(command, ":exit") == 0){

                    printf("Zamykam polaczenie\n");
                    close(sckt_client);
                    exit(1);
                }
                if(strcmp(command, ":file") == 0){
                    sendFilesList(sckt_client);
                   
                    
                }
                if(strcmp(command, ":download") == 0){
                    handleConnection(sckt_client);
                    }

                 }
            }
        }
        else
        {
            continue;
        }
    }
    return 0;
}
