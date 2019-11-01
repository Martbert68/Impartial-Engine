#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include </home/martin/Impartial/code/martin.h>
#include <math.h>
#include <string.h>

/* here are our X variables */
Display *dis;
int screen;
Window win;
GC gc;
XImage *x_image;
unsigned char *x_buffer;

/* here are our X routines declared! */
void init_x();
void close_x();
void redraw();
void paint_it(int *,XColor *);
void do_image (unsigned char *,int,char,int *,int *,int );
void display_buffer (unsigned char *,int,int *,unsigned char *,int);
int line(char *,int,int,int,int,int,int);
int *pline(char *,int *,int,int,int,int);
int arc(char *,int,int,int,int,int,int,int);
int circ(char *,int *,int,int,int,int,int);
int point(char *,int,int,int);
void *ticker();
void triplet(unsigned char *,unsigned char *,unsigned char *,int);
void average(unsigned char *,unsigned char *,unsigned char *,int);
void dletters(unsigned char *,unsigned char *,int,int,char *);
void dimage(unsigned char *,unsigned char *,int);
void noise(unsigned char *,unsigned char *,int);
void scroll(unsigned char *,unsigned char *,int);
void juxta(unsigned char *,unsigned char *,int);
void not_up(unsigned char *,unsigned char *,int);
void x_split(unsigned char *,unsigned char *,int);
int frames(unsigned char *,unsigned char *,unsigned char *,int);
void *filter(void *);
struct font {
        char name[150];
        int height;
        int width;
};

struct letter {
        int width;
        int height;
        int origin;
        char *bitmap;
};
void load_font (char *, struct letter *, struct font *);
int show_font (char *, int, struct letter *, struct font *,int ,char *);
int show_image (struct letter *, struct font *,char *);
int calc_x(char *, int, struct letter *, struct font *,int);

struct letter *car,*bar,*aar;
struct font *ant,*bnt,*cnt;

void usage ()
{
        printf("usage: show filename \n");
        exit (1);
}

int THREE=X_SIZE*Y_SIZE*3;
int XS=(X_SIZE*3);
int XH=(X_SIZE*3)/2;
int tick,ltick;
unsigned char *average_buffer;
int *buffer_state;
int go=1;

int main() 
{ 
   struct timespec tim, tim1, tim2;
   struct Image_Bundle *loaded_image;
   int reload,buff,ob1,ob2,load_thread,recycle,lob;
   FILE *df;

   buffer_state=(int *)malloc(sizeof(int)*((2*INST)+2));
   ob1=(2*(INST-1))+2;
   ob2=(2*(INST-1))+3;

   average_buffer = (unsigned char *)malloc(sizeof(unsigned char)*((2*INST)+2)*THREE);

   pthread_t ticker_id,filter_id;
   pthread_create(&ticker_id, NULL, ticker, NULL);

   tim.tv_sec = SECS;
   tim.tv_nsec = NANOSECS;
   // ftok to generate unique key 
   key_t key = ftok("shmfile",65); 
  
   // shmget returns an identifier in shmid 
   int shmid = shmget(key,sizeof(struct Image_Bundle),0666|IPC_CREAT);

    x_image = (XImage *)malloc(sizeof(XImage));

// load the font
        car=(struct letter *)malloc(sizeof (struct letter) * 256);
        bar=(struct letter *)malloc(sizeof (struct letter) * 256);
        aar=(struct letter *)malloc(sizeof (struct letter) * 256);
        ant=(struct font *)malloc(sizeof (struct font) * 1);
        bnt=(struct font *)malloc(sizeof (struct font) * 1);
        cnt=(struct font *)malloc(sizeof (struct font) * 1);
        load_font ("fonts/robot.d2t", aar, ant);
        load_font ("fonts/gm.d2t", bar, bnt);
        load_font ("fonts/ac.d2t", car, cnt);


    // shmat to attach to shared memory 
    loaded_image = (struct Image_Bundle *) shmat(shmid,(void*)0,0);

    for (load_thread=0;load_thread<INST;load_thread++)
    {
    	loaded_image->instance=load_thread;
    	pthread_create(&filter_id, NULL, filter, loaded_image);
    	while (loaded_image->instance != 10){ printf("wating to load %d\n",load_thread);}
    }

    init_x();
    buff=1;
    lob=0;
    reload=0;
    
    while (go == 1) 
    {
       char cntrl;
       cntrl=loaded_image->control;	
       
        recycle=loaded_image->hold;
       switch(cntrl) {
       case 'q' :
         go=0;
	 break;
       case '@' :
	 // relaod the drone from the output.
	 reload=1;
	 loaded_image->control='^';
	 break;
       default :
	 break; }

if (recycle==0)
{
       	if (buffer_state[lob]==4){buffer_state[lob]=0;}
	 if ( loaded_image->buffer[buff]=='l' ) 
	 {
		// We need to shove to the pipeline
		// shove first into drone
		if (buffer_state[0] == 0)
		{
			memcpy(average_buffer,loaded_image->p+(buff*THREE),THREE);
			buffer_state[0]=2;
	    		loaded_image->buffer[buff]='s';
			///printf ("Loaded input buffer 0 \n");
	    		if (buff==1){ buff=0;}else{buff=1;} 
		} else if ( buffer_state[1] == 0 ){
			memcpy(average_buffer+THREE,loaded_image->p+(buff*THREE),THREE);
			buffer_state[1]=2;
			//printf ("Loaded input buffer 1 \n");
	    		loaded_image->buffer[buff]='s';
	    		if (buff==1){ buff=0;}else{buff=1;} 
		}
	}
}else{
                if (buffer_state[0] == 0)
                {
                        memcpy(average_buffer,average_buffer+(THREE*lob),THREE);
                        buffer_state[0]=2;
                        buffer_state[lob]=0;
                } else if ( buffer_state[1] == 0 ){
                        memcpy(average_buffer+THREE,average_buffer+(THREE*lob),THREE);
                        buffer_state[1]=2;
                        buffer_state[lob]=0;
                }
}

	// any work to do?
	if ( buffer_state[ob1] == 2 && buffer_state[ob1] == 2) { 
		//printf ("CLASH %d\n",lob);
		if (lob==ob1)
		{
	    buffer_state[ob2] = 3;
            //printf ("Displaying buffer %d \n",ob2);
	    display_buffer (average_buffer+(THREE*ob2),loaded_image->speed,loaded_image->colour,loaded_image->drone,loaded_image->dronep);
            if (reload==1){ reload=0; memcpy(loaded_image->drone,average_buffer+(THREE*ob2),THREE);}
	    if (recycle==0) {buffer_state[ob2] = 0;}else{buffer_state[ob2] = 4;}
	    lob=ob2;
	    	}else{
	    buffer_state[ob1] = 3;
	    display_buffer (average_buffer+(THREE*ob1),loaded_image->speed,loaded_image->colour,loaded_image->drone,loaded_image->dronep);
            if (reload==1){ reload=0; memcpy(loaded_image->drone,average_buffer+(THREE*ob2),THREE);}
	    if (recycle==0) {buffer_state[ob1] = 0;}else{buffer_state[ob1] = 4;}
	    lob=ob1;
		}
	}else if ( buffer_state[ob1] == 2  ) {
	    buffer_state[ob1] = 3;
            if (reload==1){ reload=0; memcpy(loaded_image->drone,average_buffer+(THREE*ob2),THREE);}
	    display_buffer (average_buffer+(THREE*ob1),loaded_image->speed,loaded_image->colour,loaded_image->drone,loaded_image->dronep);
	    if (recycle==0) {buffer_state[ob1] = 0;}else{buffer_state[ob1] = 4;}
	    lob=ob1;
	} else if ( buffer_state[ob2] == 2 ){
	    buffer_state[ob2] = 3;
            if (reload==1){ reload=0; memcpy(loaded_image->drone,average_buffer+(THREE*ob2),THREE);}
	    display_buffer (average_buffer+(THREE*ob2),loaded_image->speed,loaded_image->colour,loaded_image->drone,loaded_image->dronep);
	    if (recycle==0) {buffer_state[ob2] = 0;}else{buffer_state[ob2] = 4;}
	    lob=ob2;
	}

       if(nanosleep(&tim , &tim2) < 0 )   
       {
          printf("Nano sleep system call failed \n");
          return -1;
       }
     }
      
    //detach from shared memory  
    shmdt(loaded_image); 
    
    // destroy the shared memory 
    shmctl(shmid,IPC_RMID,NULL); 

    // close x
    free (x_buffer);
    printf ("Freed x_buffer \n");
    free (average_buffer);
    printf ("Freed a_buffer \n");
    printf ("Freed data \n");
    close_x ();
     
    return 0; 
} 
/* The end */


