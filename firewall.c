/*
*
* @author lucadev23
*
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#define PORT 5000
#define OTT 4

int menu(void);
void initPresentation(void);
void insertIP(void);
void insertIP_Presentation(void);
void daemon_firewall(void);
void controll_connections(int,struct sockaddr_in);
bool controll_blacklist (FILE *, char[]);
int searchCharacters(char []);
bool compare(char[], char[], int);
int showIP(void);
void showIP_Presentation(void);
bool accurateControllAddress(char []);
bool controllClasses(int *);
void messageBadAddress(void);
void write_blacklist(char []);
void deleteIP(void);
void deleteIP_Presentation(void);
bool deleteRow();
void killchild(int);
void terminate_child(int);
void endPresentation();
void initFiles(int);

int main(int argc, char *argv[]){
  int chosen,i;
  pid_t pid;

  pid = fork();
  if(pid==0){ //child
    daemon_firewall();
  }
  else if (pid > 0){ //father
    initFiles(pid);
    do{
      signal(SIGSEGV,killchild);
      signal(SIGTERM,killchild);
      signal(SIGINT,killchild);
      chosen=menu();
      switch(chosen){
        case 1:
          insertIP();
          break;
        case 2:
          deleteIP();
          break;
        case 3:
          showIP_Presentation();
          showIP();
          printf("\nSend any number to return: ");
          scanf("%d",&i);
          break;
      }
    }while(chosen!=0);
    terminate_child(pid);
    //kill(getpid(),SIGKILL);
  }
}

void initFiles(int pid){
  FILE *fp_black,*fp_log;
  fp_black=fopen("black.list","a");
  fclose(fp_black);
  if(fp_black==NULL){
    kill(pid,SIGKILL);
    initPresentation();
    printf("Error with create file black.list, try to run another one\n");
    exit(1);
  }
  fp_log=fopen("logs.log","a");
  fclose(fp_log);
  if(fp_log==NULL){
    kill(pid,SIGKILL);
    initPresentation();
    printf("Error with create file logs.log, try to run another one\n");
    exit(1);
  }
}

void terminate_child(int pid){
  kill(pid,SIGKILL);
  endPresentation();
  printf("                             Kill PID --> %d                                \n\n",pid);
}

void endPresentation(){
  system("clear");
	printf("                                       *                                        \n");
	printf("================================================================================\n");
	printf("                        Thanks to choosing this Firewall                        \n");
	printf("================================================================================\n\n");
  printf("\n");
}

void killchild(int signal){
  int i;
  initPresentation();
  switch(signal){
    case SIGSEGV:
      printf("\t\t     ------!Fatal Error with files!------\n\n");
      printf("\t------!Follow these steps to close correctly this program!------\n\n");
      printf(" ------! 1) Open a new terminal and digit --> ps -aux | grep ./firewall ------\n\n");
      printf(" ------! 2) Copy the higher PID of the list ------\n\n");
      printf(" ------! 3) Digit --> kill -9 'PID' and close the terminal\n\n");
      printf("Digit any number after did it --> ");
      scanf("%d",&i);
      break;
    case SIGTERM:
      printf("\t\t     ------!Fatal Error with firewall!------\n\n");
      printf("\t\t      ------!Socket doesn't work!------\n\n");
      printf("\t\t------!Controll if the port %d is free!------\n\n",PORT);
      printf("\t       ------!Close the terminal and retry later!------\n\n");
      printf("Digit any number --> ");
      scanf("%d",&i);
      break;
    default:
      break;
  }
  endPresentation();
  exit(1);
}

void deleteIP(){
  int r=0,m=0;
  do{
    deleteIP_Presentation();
    printf("0 --> Exit\n\n");
    m=showIP();
    if(m==0){
      printf("\n-----Nothing to delete!-----\n");
      sleep(3);
      return;
    }else if(m==-1){
      return;
    }
    printf("Choose --> ");
    scanf("%d",&r);
    if(r==0){
      return;
    }
  }while (r<1 || r>m);
  if(deleteRow(r)) {
    printf("\n----IP Address Deleted Successfully!----\n");
  }
  else{
    printf("\n----Error with file black.list----\n");
  }
  sleep(2);
}

bool deleteRow(int r){
  /*
    Per eliminare una riga dal file, le opzioni sono due:
      1) Caricare TUTTO il file black.list in memoria (personalmente uno spreco inutile)
      2) Creare un file di "appoggio"
    Ho scelto la seconda, quindi creo il file di appoggio che non esiste,
    ma verrà automaticamente creato della funzione fopen e la chiamo black2.list
    Il procedimento è semplice, ogni riga che leggo dal file black.list , verrà
    immagazzinata dalla stringa row[INET_ADDRSTRLEN] e successivamente scritta
    nel file black2.list , facendo attenzione a non scrivere anche la riga da scartare.
    Una volta finito, elimino il file black.list e rinomino il file black2.list in black.list
    Che palle!
  */
  char row[INET_ADDRSTRLEN];
  FILE *fp_black, *fp_black2;
  int i=0;
  fp_black=fopen("black.list","r");
  fp_black2=fopen("black2.list","w"); //file di appoggio, che non esiste
  if(fp_black==NULL || fp_black2==NULL){
    return false;
  }
  do{
    i++;
		memset(row, '\0', sizeof(row));
    if(i!=r){
		  fgets(row,INET_ADDRSTRLEN,fp_black);
      fputs(row,fp_black2);
    }
    else{
      fgets(row,INET_ADDRSTRLEN,fp_black); //la carico in memoria, senza però scriverlo sul file black2.list
    }
  }while(!feof(fp_black));
  fclose(fp_black);
  fclose(fp_black2);
  system("rm black.list");
  system("mv black2.list black.list");
  return true;
}

