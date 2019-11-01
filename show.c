#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <jerror.h>
#include <jpeglib.h>
#include <setjmp.h>
#define X_SIZE 1824
#define Y_SIZE 980 

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
int show_font (char *, int, struct letter *, struct font *,int);
int calc_x(char *, int, struct letter *, struct font *,int);

void usage ()
{
	printf("usage: show filename \n");
	exit (1);
}

int main(int argc,char *argv[])
{
	struct letter *car,*bar,*aar;
	struct font *ant,*bnt,*cnt;
	char *input,pause[100];
	int pts,yy;
	int size,scale;
	int *fres;
        car=(struct letter *)malloc(sizeof (struct letter) * 256);
        bar=(struct letter *)malloc(sizeof (struct letter) * 256);
        aar=(struct letter *)malloc(sizeof (struct letter) * 256);
        ant=(struct font *)malloc(sizeof (struct font) * 1);
        bnt=(struct font *)malloc(sizeof (struct font) * 1);
        cnt=(struct font *)malloc(sizeof (struct font) * 1);
        input=(char *)malloc(sizeof (char)*100);
	x_image = (XImage *)malloc(sizeof(XImage));
	if ( argc < 2){ usage();}

	init_x();
	//input=argv[4];

	load_font (argv[1], aar, ant);
	load_font (argv[2], bar, bnt);
	load_font (argv[3], car, cnt);
	//load_font ("basic.jpg.d2t", bar, ent);
	// Number of pixels.
	size=100;
	yy=show_font (argv[4], size, aar, ant,0);
	yy=show_font (argv[5], size, bar, bnt,yy);
	yy=show_font (argv[6], size, car, cnt,yy);

	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);

	scanf("%c",pause);



/*	for (pts=0;pts<squares;pts++)
	{
	fwrite(&car[pts].width, sizeof (int), 1, d2f);
	fwrite(&car[pts].height, sizeof (int), 1, d2f);
	fwrite(&car[pts].origin, sizeof (int), 1, d2f);
	fwrite(car[pts].bitmap, sizeof (char),car[pts].height*car[pts].width,d2f);
	}
*/

	close_x();
}

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

int show_font (char *line, int size, struct letter *car, struct font *fnt,int so)
{
        int x,y,x_stride,along,xoff,yoff,scale,cutoff,going,start,end,len,yo;
	going=1;
        x_stride=X_SIZE*4;
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
                				for (x=0;x<(car[v].width*scale)/1024;x++)
                				{
							char pp;
							pp=car[v].bitmap[((y*1024)/scale)*car[v].width+((x*1024)/scale)];
							if (pp!=0)
							{
                        				x_buffer[p+((x+xoff)*4)]=pp;
                        				x_buffer[p+((x+xoff)*4)+1]=pp;
                        				x_buffer[p+((x+xoff)*4)+2]=pp;
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
	

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

void init_x()
{
/* get the colors black and white (see section for details) */
        unsigned long black,white;

        x_buffer=(unsigned char *)malloc(sizeof(unsigned char)*4*X_SIZE*Y_SIZE);
        dis=XOpenDisplay((char *)0);
        screen=DefaultScreen(dis);
        black=BlackPixel(dis,screen),
        white=WhitePixel(dis,screen);
        win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,
                X_SIZE, Y_SIZE, 5, white,black);
        XSetStandardProperties(dis,win,"image","images",None,NULL,0,NULL);
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