void *filter(void *ib )
{
        // This just ticks away in the background and increments tick
        // Hundredths per tic.
           struct timespec tim, tim2;
	   struct Image_Bundle *bundle;
           tim.tv_sec = 0;
           tim.tv_nsec = 10000000L;
	   bundle=(struct Image_Bundle *)ib;
	   int inst,inbuf1,inbuf2,outbuf1,outbuf2,iplace,oplace,got_i,got_o,entry,hold_out;
	   inst=bundle->instance;
	   bundle->instance=10;
	   char *sideb,last,now;
	   hold_out=0;

   	  sideb = (unsigned char *)malloc(sizeof(unsigned char)*(INST*5*THREE));
	  inbuf1=(2*inst);inbuf2=(2*inst)+1;outbuf1=(2*inst)+2;outbuf2=(2*inst)+3;
	  got_i=0;got_o=0;entry=0;

	//printf ("Instance %d Input1 %d Input2 %d Output1 %d Output2 %d\n",inst,inbuf1,inbuf2,outbuf1,outbuf2);
        while (go==1)
        {
		/* calculate an input buffer */
		// always prefer 1 if it is fresh. MMMH NEEDS FIXING
		// 0 free 1 filling 2 fresh 3 reading 
		if (got_i == 0 )
		{
			if ( buffer_state[inbuf1] == 2  )
			{
				buffer_state[inbuf1]=3;
				iplace=inbuf1;
				got_i=1;
			}else if ( buffer_state[inbuf2] == 2 )
			{
				buffer_state[inbuf2]=3;
				iplace=inbuf2;
				got_i=1;
			}
		}
		if ( got_i == 1 && got_o == 0 )
		{
			if ( buffer_state[outbuf1] == 0 )
			{ 
				buffer_state[outbuf1]=1;
				oplace=outbuf1;
				got_o=1;
			}else if ( buffer_state[outbuf2] == 0 )
			{
				buffer_state[outbuf2]=1;
				oplace=outbuf2;
				got_o=1;
			} 
		}
		//printf ("Inst %d ibs1 %d ibs2 %d obs1 %d obs2 %d \n",inst ,buffer_state[inbuf1],buffer_state[inbuf2],
				//buffer_state[outbuf1],buffer_state[outbuf2])	;
		if ( got_i == 1 && got_o == 1 )
		{
			//printf ("Can Work Instance %d Input1 %d Input2 %d Output1 %d Output2 %d\n",inst,inbuf1,inbuf2,outbuf1,outbuf2);
			// now we chose what to do
			//printf ("Inst %d WOrk %d \n",inst,(int)average_buffer[(THREE*iplace)+30000]);
		        now=bundle->dmode[inst] ;
			if (last==now){entry++;}else{entry=0;}
			if (now != 'f'){hold_out=0;}
		        switch(now) {
			case 'a' :
				average (average_buffer+(THREE*iplace),average_buffer+(THREE*oplace),sideb+(inst*THREE*5),entry);
			break;
      			case 'b' :
				memcpy (average_buffer+(THREE*oplace),average_buffer+(THREE*iplace),THREE); 
         		break;
      			case 'g' :
				dletters(average_buffer+(THREE*iplace),average_buffer+(THREE*oplace),entry,bundle->fsize,bundle->message);
         		break;
      			case 'G' :
				dimage(average_buffer+(THREE*iplace),average_buffer+(THREE*oplace),entry);
         		break;
      			case 'i' :
				noise(average_buffer+(THREE*iplace),average_buffer+(THREE*oplace),entry);
         		break;
      			case 'j' :
				juxta(average_buffer+(THREE*iplace),average_buffer+(THREE*oplace),entry);
         		break;
      			case 'n' :
				not_up(average_buffer+(THREE*iplace),average_buffer+(THREE*oplace),entry);
         		break;
      			case 's' :
				scroll(average_buffer+(THREE*iplace),average_buffer+(THREE*oplace),entry);
         		break;
			case 't' :
				triplet (average_buffer+(THREE*iplace),average_buffer+(THREE*oplace),sideb+(inst*THREE*5),entry);
			break;
      			case 'u' :
				x_split(average_buffer+(THREE*iplace),average_buffer+(THREE*oplace),entry);
         		break;
			case 'f' :
				hold_out=frames(average_buffer+(THREE*iplace),average_buffer+(THREE*oplace),sideb+(inst*THREE*5),entry);
			break;
			default:
			break;
			}
			//memcpy (average_buffer+(THREE*oplace),average_buffer+(THREE*iplace),THREE); 
			buffer_state[oplace]=2-hold_out;
			buffer_state[iplace]=0;
			got_i=0;got_o=hold_out;
			last=now;
		}
                nanosleep(&tim , &tim2);
        }
	printf ("goodbye from %d \n",inst);
};

