#include <pthread.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include </home/martin/Impartial/code/martin.h>

int go;
int tick;
void status( struct Image_Bundle *, int );
void *lfo(void *);
int rates[10];

int main(int argc,char *argv[]) 
{ 
    FILE *tf;
    char in[25],pressed,counter[5],loop;
    struct Image_Bundle *shared_image;
    int *col,instance;
    pthread_t lfo_id;

    // ftok to generate unique key 
    key_t key = ftok("shmfile",65); 
  
    // shmget returns an identifier in shmid 
    int shmid = shmget(key,sizeof(struct Image_Bundle),0666|IPC_CREAT); 
  
    // shmat to attach to shared memory 
    shared_image = (struct Image_Bundle *) shmat(shmid,(void*)0,0); 
    shared_image->stage=1;
    shared_image->speed=50;
    shared_image->control='h';
    shared_image->slipper=0;
    shared_image->dronep=0;
    col=shared_image->colour;
    shared_image->colour[0]=0;
    shared_image->colour[1]=0;
    instance=0;
    go=1;
    tick=0;
    for (loop=0;loop<INST;loop++)
    {
     shared_image->dmode[loop]='b';
    }

    // spin off the LFOs
   pthread_create(&lfo_id, NULL, lfo, shared_image);

               char *line = NULL;  /* forces getline to allocate with malloc */
    size_t len = 0;     /* ignored when line = NULL */
    ssize_t read;


     int cc;
    go=1;
    while ( go == 1 )
    {
	status(shared_image,instance);
        scanf (" %c",in);
	pressed=in[0];
        switch(pressed) {
      case '1' :
         printf("Instance One\n" ); 
	 instance=0;
	 break;
      case '2' :
         printf("Instance Two\n" ); 
	 instance=1;
	 break;
      case '3' :
         printf("Instance Three\n" ); 
	 instance=2;
	 break;
      case '4' :
         printf("Instance Four\n" ); 
	 instance=3;
	 break;
      case '5' :
         printf("Instance Five\n" ); 
	 instance=4;
	 break;
      case '9' :
         printf("Stage cam\n" ); 
         printf("How many frames\n" ); 
	 scanf(" %d",&cc);
         shared_image->pop=shared_image->stage;
         shared_image->stage=9;
	 if (cc>0){ shared_image->camframes=cc; }
	 break;
      case 'a' :	 
         printf("Average display mode\n" ); 
	 shared_image->dmode[instance]='a';
	 break;
      case 'b' :	 
         printf("Basic display mode\n" ); 
	 shared_image->dmode[instance]='b';
	 break;
      case 'd' :	 
         printf("Drone val 0-100\n" ); 
	 scanf(" %d",&shared_image->dronep);
	 break;
      case 'D' :	 
         printf("Drone rate 0-999n" ); 
	 scanf(" %d",&rates[0]);
	 break;
      case 'f' :	 
         printf("Quad frames\n" ); 
	 shared_image->dmode[instance]='f';
	 break;
      case 'i' :	 
         printf("Insert Noise\n" ); 
	 shared_image->dmode[instance]='i';
	 break;
      case 'j' :	 
         printf("Juxtapose\n" ); 
	 shared_image->dmode[instance]='j';
	 break;
      case 's' :	 
         printf("Sscroll n" ); 
	 shared_image->dmode[instance]='s';
	 break;
      case 'u' :	 
         printf("X Split Window  display mode\n" ); 
	 shared_image->dmode[instance]='u';
	 break;
      case 'v' :	 
         printf("Y Split Window  display mode\n" ); 
	 shared_image->dmode[instance]='v';
	 break;
      case 'w' :	 
         printf("X-Y Split Window  display mode\n" ); 
	 shared_image->dmode[instance]='w';
	 break;
      case 'r' :	 
         printf("Screen Rate in 100ths\n" ); 
	 scanf(" %d",&cc);
	 if (cc>0){ shared_image->speed=cc; }
	 break;
      case 'c' :
         printf("colour\n" );
         printf("Value 0-600\n" ); 
	 scanf(" %d",col);
	 // reset the ticker
	 col[2]=0;
	 break;
      case 'C':
         printf("Colo rate 100ths per percent\n" );
	 scanf(" %d",col+1);
	 break;
      case 'g' :
	 printf ("Generate Text Enter?\n");
	 printf ("Size\n");
	 scanf(" %d",&shared_image->fsize);
	 scanf(" %[^\n]s",shared_image->message);
	 shared_image->dmode[instance]='g';
	 break;
      case 'G' :
	 printf ("Generate Images\n");
	 shared_image->dmode[instance]='G';
	 break;
      case 'k' :
	 printf ("Random feed\n");
	 scanf(" %d",&cc);
	 shared_image->slipper=cc;
	 break;
      case 't' :
	 printf ("Triplets\n");
	 shared_image->dmode[instance]='t';
	 break;
      case 'l' :
	 printf("Load...\n");
	 shared_image->buffer[0]='l';
	 shared_image->buffer[1]='s';
	 break;
      case 'm' :
	 printf("Maintain image\n");
	 if ( shared_image->control=='m' )
	 {shared_image->control=' ';printf("Free \n");}else
	 { shared_image->control='m';printf("Maint \n");}
	 break;
      case 'n' :
	 printf ("Not the right way up\n");
	 shared_image->dmode[instance]='n';
	 break;
      case 'x' :
	 printf("Hold image...\n");
	 if (shared_image->hold==0)
	 { shared_image->hold=1;printf("Held..\n");}else
	 { shared_image->hold=0;printf("Released..\n");}
	 break;
      case 'h' :
	 printf("Halt..\n");
	 shared_image->buffer[1]=' ';
	 shared_image->buffer[0]=' ';
	 break;
      case 'q' :
	 printf("Quitting...\n");
	 shared_image->control='q';
	 go=0;
	 break;
      case '@' :
	 printf("Relaoding drone\n");
	 shared_image->control='@';
	 break;
      case 'p' :
	 printf("Drawing mode\n");
	 shared_image->dmode[instance]='p';
	 break;
      case 'z' :
	 break;
      default :
         printf("Invalid control\n" );
	 break; }
    }

    //detach from shared memory  
    shmdt(shared_image); 
} 

void status (struct Image_Bundle *ib, int instance )
{
	printf ("Status Stage %d Status %c Camframes %d Col %d Instance %d Dmode %c\n",ib->stage,ib->control,ib->camframes,ib->colour[0],instance,ib->dmode[instance]);
}


void *lfo(void *ib)
{
        // This just ticks away in the background and increments things 
        // Hundredths per tic.
           struct timespec tim, tim2;
	   struct Image_Bundle *bu;
           tim.tv_sec = 0;
           tim.tv_nsec = 100000000L;
           bu=(struct Image_Bundle *)ib;

        while (go==1)
        {
		// drone iis [0]
                nanosleep(&tim , &tim2);
		if (rates[0] != 0)
		{
		bu->dronep+=rates[0];
		if ( bu->dronep >= 500){ rates[0]=-rates[0];bu->dronep+=rates[0];}
		if ( bu->dronep <=1){ rates[0]=-rates[0];bu->dronep+=rates[0];}
		}
		if (bu->slipper>0){ bu->slipper--;}
        }
        printf ("goodbye from lfo\n");
};

