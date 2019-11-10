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
int drone[3];
int span[3];
int cols[3];

int main(int argc,char *argv[]) 
{ 
    FILE *tf;
    char in[25],pressed,counter[5],loop;
    struct Image_Bundle *shared_image;
    int instance;
    struct timespec tim, tim2;
    pthread_t lfo_id;

    // ftok to generate unique key 
    key_t key = ftok("shmfile",65); 
  
    // shmget returns an identifier in shmid 
    int shmid = shmget(key,sizeof(struct Image_Bundle),0666|IPC_CREAT); 
  
    // shmat to attach to shared memory 
    shared_image = (struct Image_Bundle *) shmat(shmid,(void*)0,0); 
    shared_image->maint=0;
    shared_image->stage=1;
    shared_image->speed=50;
    shared_image->control='h';
    shared_image->slipper=0;
    shared_image->dronep=0;
    shared_image->colour=0;
    instance=0;
    go=1;
    tick=0;
    for (loop=0;loop<INST;loop++)
    {
     shared_image->dmode[loop]='b';
    }

    // wait for images to load
    tim.tv_sec = 1;
    tim.tv_nsec = 0L;
    printf("Waiting for images to load ");
    while (shared_image->icount<=0)
    { 
    	printf(".\n");
    	nanosleep(&tim , &tim2);
    }
    printf("Loaded\n");

    // spin off the LFO
    shared_image->imax=shared_image->icount;
    shared_image->imin=0;
    span[0]=0;
    span[1]=0;
    span[2]=0;

    printf("Starting the lfo\n");
   pthread_create(&lfo_id, NULL, lfo, shared_image);
	

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
      case 'c' :
         printf("colour\n" );
	 if (cols[0]>0){ cols[0]=0; shared_image->colour=0;printf("Colour normal\n");}else{
         printf("min\n");
         scanf(" %d",&cols[1]);
         printf("max\n");
         scanf(" %d",&cols[2]);
         printf("rate seconds per cycle\n");
         scanf(" %d",&cols[0]);}
         break;
      case 'd' :	 
	 if (drone[0]>0)
	 {  shared_image->dronep=0;drone[0]=0;printf("Drone off \n");}else{
         printf("Drone min 0-100\n" ); 
	 scanf(" %d",&drone[1]);
         printf("Drone max 0-100\n" ); 
	 scanf(" %d",&drone[2]);
         printf("Drone rate seconds per cycle\n" ); 
	 scanf(" %d",&drone[0]);}
	 break;
      case 'f' :	 
         printf("Quad frames\n" ); 
	 shared_image->dmode[instance]='f';
	 break;
      case 'i' :	 
         printf("Insert Noise\n" ); 
         printf("Type \n" ); 
	 scanf(" %d",&shared_image->noise);
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
	 if (shared_image->slipper==0){ shared_image->slipper=1; printf("Random..\n");}
	 else {shared_image->slipper=0; printf("Ordered...\n");}
	 break;
      case 'K':
	 //span[0] rate span[1] target span span[2] start span.
	 printf ("Feed Lfo\n");
	 if (span[0]==0){ 
		printf("Spanning ... enter rate\n");
	 	scanf(" %d",&span[0]);
		printf("Enter size\n");
	 	scanf(" %d",&span[1]);
		 span[2]=1;
	 } else {span[0]=0; span[2]=0;printf("Full span...\n");
    		shared_image->imax=shared_image->icount;
    		shared_image->imin=0; }
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
	 if ( shared_image->maint==0 )
	 {shared_image->maint=1;printf("Maint \n");}else
	 { shared_image->maint=0;printf("Free \n");}
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
      case '#' :
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
	int l;
	printf ("Status %c Col %d loadp %d maint %d dronep %d slipper %d\n",ib->control,ib->colour,ib->loadp,ib->maint,ib->dronep,ib->slipper);
	printf ("Images Count %d min %d max %d span rate %d span width %d\n",ib->icount,ib->imin,ib->imax,span[0],span[2]);
	for (l=0;l<INST;l++)
	{
		printf ("Stage %d Status %c \n",l+1,ib->dmode[l]);
	}
}


void *lfo(void *ib)
{
        // This just ticks away in the background and increments things 
        // We sleep for 100th of a sec and adjust.
	// Rate is seconds per cyclce.
           struct timespec tim, tim2;
	   struct Image_Bundle *bu;
           tim.tv_sec = 0;
           tim.tv_nsec = 10000000L;
           bu=(struct Image_Bundle *)ib;
	   int drone_count=0;
	   int drone_direction=1;
	   int cols_count=0;
	   int cols_direction=1;
	   int span_count=0;
	   int span_rate;
	   int grow_span=0;
	   int m;
	   m=6000;
	   // span rate span[0]is the number of minutes for the whole thing.
	   // whole thing is 60*100*span[0] ticks. To take action for all images.

        while (go==1)
        {
                nanosleep(&tim , &tim2);
		// drone processing.
		if(drone[0]>0)
		{
			int delta,dv;
			delta=drone[2]-drone[1];
			drone_count=drone_count+drone_direction;
			dv=(drone_count*delta)/(drone[0]*100);
			if (dv>drone[2]){ drone_direction=-1; dv=drone[2];drone_count=((dv*100*drone[0])/delta)-1;}
			if (dv<drone[1]){ drone_direction=1; dv=drone[1];drone_count=((dv*100*drone[0])/delta);}
			bu->dronep=dv;
		}
		// image selection processing
		if(span[0]>0)
		{
	   		span_rate=m*span[0]/bu->icount;
	   		span_count++;
			if (span_count>span_rate) 
			{
				span_count=0;
				if (span[2]<span[1]){ span[2]++ ;bu->imax=bu->imin+span[2];}
					else{
					if (bu->imax+1<=bu->icount){bu->imax++;}
					if (bu->imin+span[2]<bu->icount && grow_span==0){bu->imin++; }else{grow_span=1;m=2000;}
					}
				if (grow_span==1 && bu->imin>0){ bu->imin--;}	
				//printf ("span 2  %d max %d min %d\n",span[2],bu->imax,bu->imin);
			}
		}else{ grow_span=0;m=6000;}
		// colour cycle
		if (cols[0]>0 )
		{ 
			int delta,cv;
			delta=cols[2]-cols[1];
	   		cols_count+=cols_direction;
			cv=(cols_count*delta)/(cols[0]*100);
			if (cv>cols[2]){ cols_direction=-1; cv=cols[2];cols_count=((cv*100*cols[0])/delta)-1;}
			if (cv<cols[1]){ cols_direction=1; cv=cols[1];cols_count=((cv*100*cols[0])/delta);}
			bu->colour=cv;
		}		
				
        }
        printf ("goodbye from lfo\n");
};