void not_up(unsigned char *input,unsigned char *output,int ent) 
{
        int y,x;
        for (y=0;y<Y_SIZE;y++)
        {
                int alongi,alongo;
                alongi=y*XS;
                alongo=(Y_SIZE-1-y)*XS;
                for (x=0;x<XS;x+=3)
                {
                        output[alongo+x]=input[alongi+x];
                        output[alongo+x+1]=input[alongi+x+1];
                        output[alongo+x+2]=input[alongi+x+2];
                }
        }

}


void x_split(unsigned char *input,unsigned char *output,int ent)
{
        int y,x;
        for (y=0;y<Y_SIZE;y++)
        {
                int along;
                along=y*XS;
                for (x=0;x<XS/2;x+=3)
                {
                        output[along+x]=input[along+(x*2)];
                        output[along+x+1]=input[along+(x*2)+1];
                        output[along+x+2]=input[along+(x*2)+2];
                        output[along+x+XH]=input[along+(x*2)+3];
                        output[along+x+XH+1]=input[along+(x*2)+4];
                        output[along+x+XH+2]=input[along+(x*2)+5];
                }
        }

}

void noise(unsigned char *input,unsigned char *output,int ent) 
{
	memcpy (output,input,THREE);
	int point,loop;
	for (loop=0;loop<ent*10000;loop++)
	{
		point=rand()%(THREE/3);	
		point=point*3;
		output[point]=output[point]/2;
		output[point+1]=output[point+1]/2;
		output[point+2]=output[point+2]/2;
	}
}

void juxta(unsigned char *input,unsigned char *output,int ent) 
{
        int y,x,Y,X;
	int x1,x2,x3,x4,y1,y2,y3,y4;

	x1=ent*5;x2=X_SIZE-(ent*5);x3=0;x4=X_SIZE;
	y1=0;y2=0;y3=Y_SIZE-1;y4=Y_SIZE-1;

	for (y=0;y<Y_SIZE;y++)
	{	
		Y=((y-y1)*Y_SIZE)/(y3-y1);
		for (x=0;x<X_SIZE;x++)
		{
			X=((x-x1)*X_SIZE*(y3-y))/((x2-x1)*(y3-y1))+((x-x3)*X_SIZE*(y1-y)/((x4-x3)*(y1-y3)));
			//X=((x-x3)*X_SIZE*(1)/((x4-x3)*(1)));
			if (X>=0 && X<X_SIZE && Y>=0 && Y<Y_SIZE )
			{
				int P,p;
				P=(Y*X_SIZE*3)+(X*3);
				p=(y*X_SIZE*3)+(x*3);
				output[p]=input[P];
				output[p+1]=input[P+1];
				output[p+2]=input[P+2];
			}
		}
	}
}

void scroll (unsigned char *input,unsigned char *output,int ent) 
{
	int a;
	a=(10*ent) % X_SIZE;
	a=a*3;
        int y,x;
	for (y=0;y<Y_SIZE;y++)
	{	
		int along;
		along=y*XS;
		for (x=0;x<a;x++)
		{
			output[along+x]=input[along+x+XS-a-3];
		}
		for (x=a;x<=XS;x++)
		{
			along;
			output[along+x]=input[along+x-a];
		}
	}
}

