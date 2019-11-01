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
#define X_SIZE 1024
#define Y_SIZE 768 

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

/* Jpegs */
int read_JPEG_file (char *, unsigned char *, int *);
int jayit(unsigned char *,int, int, char *);


struct cords{
	int xmin;
	int xmax;
	int ymin;
	int ymax;
};

struct letters {
	int count;
	int cords[200];
	size_t dsize;
	char *data;
};

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

int pindown(struct cords *,int *,int);
int square(struct cords *,char *,int,int,int);
int datait(struct cords *,int *,int,struct letter *);

void usage ()
{
	printf("usage: font filename threshold [20-40 ish] startcode [65A 97a]\n");
	exit (1);
}


int main(int argc,char *argv[])
{
        unsigned char *image1;
	char top;
	FILE *d2f;
        unsigned char *dot,*input,*output,*d2file;
        int sc,tm,cordc,tdots,*dims,squares,x_size,y_size,x_stride,*imap,x,y,intens,thresh,yp[20],xp[20],pts,x_scale,y_scale,letc;
	unsigned long total;
	struct cords *c,*d;
	struct letters lets;
	struct letter *car;
	struct font fnt;
	int fix[6]={102,103,106,112,113,121};
        dims=(int *)malloc(3*sizeof (int));
        image1=(unsigned char *)malloc(sizeof (char)*30*4096*4096);
        car=(struct letter *)malloc(sizeof (struct letter) * 256);
        c=(struct cords *)malloc(sizeof (struct cords)*30);
        d=(struct cords *)malloc(sizeof (struct cords)*30);
        imap=(int *)malloc(sizeof (int)*10*4096*4096);
        input=(char *)malloc(sizeof (char)*100);
        output=(char *)malloc(sizeof (char)*100);
        d2file=(char *)malloc(sizeof (char)*100);
	x_image = (XImage *)malloc(sizeof(XImage));
	if ( argc < 4){ usage();}
	tm=atoi(argv[2]);
	sc=atoi(argv[3]);

	init_x();


	input=argv[1];
	read_JPEG_file (input,image1,dims);
	x_size=dims[0];y_size=dims[1];

	x_scale=(1024*X_SIZE)/x_size;
	y_scale=(1024*Y_SIZE)/y_size;

	strcpy(output,"font_");
	strcpy(d2file,input);
	strcpy(fnt.name,input);
	strcat(output,input);
	strcat(d2file,".d2t");
        printf ("Image %s Output %s \n",input,output);
	x_stride=x_size*3;
	total=0;
	// calculate threshold
	for (y=0;y<y_size;y++)
	{
		int p=y*x_size;
		for (x=0;x<x_size;x++)
		{
			int xpoint;
			xpoint=(x*3)+(p*3);
			imap[x+p]=image1[xpoint]+image1[xpoint+1]+image1[xpoint+2];
			total=total+imap[x+p];
		}
	}		

	intens=total/(x_size*y_size);
	thresh=10*total/(tm*x_size*y_size);
	thresh=intens-thresh;
	printf ("Intens %d Thresh %d \n",intens,thresh);

        //Normalise 0-thresh ->255->128 rest 0 
        for (y=0;y<y_size;y++)
        {
                int p=y*x_size;
                int XYP=X_SIZE*4*((y*y_scale)/1024);
                for (x=0;x<x_size;x++)
                {
                        int val;
			int xpoint;
			int X_POINT;
			X_POINT=XYP+4*((x*x_scale)/1024);
			xpoint=(x*3)+(p*3);
			val=imap[x+p];
			if (val < thresh)
			{
				imap[x+p]=64+(((thresh-val)*191)/thresh);
				x_buffer[X_POINT]=0;
				x_buffer[X_POINT+1]=0;
				x_buffer[X_POINT+2]=imap[x+p];
			}else{
			 	imap[x+p]=0 ;
				x_buffer[X_POINT]=255;
				x_buffer[X_POINT+1]=255;
				x_buffer[X_POINT+2]=255;
			}
                }
        }

	c->xmin=10;
	c[1].xmin=10;
	c->xmax=x_size-10;
	c->ymin=10;
	c->ymax=y_size-10;
	squares=pindown(c,imap,x_size);
	printf ("squares %d \n",squares);

	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);


	lets.count=squares;
	cordc=0;
	int start;
	start=0;
	int xmax=0;
	int ymax=0;
	for (pts=1;pts<=squares;pts++)
	{
		int sq;
		d->xmin=c[pts].xmin-1;
		d->xmax=c[pts].xmax+1;
		d->ymin=c[pts].ymin-1;
		d->ymax=c[pts].ymax+1;
		sq=pindown(d,imap,x_size);
		// dot
		if (sq == 2)
		{
			d[1].ymax=d[2].ymax;
		}
		lets.cords[cordc]=1+d[1].xmax-d[1].xmin;
		if (xmax < lets.cords[cordc]){ xmax=lets.cords[cordc];}
		cordc++;
		lets.cords[cordc]=1+d[1].ymax-d[1].ymin;
		if (ymax < lets.cords[cordc]){ ymax=lets.cords[cordc];}
		cordc++;
		lets.cords[cordc]=start;
		datait(d+1,imap,x_size,car+pts+sc-1);
		start=start+(lets.cords[cordc-1]*lets.cords[cordc-2]);
		cordc++;
		square (d+1,x_buffer,x_stride,x_scale,y_scale);
		printf ("PTS %d  xmin %d xmax %d ymin %d ymax %d\n",pts,d[1].xmin,d[1].xmax,d[1].ymin,d[1].ymax);
		// big assumption letters are a-z { | 7x4 grid initially.
	}
	XPutImage(dis, win, gc, x_image, 0, 0, 0, 0, X_SIZE, Y_SIZE);
  	if ((d2f = fopen(d2file, "wb")) == NULL) {
    	fprintf(stderr, "can't open %s\n", d2file);}

	fnt.height=ymax;
	fnt.width=xmax;

	fwrite(&fnt, sizeof (struct font), 1, d2f);
	for (pts=0;pts<256;pts++)
	{
		if (car[pts].width==0){
			car[pts].width=1;car[pts].height=1;car[pts].origin=1;car[pts].bitmap=(char *)malloc(sizeof(char));
	       printf ("Pts %d nulling \n",pts);	}

	int l;
	for (l=0;l<sizeof(fix)/4;l++)
	{ 
		if (pts==fix[l]) 
		{
			printf ("fixing %c\n",(char)fix[l]);
			car[pts].origin=car[pts].origin/2;
		}
	}
	fwrite(&car[pts].width, sizeof (int), 1, d2f);
	fwrite(&car[pts].height, sizeof (int), 1, d2f);
	fwrite(&car[pts].origin, sizeof (int), 1, d2f);
	fwrite(car[pts].bitmap, sizeof (char),car[pts].height*car[pts].width,d2f);
	}

	scanf ("%c",&top);

	fclose (d2f);
        free (dims);
        free (image1);
        free (imap);
	close_x();
}