void deleteIP_Presentation(){
  system("clear");
	printf("                                       *                                        \n");
	printf("================================================================================\n");
	printf("                       Delete IPv4 Address from Blacklist                       \n");
	printf("================================================================================\n");
  printf("\n");
}

void showIP_Presentation(){
  system("clear");
	printf("                                       *                                        \n");
	printf("================================================================================\n");
	printf("                          Show Blacklist's IPv4 Address                         \n");
	printf("================================================================================\n");
  printf("\n");
}

int showIP(void){
  char row[INET_ADDRSTRLEN];
  FILE *fp_blacklist;
  int i=0;
  fp_blacklist=fopen("black.list","r"); //read
  if(fp_blacklist==NULL){
    printf("No black.list file exist, create it!\n");
    sleep(3);
    return -1; //flag di errore
  }
  do{
    i++;
		memset(row, '\0', sizeof(row));
		fgets(row,INET_ADDRSTRLEN,fp_blacklist);
    printf("%d --> %s\n",i,row);
  }while(!feof(fp_blacklist));
  fclose(fp_blacklist);
  return i-1; //perchè stampa e conta anche la riga EOF che non ci serve sapere
}

bool controll_blacklist (FILE *fp, char addr[]){
	int righe=0;
	int countBlack;
	char row[INET_ADDRSTRLEN];
	char addrBlack[INET_ADDRSTRLEN];
	do{
		memset(row, '\0', sizeof(row));
		fgets(row,INET_ADDRSTRLEN,fp);
		if((searchCharacters(addr)==(countBlack=searchCharacters(row)))){ /* se di lunghezza uguale, potenzialmente sono identici*/
			if(compare(row,addr,countBlack)){
				return false; // <--- trovato
			}
		}
	}while(!feof(fp));
	return true;
}

bool compare(char black[],char addr[], int n){
	int i;
	for(i=0;i<n;i++){
		if(black[i]!=addr[i]){
			return false;
		}
	}
	return true;
}

int searchCharacters(char addr[]){
	int i=0;
	while((addr[i]>47 && addr[i]<58) || addr[i]=='.'){
		i++;
	} /* conta i caratteri da '0' a '9' più il carattere '.' per sapere quanto è lungo l'indirizzo nella riga*/
	return i;
}

void controll_connections(int sockfd, struct sockaddr_in serv_addr){
  char addr[INET_ADDRSTRLEN];
  time_t ticks;
	FILE *fp_log,*fp_blacklist;
  int connfd;
  struct sockaddr_in client_addr;
  int dim=sizeof(client_addr);

  while(1) {
    connfd = accept(sockfd,(struct sockaddr*) &client_addr,(socklen_t *) &dim);
    if (connfd < 0)
         continue; //<--- giusto(?)
    memset(addr, '\0', sizeof(addr));
		inet_ntop(AF_INET,&(client_addr.sin_addr), addr, INET_ADDRSTRLEN);
		ticks = time(NULL);
    fp_blacklist=fopen("black.list","r"); //read
    if(fp_blacklist==NULL){
      kill(getppid(),SIGSEGV); //errore di segmentazione(file black.list inesistente)
      exit(1);
    }
    fp_log=fopen("logs.log", "r");
    if(fp_log==NULL){
      kill(getppid(),SIGSEGV); //errore di segmentazione(file logs.log inesistente,IMPOSSIBILE!), probabile eliminazione da parte di un malintenzionato
    }
    fclose(fp_log);
		fp_log=fopen("logs.log", "a"); //write after the EOF or create the file if it doesn't exist
		if(!controll_blacklist(fp_blacklist,addr)){
			fputs("BLACKLISTED --> ",fp_log);
			fputs("try to connect by ",fp_log);
			fputs(("%s",addr),fp_log);
			fputs(" at ",fp_log);
			fputs(("%.24s\n",ctime(&ticks)),fp_log);
			fclose(fp_blacklist);
			fclose(fp_log);
			close(connfd);
			continue;
		}
    fputs("Connection estabilished with ",fp_log);
    fputs(("%s\t",addr),fp_log);
    fputs(" at ",fp_log);
    fputs(("%.24s\n",ctime(&ticks)),fp_log); // li ho dovuti separare perchè messi insieme creavano problemi
    fclose(fp_log);
    fclose(fp_blacklist);
    close(connfd);
  }
}