void triplet(unsigned char *input,unsigned char *output,unsigned char * buff,int ent)
{
	int along;
	if (ent == 0)
	{
		// first entry just pass and cache
		memcpy (output,input,THREE);
		memcpy (buff,input,THREE);
	}else if (ent == 1)
	{
		// second entry average the two
		memcpy (buff+THREE,input,THREE);
		for (along=0;along<THREE;along++)
		{
			output[along]=(input[along]+buff[along])/2;
		}
	// third entry onwards.
	}else{
		int b,bnow,blast;
		b=ent&1;
		bnow=(b*THREE);blast=((1-b)*THREE);
		for (along=0;along<THREE;along++)
		{
			output[along]=((input[along]*buff[along+bnow])+(buff[along+blast]*(255-buff[along+bnow])))/255;
		}
		// buff bnow blast and input made 3 now splat input into bnow
		memcpy (buff+bnow,input,THREE);
	}
}

void average(unsigned char *input,unsigned char *output,unsigned char * buff,int ent)
{
        int along;
        if (ent == 0)
        {
                // first entry just pass and cache
                memcpy (output,input,THREE);
                memcpy (buff,input,THREE);
        }else if (ent == 1)
        {
                // second entry average the two
                memcpy (buff+THREE,input,THREE);
                for (along=0;along<THREE;along++)
                {
                        output[along]=(input[along]+buff[along])/2;
                }
        // third entry onwards.average 3
        }else{
                int b,bnow,blast;
                b=ent&1;
                bnow=(b*THREE);blast=((1-b)*THREE);
                for (along=0;along<THREE;along++)
                {
                        output[along]=(input[along]+buff[along+bnow]+buff[along+blast])/3;
                }
                // buff bnow blast and input made 3 now splat input into bnow
                memcpy (buff+bnow,input,THREE);
        }
}


void dletters(unsigned char *input,unsigned char *output,int ent,int size,char *mess)
{
	int yy;
        memcpy (output,input,THREE);
	//yy=show_font ("Martin abcABC", l, aar, ant,0,output);
	yy=show_font (mess, size, car, cnt,0,output);
	//yy=show_font ("Andy abcABC", l, car, cnt,yy,output);
}

void dimage(unsigned char *input,unsigned char *output,int ent)
{
	int yy;
        memcpy (output,input,THREE);
	yy=show_image(aar,ant,output);
}


int frames(unsigned char *input,unsigned char *output,unsigned char * buff,int ent)
{
        int along;
        int f,b,bnow,x,y,bp,XH,YH,BH;
	XH=(X_SIZE*3)/2;
	YH=THREE/2;
	BH=XH+YH;

        b=ent&3;
        bnow=(b*THREE);
        memcpy (buff+bnow,input,THREE);
	if ( b != 3 )
	{
		// shove out the last thing
		//printf ("Inst Shoving %d \n",b);
        	memcpy (output,buff+(4*THREE),THREE);
	} else{
		f=0;
		bp=f*THREE;
       		for (y=0;y<(Y_SIZE/2);y++)
		{
			along=3*y*X_SIZE;
       			for (x=0;x<(XH);x+=3)
			{
                 		output[along+x]=buff[2*(along+x)];
                 		output[along+x+1]=buff[2*(along+x)+1];
                 		output[along+x+2]=buff[2*(along+x)+2];
			}
			/*//blur the top right line
                 	output[along+x]=(buff[2*(along+x)]+buff[THREE+(2*along)])/2;
                 	output[along+x+1]=(buff[2*(along+x+1)]+buff[(THREE+(2*along))+1])/2;
                 	output[along+x+2]=(buff[2*(along+x+2)]+buff[(THREE+(2*along))+2])/2; */
		}
		/*// blur the left bottom line
			along=3*y*X_SIZE;
       			for (x=0;x<(XH-1);x+=3)
			{
                 	output[along+x]=(buff[2*(along+x)]+buff[(2*THREE)+(2*along)+x])/2;
                 	output[along+x+1]=(buff[2*(along+x+1)]+buff[((2*THREE)+(2*along))+x+1])/2;
                 	output[along+x+2]=(buff[2*(along+x+2)]+buff[((2*THREE)+(2*along))+x+2])/2;
			} */

        	f=1;
        	bp=f*THREE;
        	for (y=0;y<Y_SIZE/2;y++)
        	{
                	along=3*y*X_SIZE;
                	for (x=3;x<XH;x+=3)
                	{
                        	output[XH+along+x]=buff[bp+2*(along+x)];
                        	output[XH+along+x+1]=buff[bp+2*(along+x)+1];
                        	output[XH+along+x+2]=buff[bp+2*(along+x)+2];
                	}
        	}
        	f=2;
        	bp=f*THREE;
        	for (y=1;y<Y_SIZE/2;y++)
        	{
                	along=3*y*X_SIZE;
                	for (x=0;x<XH;x+=3)
                	{
                        	output[YH+along+x]=buff[bp+2*(along+x)];
                        	output[YH+along+x+1]=buff[bp+2*(along+x)+1];
                        	output[YH+along+x+2]=buff[bp+2*(along+x)+2];
               		 }	
        	}
        	f=3;
        	bp=f*THREE;
        	for (y=0;y<Y_SIZE/2;y++)
        	{
                	along=3*y*X_SIZE;
                	for (x=0;x<XH;x+=3)
                	{
                        	output[BH+along+x]=buff[bp+2*(along+x)];
                        	output[BH+along+x+1]=buff[bp+2*(along+x)+1];
                        	output[BH+along+x+2]=buff[bp+2*(along+x)+2];
                	}
		// buffer the last thing for the next 3 frames
		}
        	memcpy (buff+(4*THREE),output,THREE);
	}
	if (b<3){ return 1;}else{return 0;}
}