int square(struct cords *c,char *z,int xstr,int xscale,int yscale)
{
	// top and bottom line
	int x,y;
	int xpoint1,xpoint2;
	for (x=c->xmin;x<=c->xmax;x++)
	{
		xpoint1=(4*X_SIZE*((c->ymin*yscale)/1024))+(4*((x*xscale)/1024));
		xpoint2=(4*X_SIZE*((c->ymax*yscale)/1024))+(4*((x*xscale)/1024));
		z[xpoint1]=255;
		z[xpoint1+1]=0;
		z[xpoint1+2]=0;
		z[xpoint2]=255;
		z[xpoint2+1]=0;
		z[xpoint2+2]=255;

	}
	for (y=c->ymin;y<=c->ymax;y++)
	{
		xpoint1=(4*X_SIZE*((y*yscale)/1024))+(4*((c->xmin*xscale)/1024));
		xpoint2=(4*X_SIZE*((y*yscale)/1024))+(4*((c->xmax*xscale)/1024));
		z[xpoint1]=255;
		z[xpoint1+1]=255;
		z[xpoint1+2]=0;
		z[xpoint2]=0;
		z[xpoint2+1]=255;
		z[xpoint2+2]=255;
	}
};

int datait(struct cords *e,int *imap,int xsize,struct letter *car)
{
	int x,y,sz,pp;
	car->width=1+e->xmax-e->xmin;
	car->height=1+e->ymax-e->ymin;
	car->origin=car->height;
	sz=car->width*car->height;
	car->bitmap=(char *)malloc(sizeof (char)*sz);
	pp=0;

	for (y=e->ymin;y<=e->ymax;y++)
	{
		int p;p=y*xsize;
		for (x=e->xmin;x<=e->xmax;x++)
		{
			car->bitmap[pp]=imap[p+x];	
			pp++;
		}
	}
};


int pindown(struct cords *c,int *a,int xstr)
{
	int trigger,x,y,x1[100],x2[100],y1[100],y2[100];
	int yc,xc;
	trigger=0;
	yc=0;
	// find y points.
	for (y=c->ymin;y<c->ymax;y++)
	{
		int p=y*xstr;
		int found;
		found=0;
		for (x=c->xmin;x<c->xmax;x++)
		{
			int xpoint;
			xpoint=(x*3)+p;
			if (a[x+p]>0 && trigger ==0)
			{
				y1[yc]=y;
				trigger=1;
				found=1;
				x=30000;
			}
			if (a[x+p]>0 && trigger ==1)
			{
				found=1;
				x=30000;
			}
		}
		//not found in this row
		if ( found == 0 && trigger==1) { trigger=0; y2[yc]=y;yc++ ;}
	}
	// find x points.
	xc=0;
	trigger=0;
        for (x=c->xmin;x<c->xmax;x++)
        {
                int found;
                found=0;
        	for (y=c->ymin;y<c->ymax;y++)
                {
                	int p=y*xstr;
                        int xpoint;
                        xpoint=(x*3)+p;
                        if (a[x+p]>0 && trigger ==0)
                        {
                                x1[xc]=x;
				trigger=1;
				found=1;
				y=30000;
                        }
                        if (a[x+p]>0 && trigger ==1)
                        {
                                found=1;
				y=30000;
                        }
                }
                //not found in this row
		if ( found == 0 && trigger==1) { trigger=0; x2[xc]=x; xc++; }
        }

	int p;
	p=1;
	for (y=0;y<yc;y++)
	{
		for (x=0;x<xc;x++)
		{
		c[p].xmin=x1[x];
		c[p].xmax=x2[x];
		c[p].ymin=y1[y];
		c[p].ymax=y2[y];
		p++;
		}
	}
	return --p;
};


struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

GLOBAL(int)
read_JPEG_file (char * filename, unsigned char * dots, int * params)
{
  /* This struct contains the JPEG decompression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   */
  struct jpeg_decompress_struct cinfo;
  /* We use our private extension JPEG error handler.
   * Note that this struct must live as long as the main JPEG parameter
   * struct, to avoid dangling-pointer problems.
   */
  struct my_error_mgr jerr;
  /* More stuff */
  FILE * infile;		/* source file */
  JSAMPARRAY buffer;		/* Output row buffer */
  int row_stride;		/* physical row width in output buffer */

  if ((infile = fopen(filename, "rb")) == NULL) {
    fprintf(stderr, "can't open %s\n", filename);
    return 0;
  }

  /* Step 1: allocate and initialize JPEG decompression object */

  /* We set up the normal JPEG error routines, then override error_exit. */
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = my_error_exit;
  /* Establish the setjmp return context for my_error_exit to use. */
  if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
  }
  /* Now we can initialize the JPEG decompression object. */
  jpeg_create_decompress(&cinfo);

  /* Step 2: specify data source (eg, a file) */

  jpeg_stdio_src(&cinfo, infile);

  /* Step 3: read file parameters with jpeg_read_header() */

  (void) jpeg_read_header(&cinfo, TRUE);
  /* We can ignore the return value from jpeg_read_header since
   *   (a) suspension is not possible with the stdio data source, and
   *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
   * See libjpeg.txt for more info.
   */

  /* Step 5: Start decompressor */

  (void) jpeg_start_decompress(&cinfo);
  /* We can ignore the return value since suspension is not possible
   * with the stdio data source.
   */

  /* We may need to do some setup of our own at this point before reading
   * the data.  After jpeg_start_decompress() we have the correct scaled
   * output image dimensions available, as well as the output colormap
   * if we asked for color quantization.
   * In this example, we need to make an output work buffer of the right size.
   */ 
  /* JSAMPLEs per row in output buffer */
  row_stride = cinfo.output_width * cinfo.output_components;
  /* Make a one-row-high sample array that will go away when done with image */
  buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);


  /* Step 6: while (scan lines remain to be read) */
  /*           jpeg_read_scanlines(...); */

  /* Here we use the library's state variable cinfo.output_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   */

  while (cinfo.output_scanline < cinfo.output_height) {
    /* jpeg_read_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could ask for
     * more than one scanline at a time if that's more convenient.
     */
    (void) jpeg_read_scanlines(&cinfo, buffer, 1);
    memcpy (dots+(row_stride*cinfo.output_scanline),buffer[0],row_stride);
    /* Assume put_scanline_someplace wants a pointer and sample count. */
    /* put_scanline_someplace(buffer[0], row_stride); */

  }
  /* Step 7: Finish decompression */
  params[0]=cinfo.output_width;
  params[1]=cinfo.output_height;
  params[2]=cinfo.output_components;

  (void) jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(infile);

  /* And we're done! */
  return 1;
}

int jayit(unsigned char *screen,int image_width, int image_height, char *name)
{

int row_stride,ex,why,cmp,div,set;
unsigned char *image,**row_pointer,*cr,*cg,*cb;
row_pointer=(unsigned char **)malloc(1);

struct jpeg_compress_struct cinfo;
struct jpeg_error_mgr jerr;
FILE * outfile;		/* target file */
cinfo.err = jpeg_std_error(&jerr);
jpeg_create_compress(&cinfo);
if ((outfile = fopen(name, "wb")) == NULL) { 
	fprintf(stderr, "can't open file\n");
	exit(1);
}
jpeg_stdio_dest(&cinfo, outfile);
cinfo.image_width = image_width; 	/* image width and height, in pixels */
cinfo.image_height = image_height;
cinfo.input_components = 3;		/* # of color components per pixel */
cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */
jpeg_set_defaults(&cinfo);
jpeg_set_quality(&cinfo,100,TRUE); /* limit to baseline-JPEG values */
jpeg_start_compress(&cinfo, TRUE);

  row_stride = image_width * 3;	/* JSAMPLEs per row in image_buffer */

  while (cinfo.next_scanline < cinfo.image_height) {
    /* jpeg_write_scanlines expects an array of pointers to scanlines.
     * Here the array is only one element long, but you could pass
     * more than one scanline at a time if that's more convenient.
     */
    row_pointer[0] = & screen[cinfo.next_scanline * row_stride];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
jpeg_finish_compress(&cinfo);
fclose(outfile);
jpeg_destroy_compress(&cinfo);
}

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