void daemon_firewall(void){
  int listenfd;
  struct sockaddr_in serv_addr;
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  if (listenfd < 0){
    kill(getppid(),SIGTERM); //errore creazione socket
    exit(0);
  }
  memset(&serv_addr, '\0', sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(PORT); // porta definita come costante
  if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){ //errore nel collegamento tra socket e indirizzo
    kill(getppid(),SIGTERM);
    exit(0);
  }
	listen(listenfd, 10);
  controll_connections(listenfd, serv_addr);
}

void insertIP(){
  char addr[INET_ADDRSTRLEN]; //INET_ADDRSTRLEN = 16, costante definita nelle librerie
  int i=0, fl=-1;
  bool c=false;
  do{
    memset(addr, '\0', sizeof(addr));
    insertIP_Presentation();
    printf("Insert an Ipv4 Address --> ");
    scanf("%s",addr);
    i=searchCharacters(addr); //controllo della lunghezza in maniera generica
    if(i<7 || i>15){
      messageBadAddress();
      printf("Exit inserting 0 or another number to continue--> ");
      scanf("%d", &fl);
    }
  }while((i<7 || i>15) && fl!=0); // ==0 è il flag di uscita
  if(fl==0)
    return;
  c=accurateControllAddress(addr);
  if(c){
    printf("Address Correct! It has been Blacklisted\n");
    write_blacklist(addr);
    sleep(2);
  }
  else{
    messageBadAddress();
    sleep(4);
    return;
  }
}

void write_blacklist(char addr[]){
  FILE *fp;
  fp=fopen("black.list", "a"); //write after the EOF
  fputs(addr,fp);
  fputs("\n",fp);
  fclose(fp);
}

void messageBadAddress(void){
  printf("Error with address....\n");
  printf("Usage 0-255.0-255.0-255.0-255 address\n");
}

bool accurateControllAddress(char *addr){
  char num[INET_ADDRSTRLEN];
  int p=0,n=0,c=0,i=0,v[OTT]={0};
  memset(num, '\0', sizeof(num));
  for(c=0;c<INET_ADDRSTRLEN;c++){
      if(addr[c]=='.'){ //controllo che i punti dell'indirizzo siano 3
        p++;
      }
  }
  if(p==3){
    p=0;
    for(c=0;c<INET_ADDRSTRLEN;c++){
      if(addr[c]>47 && addr[c]<58){ // controllo che i numeri vadano da 0 a 9
        num[n]=addr[c];
        n++;
      }
      else if(addr[c]=='.' || addr[c]=='\0'){
        if(i<OTT){ //salvo i valori degli ottetti nel vettore, se i è < 4
          v[i]=atoi(num);
        }
        else{ //gli ottetti sono 4 e i numeri vanno da 0 a 255, ora bisogna controllare la classe degli indirizzi accuratamente(?)
          return controllClasses(v); //true se indirizzo corretto, false altrimenti
        }
        if(v[i]<0 || v[i] >255){
          return false;
        }
        n=0;
        memset(num, '\0', sizeof(num));
        i++;
      }
      else { //se non sono ne numeri da 0 a 255 ne i caratteri . e \0
        return false;
      }
      if(n>3){ //valore superiore ai 3 caratteri, quindi sicuramente maggiore di 255
        return false;
      }
    }
    return true;
  }
  else{
    return false;
  }
}

bool controllClasses(int v[]){
  return true; // funzione utilizzata per altri tipi di controlli...
}

void insertIP_Presentation(){
  system("clear");
	printf("                                       *                                        \n");
	printf("================================================================================\n");
	printf("                        Insert IPv4 Address on Blacklist                        \n");
	printf("================================================================================\n");
  printf("\n");
}

int menu(void){
  int chosen;
  do{
    initPresentation();
    printf(" 1 - Insert IPv4 Address on Blacklist\n");
    printf(" 2 - Delete IPv4 Address on Blacklist\n");
    printf(" 3 - Show Blacklist's IPv4 Address\n");
    printf(" 0 - Exit\n\n");
    printf("Choose --> ");
    scanf("%d", &chosen);
  }while(chosen < 0 || chosen > 3);
  return chosen;
}

void initPresentation(void){
  system("clear");
	printf("                                       *                                        \n");
	printf("================================================================================\n");
	printf("                    Starting Test for Firewall and Blacklist                    \n");
	printf("================================================================================\n");
  printf("\n");
}
//scusi prof, non lo faccio più