/*
void do_image (unsigned char *loaded,int tdelay,char dmode,int *ring,int *colour,int hold)
{
	 int along,along1,age,cycle,rring,bring,pf[11],tb,x,y,z,XH;
	 rring=*ring;
	 tb=10*THREE;
	 XH=(3*X_SIZE)/2;
	// we keep 10 frames in memory in ab unless held
	if (hold==0)
	{
		for (age=0;age<10;age++)
		{
			int offset;
			offset=rring-age;	
			if (offset < 0) { offset =10+offset; }
			pf[age]=offset*THREE;
		}
		//memcpy (ab+(rring*THREE),loaded,THREE); 
		//memcpy (ab+tb,loaded,THREE); 
	}else{
		// pull it back one
		if (*ring>0){ *ring=*ring-1;}else{*ring=9;}
	}

       switch(dmode) {
       case 'b' :
		display_buffer(loaded,tdelay,colour);
	 break;
	case 'p':
	 	buffer_state[0]=2;
	 	for (z=69;z<70;z+=1)
		{			
	 	for (x=0;x<X_SIZE;x+=30)
		{
		int xoff;
		if (x<X_SIZE/2){xoff=x+z;}else{xoff=x-z;}
		xoff=((((X_SIZE/2)-x)*z)/100)+x;
		line (ab+tb ,xoff,0,x,Y_SIZE,2,155);
		}
	 	for (y=0;y<Y_SIZE;y+=5)
		{
		int yoff;
		yoff=((Y_SIZE-y)*z)/115;
		line (ab+tb,yoff,y,X_SIZE-yoff,y,2,155);
		y=y+(y/40);
		}
                display_buffer(ab+tb,tdelay,colour);
		}
	break;
       case 's' :
                for (along=0;along<THREE;along+=3)
                {
			int intensity,mintensity,t;
			intensity=ab[pf[1]+along]+ab[pf[1]+along+1]+ab[pf[1]+along+2];
			mintensity=ab[pf[0]+along]+ab[pf[0]+along+1]+ab[pf[0]+along+2];
			if ( mintensity==0 ){ mintensity=1;}
			t=(ab[pf[0]+along]*intensity)/mintensity; if (t > 255 ){ t=255;} ab[along+tb]=t;
			t=(ab[pf[0]+along+1]*intensity)/mintensity; if (t > 255 ){ t=255;} ab[along+tb+1]=t;
			t=(ab[pf[0]+along+2]*intensity)/mintensity; if (t > 255 ){ t=255;} ab[along+tb+2]=t;
                }
                display_buffer(ab+tb,tdelay,colour);
	break;	 
       case 'u' :
		printf ("la\n");
                for (y=0;y<Y_SIZE;y++)
                {
                        along=y*X_SIZE*3;
                        for (x=0;x<XH;x+=3)
                        {
                                ab[along+x+pf[0]]=ab[tb+along+2*x];
                                ab[along+x+pf[0]+1]=ab[tb+along+(2*x)+1];
                                ab[along+x+pf[0]+2]=ab[tb+along+(2*x)+2];
                                ab[along+x+pf[0]+XH]=ab[tb+along+(2*x)+3];
                                ab[along+x+pf[0]+XH+1]=ab[tb+along+(2*x)+4];
                                ab[along+x+pf[0]+XH+2]=ab[tb+along+(2*x)+5];
                        }
                }
                display_buffer(ab+pf[0],tdelay,colour); 
                memcpy (ab+tb,ab+pf[0],THREE);
         break;
       case 'v' :
                for (y=0;y<Y_SIZE/2;y++)
                {
                        along=y*XS;
                        along1=(y*2)*XS;
                                for (x=0;x<XS;x++)
                                {
                                        ab[pf[1]+along+x]=ab[along1+tb+x];
                                        ab[pf[1]+along+x+((Y_SIZE/2)*XS)]=ab[along1+tb+XS+x];
                                }
                }
                display_buffer(ab+pf[1],tdelay,colour);
                memcpy (ab+tb,ab+pf[1],THREE);
         break;
       case 'w' :
		for (y=0;y<Y_SIZE/2;y++)
		{
			along=y*XS;
			along1=(y*2)*XS;
				for (x=0;x<XS;x++)
				{
					ab[pf[2]+along+x]=ab[along1+tb+x];
					ab[pf[2]+along+x+((Y_SIZE/2)*XS)]=ab[along1+tb+XS+x];
				}
		}
	       for (y=0;y<Y_SIZE;y++)
                {
                        along=y*XS;
                        for (x=0;x<XH;x+=3)
                        {
                                ab[along+x+tb]=ab[pf[2]+along+2*x];
                                ab[along+x+tb+1]=ab[pf[2]+along+(2*x)+1];
                                ab[along+x+tb+2]=ab[pf[2]+along+(2*x)+2];
                                ab[along+x+tb+XH]=ab[pf[2]+along+(2*x)+3];
                                ab[along+x+tb+XH+1]=ab[pf[2]+along+(2*x)+4];
                                ab[along+x+tb+XH+2]=ab[pf[2]+along+(2*x)+5];
                        }
                }
                display_buffer(ab+tb,tdelay,colour);
	 break;
       // average mode
       case 'a' :
	// Display the new image.
	//display_buffer(loaded,tdelay);
	for (cycle=9;cycle<=10;cycle++)
	{
		for (along=0;along<THREE;along++)
		{
			int frames;
			int point_sum=ab[pf[0]+along];
			for (frames=1;frames<cycle;frames++)
			{
				point_sum=point_sum+ab[pf[frames]+along];
			 }	
			ab[along+tb]=(char)(point_sum/cycle); 
		}			
		display_buffer(ab+tb,tdelay,colour);
	}
        break;
       case 't' :
		for (along=0;along<THREE;along++)
		{
			ab[along+tb]=((ab[pf[0]+along]*ab[pf[2]+along])+(ab[pf[1]+along]*(255-ab[pf[2]+along])))/255;
		}
		display_buffer(ab+tb,tdelay,colour);
	break;
       default :
	 break; }

       // increment ring buffer for next time
	if (*ring<9){ *ring=*ring+1;}else{*ring=0;}
}
*/
void display_buffer (unsigned char *sent,int tdelay,int *col,unsigned char *dr,int dp)
{
         int along,three,phase,aphase,ap;
	 struct timespec tim, tim2;
	 tim.tv_sec = 0;
         tim.tv_nsec = 10000000L;

         three=0;
	 if (*col==0 )
	 {
         	for (along=0;along<X_SIZE*Y_SIZE*4;along+=4)
         	{
                	x_buffer[along]=sent[three++];
                	x_buffer[along+1]=sent[three++];
                	x_buffer[along+2]=sent[three++];
         	}
 	}else if (*col <= 100){
		phase=*col;aphase=100-*col;
         	for (along=0;along<X_SIZE*Y_SIZE*4;along+=4)
         	{
			int avg,rvg,svg;
			avg=(sent[three++]+sent[three++]+sent[three++])/3;
			if (avg > 127 ){ rvg=255; }else{rvg=0;}
			svg=((aphase*rvg)+(phase*avg))/100;
                	x_buffer[along]=svg;
                	x_buffer[along+1]=svg;
                	x_buffer[along+2]=svg;
		}	
       	}else if (*col <= 200){
         	for (along=0;along<X_SIZE*Y_SIZE*4;along+=4)
         	{
			int avg,r,g,b;
			r=sent[three++];
			g=sent[three++];
			b=sent[three++];
			avg=(r+g+b)/3;
			r=(((200-*col)*avg)+((*col-100)*r))/100;
			g=(((200-*col)*avg)+((*col-100)*g))/100;
			b=(((200-*col)*avg)+((*col-100)*b))/100;
                	x_buffer[along]=r;
                	x_buffer[along+1]=g;
                	x_buffer[along+2]=b;
		}
	} else if (*col <=300) {
         	for (along=0;along<X_SIZE*Y_SIZE*4;along+=4)
		{
			int r,g,b,rr,gg,bb;
			rr=0;gg=0;bb=0;
			r=sent[three++];
			g=sent[three++];
			b=sent[three++];
			if (r>127){ rr=255;}
			if (g>127){ gg=255;}
			if (b>127){ bb=255;}
			r=(((300-*col)*r)+((*col-200)*rr))/100;
			g=(((300-*col)*g)+((*col-200)*gg))/100;
			b=(((300-*col)*b)+((*col-200)*bb))/100;
                	x_buffer[along]=r;
                	x_buffer[along+1]=g;
                	x_buffer[along+2]=b;
		}
	} else if (*col <=400) { 
		phase=*col-300;
		aphase=400-*col;
         	for (along=0;along<X_SIZE*Y_SIZE*4;along+=4)
		{
			int r,g,b,rr,gg,bb;
			r=sent[three++];
			g=sent[three++];
			b=sent[three++];
			rr=((aphase*r)+(phase*g))/100;
			gg=((aphase*g)+(phase*b))/100;
			bb=((aphase*b)+(phase*r))/100;
                	x_buffer[along]=rr;
                	x_buffer[along+1]=gg;
                	x_buffer[along+2]=bb;
		}
        } else if (*col <=500) {
		phase=*col-400;aphase=500-*col;
                for (along=0;along<X_SIZE*Y_SIZE*4;along+=4)
                {
			int r,g,b,rr,gg,bb;
                        r=sent[three++];
                        g=sent[three++];
                        b=sent[three++];
                        rr=((aphase*g)+(phase*b))/100;
                        gg=((aphase*b)+(phase*r))/100;
                        bb=((aphase*r)+(phase*g))/100;
                	x_buffer[along]=rr;
                	x_buffer[along+1]=gg;
                	x_buffer[along+2]=bb;
                }
        } else if (*col <=600) {
                phase=*col-500;
                aphase=600-*col;
		printf ("phase %d aphase %d \n",phase,aphase);
                for (along=0;along<X_SIZE*Y_SIZE*4;along+=4)
                {
			int r,g,b,rr,gg,bb;
                        r=sent[three++];
                        g=sent[three++];
                        b=sent[three++];
                        rr=((aphase*b)+(phase*r))/100;
                        gg=((aphase*r)+(phase*g))/100;
                        bb=((aphase*g)+(phase*b))/100;
                	x_buffer[along]=rr;
                	x_buffer[along+1]=gg;
                	x_buffer[along+2]=bb;
                }
        }


	if (*col>0 && col[1] != 0)
	{
		if (col[2]==0)
		{
			col[2]=tick;
		}else{
			int inc;
			inc=(tick-col[2])/col[1];
			if (inc >0 )
			{
				col[0]=col[0]+inc;
				col[2]=tick;
			}
		}
	}
	if (*col==300 ){ col[0]=1;}
	if (*col>=600){ col[0]=301;}

	three=0;
	ap=1000-dp;
        for (along=0;along<X_SIZE*Y_SIZE*4;along+=4)
	{
                	x_buffer[along]=((ap*x_buffer[along])+(dp*dr[three++]))/1000;
                	x_buffer[along+1]=((ap*x_buffer[along+1])+(dp*dr[three++]))/1000;
                	x_buffer[along+2]=((ap*x_buffer[along+2])+(dp*dr[three++]))/1000;
	} 	
        // This dispays the buffer
	//printf ("Before %d\n",tick);
	// All the timing is going here
	if ( tick> ltick+tdelay){ printf ("Overrun %d\n",tick);}
	while ( tick < ltick+tdelay ) {
		nanosleep(&tim , &tim2);
	}
	ltick=tick;		
        XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
}
 
