#include <stdlib.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h> 
#include </home/martin/Impartial/code/martin.h>


/* here are our X variables */
Display *dis;
int screen;
Window win;
GC gc;
XImage *x_image;

/* here are our X routines declared! */
void init_x();
void close_x();
void redraw();
void paint_it(int *,XColor *);
void do_image (unsigned char *);

int main() 
{ 
   struct timespec tim, tim2;
   struct Image_Bundle *my_image;
   int go;

	int onetwo,checker,*overlay,*window,window_pixels,ecounter,saveit,counter,col,flag,*params,readb;


   tim.tv_sec = 1;
   tim.tv_nsec = 200000000L;
    // ftok to generate unique key 
    key_t key = ftok("shmfile",65); 
  
    // shmget returns an identifier in shmid 
    int shmid = shmget(key,sizeof(struct Image_Bundle),0666|IPC_CREAT);

      x_image = (XImage *)malloc(sizeof(XImage));

  
    // shmat to attach to shared memory 
    my_image = (struct Image_Bundle *) shmat(shmid,(void*)0,0);

        XEvent event;           /* the XEvent declaration !!! */
        //KeySym key;             /* a dealie-bob to handle KeyPress Events */
        init_x();
	Colormap cmap;
	int sixmill,smil;
	sixmill=16581375;
	sixmill=262144;
        XColor    color[sixmill];
        cmap = DefaultColormap(dis, screen);
	for (smil=0;smil<sixmill;smil++)
	{
        	color[smil].red = smil/8; 
		color[smil].green = 0 ;
		color[smil].blue = 0;
        	col=XAllocColor(dis, cmap, &color[smil]); 
	} 

        XClearWindow(dis, win);


	go=1;
    while (go == 1) 
{
 	char cntrl;
       cntrl=my_image->control;	
    printf("Data read from memory: %c\n",cntrl); 
       char grade = 'B';

   switch(cntrl) {
      case 'q' :
         go=0;
	 break;
      case 'l' :
         printf("locked\n" );
	 break;
      case 'o' :
         printf("open\n" );
	 my_image->control='z';
	 do_image (my_image->p);
	 my_image->control='w';
	 break;
      case 'w' :
	 printf("Wating...\n");
      default :
         printf("Invalid control\n" );
	 break;
   }

   if(nanosleep(&tim , &tim2) < 0 )   
   {
      printf("Nano sleep system call failed \n");
      return -1;
   }
}
      
    //detach from shared memory  
    shmdt(my_image); 
    
    // destroy the shared memory 
    shmctl(shmid,IPC_RMID,NULL); 

    // close x
    close_x ();
     
    return 0; 
} 


void do_image (unsigned char *c)
{
	XMapWindow(dis, win);
	Visual *visual=DefaultVisual(dis, 0);
	x_image->width=X_SIZE;
	x_image->height=Y_SIZE;
	x_image->xoffset=0;
	x_image->format=ZPixmap;
	x_image->data=c;
	x_image->byte_order=LSBFirst;
	x_image->bitmap_unit=8;
	x_image->bitmap_bit_order=LSBFirst;
	x_image->bitmap_pad=8;
	x_image->depth=1;
	x_image->bytes_per_line=X_SIZE*3;
	x_image->bits_per_pixel=8;
	printf ("here \n");
	//XInitImage(x_image);

	printf ("here \n");

	 x_image=XCreateImage(dis, visual, DefaultDepth(dis,DefaultScreen(dis)), ZPixmap, 0, c, X_SIZE, Y_SIZE, 32, 0);


	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);



	int x,y,pval;
	for (y=0;y<Y_SIZE;y++)
	{
		for (x=0;x<X_SIZE;x++)
		{
			int q=(y*X_SIZE*3)+3*x;
			//pval=(unsigned char)p[q];
			//printf ("doing image R %d\n",pval);
		}
	}
}
/* The end */


 
void init_x()
{
/* get the colors black and white (see section for details) */        
	unsigned long black,white;

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

