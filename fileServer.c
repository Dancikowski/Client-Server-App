#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>


#define PORT htons(4448)

void sendFilesList(int gn){
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
            printf("Name is %s\n", names[i]);
            i++;
        }
    }
    closedir(mydir);

    send(gn, &counter, sizeof(int),0 );

    
    for (int i=0; i<counter; i++){
            int len = strlen(names[i]);
            printf("len: %d\n", len);
            send(gn, &len, sizeof(int),0);
            send(gn, names[i], strlen(names[i]),0 );
    }

}

void ObsluzPolaczenie(int gn)
{
    char sciezka[512];
    long long dl_pliku, wyslano, wyslano_razem, przeczytano;
    struct stat fileinfo;
    FILE* plik;
    unsigned char bufor[1024];
    unsigned char myFileSize[1024];
    
    memset(sciezka, 0, 512);
    if (recv(gn, sciezka, 512, 0) <= 0)
    {
        printf("Potomny: blad przy odczycie sciezki\n");
        return;
    }
    
    /*char *sciezka = sciezka +1;*/

    printf("Potomny: klient chce plik %s\n", sciezka);
    
    if (stat(sciezka, &fileinfo) < 0)
    {
        printf("Potomny: nie moge pobrac informacji o pliku\n");
        return;
    }
    
    if (fileinfo.st_size == 0)
    {
        printf("Potomny: rozmiar pliku 0\n");
        return;
    }
    
    printf("Potomny: dlugosc pliku: %d\n", fileinfo.st_size);
    
    dl_pliku = (long) fileinfo.st_size;

    if (send(gn, &dl_pliku, sizeof(long), 0) != sizeof(long))
    {
        printf("Potomny: blad przy wysylaniu wielkosci pliku\n");
        return;
    }
    
    dl_pliku = fileinfo.st_size;
    wyslano_razem = 0;
    plik = fopen(sciezka, "rb");
    if (plik == NULL)
    {
        printf("Potomny: blad przy otwarciu pliku\n");
        return;
    }
    
    while (wyslano_razem < dl_pliku)
    {
        przeczytano = fread(bufor, 1, 1024, plik);
        wyslano = send(gn, bufor, przeczytano, 0);
        if (przeczytano != wyslano){
            break;
        }
        wyslano_razem += wyslano;
        /*printf("Potomny: wyslano %d bajtow\n", wyslano_razem);*/
    }
    
    if (wyslano_razem == dl_pliku){

        printf("Potomny: plik wyslany poprawnie\n");
    }
    else
        printf("Potomny: blad przy wysylaniu pliku\n");
    fclose(plik);

   
    return;    
}


int main(void)
{
    int gn_nasluch, gn_klienta;
    struct sockaddr_in adr;
    socklen_t dladr = sizeof(struct sockaddr_in);
    char command[10];
    gn_nasluch = socket(PF_INET, SOCK_STREAM, 0);
    adr.sin_family = AF_INET;
    adr.sin_port = PORT;
    adr.sin_addr.s_addr = INADDR_ANY;
    memset(adr.sin_zero, 0, sizeof(adr.sin_zero));
    
    if (bind(gn_nasluch, (struct sockaddr*) &adr, dladr) < 0)
    {
        printf("Glowny: bind nie powiodl sie\n");
        return 1;
    }
    
    listen(gn_nasluch, 10);
    
    while(1)
    {
        dladr = sizeof(struct sockaddr_in);
        gn_klienta = accept(gn_nasluch, (struct sockaddr*) &adr, &dladr);
        if (gn_klienta < 0)
        {
            printf("Glowny: accept zwrocil blad\n");
            continue;
        }
        printf("Glowny: polaczenie od %s:%u\n", 
            inet_ntoa(adr.sin_addr),
            ntohs(adr.sin_port)
            );
        printf("Glowny: tworze proces potomny\n");
        if (fork() == 0)
        {
            while(1){
            printf("Potomny: zaczynam obsluge\n");
            
            memset(command, 0, 10);
            if (recv(gn_klienta, command, 10, 0) <= 0)
            {
            printf("Potomny: blad przy odczycie sciezki\n");
            exit(1);
            }

            else {
                if(strcmp(command, ":exit") == 0){

                    printf("Zamykam\n");
                    close(gn_klienta);
                    exit(1);
                }
                if(strcmp(command, ":file") == 0){
                    printf("Wysyłam listę\n");
                    sendFilesList(gn_klienta);
                   
                    
                }
                if(strcmp(command, ":download") == 0){
                    printf("Wysyłam plik\n");
                    ObsluzPolaczenie(gn_klienta);

                }

                else{
                    printf("%s\n", command);
                }

            }
            printf("Potomny: zamykam gniazdo\n");
            printf("Potomny: koncze proces\n");

            }
        }
        else
        {
            /* proces macierzysty */
            printf("Glowny: wracam do nasluchu\n");
            continue;
        }
    }
    return 0;
}