void init_x()
{
/* get the colors black and white (see section for details) */        
	unsigned long black,white;

     	x_buffer=(unsigned char *)malloc(sizeof(unsigned char)*4*X_SIZE*Y_SIZE);
	printf ("la \n");
	dis=XOpenDisplay((char *)0);
   	screen=DefaultScreen(dis);
	black=BlackPixel(dis,screen),
	white=WhitePixel(dis,screen);
   	win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,	
		X_SIZE, Y_SIZE, 5, white,black);
	XSetStandardProperties(dis,win,"image","images",None,NULL,0,NULL);
	//XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);
        gc=XCreateGC(dis, win, 0,0);        
	XSetBackground(dis,gc,black); XSetForeground(dis,gc,white);
	XClearWindow(dis, win);
	XMapRaised(dis, win);
	//XMoveWindow(dis, win,window_x,100);
	Visual *visual=DefaultVisual(dis, 0);
	x_image=XCreateImage(dis, visual, DefaultDepth(dis,DefaultScreen(dis)), ZPixmap, 0, x_buffer, X_SIZE, Y_SIZE, 32, 0);
};

void close_x() {
	XFreeGC(dis, gc);
	XDestroyWindow(dis,win);
	XCloseDisplay(dis);	
	exit(1);				
};

void redraw() {
	XClearWindow(dis, win);
};

