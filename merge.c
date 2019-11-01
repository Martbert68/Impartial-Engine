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
	struct letter *car,*bar;
	FILE *d2f;
	struct font *fnt,*ent;
	char *input,pause[100];
	int pts,yy;
	int size,scale;
        car=(struct letter *)malloc(sizeof (struct letter) * 256);
        bar=(struct letter *)malloc(sizeof (struct letter) * 256);
        fnt=(struct font *)malloc(sizeof (struct font) * 1);
        ent=(struct font *)malloc(sizeof (struct font) * 1);
        input=(char *)malloc(sizeof (char)*100);
	x_image = (XImage *)malloc(sizeof(XImage));
	if ( argc < 2){ usage();}

	init_x();

	load_font (argv[1], car, fnt);
	load_font (argv[2], bar, ent);
	// Number of pixels.
	size=70;
	yy=show_font ("abcdefghijklmnopqrstuvwxyz", size, car, fnt,0);
	yy=show_font ("ABCDEFGHIJKLMONOPQRSTUVWXYZ", size, bar, ent,size);

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


        if ((d2f = fopen("merged.d2t", "wb")) == NULL) {
        fprintf(stderr, "can't open %s\n", "merged");}


        if (fnt->height > ent->height){fwrite(fnt, sizeof (struct font), 1, d2f);}
			else {fwrite(ent, sizeof (struct font), 1, d2f);}
        for (pts=0;pts<256;pts++)
        {
		if (car[pts].width==1 && bar[pts].width==1)
		{
        		fwrite(&car[pts].width, sizeof (int), 1, d2f);
        		fwrite(&car[pts].height, sizeof (int), 1, d2f);
        		fwrite(&car[pts].origin, sizeof (int), 1, d2f);
        		fwrite(car[pts].bitmap, sizeof (char),car[pts].height*car[pts].width,d2f);
		} else if (car[pts].width > 1) {
        		fwrite(&car[pts].width, sizeof (int), 1, d2f);
        		fwrite(&car[pts].height, sizeof (int), 1, d2f);
        		fwrite(&car[pts].origin, sizeof (int), 1, d2f);
        		fwrite(car[pts].bitmap, sizeof (char),car[pts].height*car[pts].width,d2f);
		} else {
        		fwrite(&bar[pts].width, sizeof (int), 1, d2f);
        		fwrite(&bar[pts].height, sizeof (int), 1, d2f);
        		fwrite(&bar[pts].origin, sizeof (int), 1, d2f);
        		fwrite(bar[pts].bitmap, sizeof (char),bar[pts].height*bar[pts].width,d2f);

		}

	}

	scanf("%c",pause);



/*	for (pts=0;pts<squares;pts++)
	{
	fwrite(&car[pts].width, sizeof (int), 1, d2f);
	fwrite(&car[pts].height, sizeof (int), 1, d2f);
	fwrite(&car[pts].origin, sizeof (int), 1, d2f);
	fwrite(car[pts].bitmap, sizeof (char),car[pts].height*car[pts].width,d2f);
	}
*/
	fclose(d2f);
	close_x();
}

int calc_x(char *line, int scale, struct letter *car, struct font *fnt, int along)
{
	int x=0;
	int cutoff=0;
	while (along<strlen(line))
	{
		int v,w;
        	v=line[along];
        	w=car[v].width;
        	if (w==1) { x=x+((fnt->width*scale)/2048); }else{ x=x+((w*scale)/1024);}
        	// this means we hit the end of the line
        	if (x>= X_SIZE)
        	{
       			// OK we blow out.
                	printf ("Overshot\n");
                	cutoff=along;
                	break;
        	}
        	along++;
        }

        // if we cutoff we need to back up
        if (cutoff>0)
        {
                for (along=cutoff;along>=0;along--)
                {
                        printf ("BAcking up \n");
                        int v,w;
                        v=line[along];
                        w=car[v].width;
                        // back to the last space
                        if (w==1){ cutoff=along-1;break;}
                }
                // no space. just go back one
                if (along<0){cutoff--;}
        }else{ cutoff=along-1;}
	return cutoff;
}

int show_font (char *line, int size, struct letter *car, struct font *fnt,int yo)
{
        int x,y,x_stride,along,xoff,yoff,scale,cutoff,going,start,end;
	going=1;
        x_stride=X_SIZE*4;
	start=0;
        scale=(size*1024)/fnt->height;

	// first how much fits x
	//
	//
	while (going==1)
	{
		// does it fit Y?
        	xoff=0;
		if (yo+size>=Y_SIZE)
		{
			//ran out of y space
			printf ("Blew Y\n");
			going=0;
		}else {
			end=calc_x(line,scale,car,fnt,start);
	        	along=start;
			//leftover space
			if (car[line[along]].width==1){ along++;}
        		while (line[along] != 0 && along<=end)
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