void *ticker()
{
	// This just ticks away in the background and increments tick
	// Hundredths per tic.
	   struct timespec tim, tim2;
	   tim.tv_sec = 0;
           tim.tv_nsec = 10000000L;
	while (go==1)
	{
		nanosleep(&tim , &tim2);
		tick++;
	}
	tick=tick+10000;
	printf ("goodbye from ticker\n");
};

double dist(int x1,int y1, int x2, int y2)
{
	return sqrt(((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1)));
}


int *pline (char *wind,int *ends, int len, int angle, int w,int col)
{	
	int x2,y2;
	
	x2=ends[0]+len*(sin(M_PI*angle/180));
	y2=ends[1]+len*(cos(M_PI*angle/180));
	line (wind,ends[0],ends[1],x2,y2,w,col);
	ends[0]=x2;ends[1]=y2;
};

int line (char *wind,int x1,int y1, int x2, int y2, int w,int col)
{
	int mul,xs,ys,xe,ye,wobbling,amp;
	double dx,dy,len;
	len=dist(x1,y1,x2,y2);
	dx=(x2-x1)/len;
	dy=(y2-y1)/len;
	wobbling=0;
	for (mul=0;mul<(int)len;mul++)
	{
		if (wobbling==0)
		{
			if (amp < 0 ){ amp=rand()%3; }else{ amp=-rand()%3;}
			wobbling=35+rand()%25;
			xs=x1+(mul*dx);ys=y1+(mul*dy);
		}
		if (wobbling > 0)
		{
			wobbling --;
			if (wobbling==0)
			{
				xe=x1+(mul*dx);ye=y1+(mul*dy);
				arc(wind,xs,ys,xe,ye,amp,w,col);
			}
		}
	}
	arc(wind,xs,ys,x2,y2,amp,w,col);
};

int circ (char *wind, int *cords, int r,int theta,int el,int w,int col)
{
	int len,over,ex,why;
	double r2,pi,shrink,xt,yt,x2,y2;
	shrink=(double)(2+rand()%10)/100;
	pi=2*M_PI;
	xt=sin((M_PI*(double)theta)/180);
	yt=cos((M_PI*(double)theta)/180);
	for (r2=r;r2<r+w;r2++)
	{
		over=(6*r2*(rand()%10))/100;
		len=pi*r2;
		double a;
		double fi;
		for (a=-over;a<=len+over;a+=0.5)
		{
			fi=pi*a/len;
			x2=(1-(shrink*a)/len)*(r2*cos(fi+M_PI));	
			y2=(1-(shrink*a)/len)*el*(r2*sin(fi+M_PI))/100;	
			ex=(x2*yt)-(y2*xt);
			why=(y2*yt)+(x2*xt);
			point (wind,cords[0]+ex,cords[1]+why,col);
		}
	}
};

int point (char *window, int x, int y, int col)
{
	int loc;
	loc=x*3+(y*XS);
	if (x<X_SIZE && x>=0 && y>=0 && y<Y_SIZE )
	{
		window[loc]=(char)col;
		window[loc+1]=(char)col;
		window[loc+2]=(char)col;
	}
};


int arc (char *wind,int x1,int y1, int x2, int y2, int b, int w,int col)
{
	double theta,fi,div,mul;
	double dx,dy,len;
	len=dist(x1,y1,x2,y2);
	dx=x2-x1;dy=y2-y1;
	theta=(dy/len);
	fi=(dx/len);
	div=(len*len)/(4*b);
	
	for (mul=0;mul<=len;mul+=0.1)
	{
		double rx,ry,bx,by;
		bx=mul;
		by=(mul-(len/2));by=by*by; by=b-(by/div);
		rx=-(theta*by)+(fi*mul);
		ry=(fi*by)+(theta*mul);
		point(wind,x1+rx,y1+ry,col);
		int d;
		for (d=1;d<w;d++)
		{
			point(wind,x1+rx+d,y1+ry,col);
			point(wind,x1+rx-d,y1+ry,col);
			point(wind,x1+rx,y1+ry+d,col);
			point(wind,x1+rx,y1+ry-d,col);
		}
				
	}
};

int calc_x(char *line, int scale, struct letter *car, struct font *fnt, int along)
{
	int x=0;
	int v,w,start;
	int cutoff=0;
	start=along;
	while (along<strlen(line))
	{
        	v=line[along];
        	w=car[v].width;
        	if (w==1) { x=x+((fnt->width*scale)/2048); }else{ x=x+((w*scale)/1024);}
		// newline 
		if (v==61) { printf ("NEwline \n"); along++;break;}
        	// this means we hit the end of the line
        	if (x>= X_SIZE)
        	{
       			// OK we blow out.
                	cutoff=along-1;
                	printf ("Overshot cutoff %d\n",cutoff);
                	break;
        	}
        	along++;
        }

        // if we cutoff we need to back up
	int aa;
        if (cutoff>0)
        {
                for (aa=cutoff;aa>=start;aa--)
                {
                        printf ("%d ",aa);
                        v=line[aa];
                        w=car[v].width;
                        // back to the last space
                        if (w==1){ cutoff=aa-1;break;}
                }
		printf (" cut \n");
                // no space. just go back one
		if ( aa<=start){ printf ("aa lt start \n");cutoff=along-1;}
        }else{ cutoff=along-1;}
	return cutoff;
}

int show_image(struct letter *car, struct font *fnt,char *buff)
{
        int x,y,x_stride,along,yo,yl,xl,scale,v,size,col;
        x_stride=X_SIZE*3;
	v=(rand()%8)+1;
	size=rand()%Y_SIZE;
	xl=rand()%X_SIZE;
	yl=rand()%Y_SIZE;
	yo=car[v].height;
        scale=(size*1024)/yo;
	col=rand()%6;
        for (y=0;y<(yo*scale)/1024;y++)
        {
		int p=x_stride*y;
                int yp=((y)*1024)/scale;
                for (x=0;x<(car[v].width*scale)/1024;x++)
                {
               		 char pp;
                         int pos;
                         pos=p+((x+xl)*3);
                         pp=car[v].bitmap[(yp*car[v].width)+((x*1024)/scale)];
                         if (pp!=0 && pos < THREE-2 && pos > 0)
                         {
				if (col==0){ buff[pos]=pp; buff[pos+1]=0; buff[pos+2]=0;}
				if (col==1){ buff[pos]=0; buff[pos+1]=pp; buff[pos+2]=0;}
				if (col==2){ buff[pos]=0; buff[pos+1]=0; buff[pos+2]=pp;}
				if (col==3){ buff[pos]=pp; buff[pos+1]=pp; buff[pos+2]=pp;}
				if (col==4){ buff[pos]=0; buff[pos+1]=0; buff[pos+2]=0;}
				if (col==5){ buff[pos]=255-buff[pos]; buff[pos+1]=255-buff[pos+1]; buff[pos+2]=255-buff[pos+2];}
                        }
                }
        }
        return 1;
}


int show_font (char *line, int size, struct letter *car, struct font *fnt,int so,char *buff)
{
        int x,y,x_stride,along,xoff,yoff,scale,cutoff,going,start,end,len,yo;
	going=1;
        x_stride=X_SIZE*3;
	start=0;
        scale=(size*1024)/fnt->height;
	len=strlen(line);
	yo=so;

	// first how much fits x
	//
	//
	while (going==1)
	{
		// does it fit Y?
        	xoff=0;
		if (yo+size>=Y_SIZE )
		{
			//ran out of y space
			printf ("Blew Y start %d len %d yo %d\n",start,len,yo+size);
			going=0;
		} else if (start >= len) {
			printf ("Blew String %d len %d \n",start,len);
			going=0;
			start=0;
		}else {
			end=calc_x(line,scale,car,fnt,start);
			printf ("START %d END %d /n",start,end);
	        	along=start;
			//leftover space
			if (car[line[along]].width==1){ along++;}
        		while (along<=end)
        		{
	        		int v;
        			v=line[along];
        			if (car[v].width==1)
        			{
                			xoff=xoff+((fnt->width*scale)/2048);
        			} else {
        				yoff=((fnt->height-car[v].origin)*scale)/1024;
        				for (y=0;y<(car[v].height*scale)/1024;y++)
        				{
                				int p=x_stride*(y+yoff+yo);
						int yp=(y*1024)/scale;
                				for (x=0;x<(car[v].width*scale)/1024;x++)
                				{
							char pp;
							int pos;
							pos=p+((x+xoff)*3);
							pp=car[v].bitmap[(yp*car[v].width)+((x*1024)/scale)];
							if (pp!=0 && pos < THREE)
							{
                        				buff[pos]=255-buff[pos];
                        				buff[pos+1]=255-buff[pos+1];
                        				buff[pos+2]=255-buff[pos+2];
							}
                				}	
        				}
        				xoff=xoff+x;
        			}
        			along++;
			}
			start=end+1;
			yo=yo+size;
		}
	}
	return yo+y;
	//return start;
}

void load_font (char *input, struct letter *car, struct font *fnt)
{
	int pts;
	FILE *d2f;
        if ((d2f = fopen(input, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", input);}

        fread(fnt, sizeof (struct font), 1, d2f);
        printf("Font name %s Height %d Width %d \n",fnt->name,fnt->height,fnt->width);
        for (pts=0;pts<256;pts++)
        {
                int c_size,read;
                read=fread(&car[pts].width, sizeof (int), 1, d2f);
                printf ("read %d \n",read);
                read=fread(&car[pts].height, sizeof (int), 1, d2f);
                printf ("read %d \n",read);
                read=fread(&car[pts].origin, sizeof (int), 1, d2f);
                printf ("read %d \n",read);
                c_size=car[pts].width*car[pts].height;
                car[pts].bitmap=(char *)malloc(sizeof (char)*c_size);
                read=fread(car[pts].bitmap, sizeof (char),c_size,d2f);
                printf ("read %d \n",read);
                printf("PTS %d SIZE %d Width %d Height %d\n",pts,c_size,car[pts].width,car[pts].height);
        }
	fclose (d2f);
}	
	
