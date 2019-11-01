#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <time.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <math.h>
#include <string.h>

int button(int *,int,int,int,char *,XColor *);
int jayit(int *,int,int,char *);
double dist(int,int,int,int);
int line(int *,int,int,int,int,int,int);
int *pline(int *,int *,int,int,int,int);
int arc(int *,int,int,int,int,int,int,int);
int circ(int *,int *,int,int,int,int,int);
int point(int *,int,int,int);
int gcol();
int bodym(int *,int *,int);
int bodyf(int *,int *,int);
int clear_buffer(int *);
int window_x=1024;
int window_y=768;
int max=1024*768;

/* here are our X variables */
Display *dis;
int screen;
Window win,cin;
GC gc,gd;

/* here are our X routines declared! */
void init_x();
void close_x();
void redraw();
void paint_it(int *,XColor *);

int main(int argc,char *argv[])
{
	int onetwo,checker,*overlay,*window,window_pixels,ecounter,saveit,counter,col,flag,*params,readb;
	time_t t;
	char *directory,*sys1,*sys2,*rstring,*hstring,nums[100];
	int film[500][51];
	FILE* tracker;

	directory=(char *)malloc(sizeof(char)*200);
	sys1=(char *)malloc(sizeof(char)*200);
	sys2=(char *)malloc(sizeof(char)*200);
	params=(int *)malloc(sizeof(int)*50);
	counter=0; ecounter=0; saveit=0; onetwo=0;
	directory="./jpegs/";
	srand((unsigned) time(&t));

	sys1=argv[1];
	if ( sys1 == NULL ){ sys1="default";}
	tracker = fopen(sys1, "r+"); 

	if ( tracker == NULL){ 
		printf ("File %s will not open... creating fresh\n",sys1);
		tracker = fopen(sys1, "w"); 
	}else{
		readb=fread(film,sizeof(int), 25500 , tracker);
		rewind(tracker);
	}

	if (film[0][0]==0)
	{
		film[0][0]=128;film[0][1]=255;film[0][2]=85;film[0][3]=120;
		film[0][25]=383;film[0][26]=255;film[0][27]=70;film[0][28]=100;film[0][50]=400;
		ecounter=0;
	}else{
		for (checker=0;checker<500;checker++)
		{
			if (film[checker][0]==0 && film[checker][1]==0){ ecounter=checker-1;counter=ecounter;break;}
		}
	}

	window=(int *)malloc(sizeof(int)*max); overlay=(int *)malloc(sizeof(int)*max);

	clear_buffer(window); 

	XEvent event;		/* the XEvent declaration !!! */
	KeySym key;		/* a dealie-bob to handle KeyPress Events */	
	char text[255];		/* a char buffer for KeyPress Events */
	char words[]="XposYposWdthHghtBnglNeckEyesSlntSmleBlnkLarmRarmLelbRelbLhndRhndLlegRlegLkneRkneLfotRfot";

	init_x();
	Colormap cmap;
	XColor    color[1000];
	cmap = DefaultColormap(dis, screen);
	color[0].red = 0; color[0].green = 0; color[0].blue = 0;
	color[3].red = 20535; color[3].green = 10000; color[3].blue = 10000;
	color[2].red = 65535; color[2].green = 45535; color[2].blue = 45535;
	color[1].red = 45535; color[1].green = 45535; color[1].blue = 65535;
	color[4].red = 10000; color[4].green = 10000; color[4].blue = 20535;
	color[10].red = 20535; color[10].green = 65535; color[10].blue = 20535;
	color[11].red = 20535; color[11].green = 65535; color[11].blue = 20535;
        col=XAllocColor(dis, cmap, &color[1]); col=XAllocColor(dis, cmap, &color[0]); 
	col=XAllocColor(dis, cmap, &color[2]); col=XAllocColor(dis, cmap, &color[3]); 
	col=XAllocColor(dis, cmap, &color[4]); 
	col=XAllocColor(dis, cmap, &color[10]); col=XAllocColor(dis, cmap, &color[11]); 
	// define button colours
	int bbs;
	for (bbs=100;bbs<200;bbs++)
	{
		if (bbs<140){
			color[bbs].red = 65535; color[bbs].green = 0; color[bbs].blue = 0;
		} else {
			if (bbs < 180)
			{
			color[bbs].red = 0; color[bbs].green = 65535; color[bbs].blue = 0;
			}
			else {
			color[bbs].red = 0; color[bbs].green = 0; color[bbs].blue = 65535;
			}
		}
        	col=XAllocColor(dis, cmap, &color[bbs]); 
	}
	color[185].red = 65535; color[185].green = 25535; color[185].blue = 25535;
       	col=XAllocColor(dis, cmap, &color[185]); 
				char *bit;
				bit=(char *)malloc(sizeof(char)*4);

	XClearWindow(dis, win);
	XClearWindow(dis, cin);
	int x_point,y_point;

	/* look for events forever... */
	while(1) {		
		XNextEvent(dis, &event);
		
		//if (event.type==Expose && event.xexpose.count==0) {
		/* the window was exposed redraw it! */
		//	redraw();
		//}
		if (event.type==KeyPress&&
		    XLookupString(&event.xkey,text,255,&key,0)==1) {
		/* use the XLookupString routine to convert the invent
		   KeyPress data into regular text.  Weird but necessary...
		*/
			if (text[0]=='q') {
				break;
			}
			printf("You pressed the %c key!\n",text[0]);
		}
		if (event.type==ButtonPress ) {
			int val,all,join;
			if ( flag==0) {
				XClearWindow(dis, cin);
				int dd;
				int tl;
				for (dd=0;dd<11;dd++)
				{		
					for (tl=0;tl<4;tl++){ bit[tl]=words[tl+(2*dd*4)];}
					button(overlay,10,10+(dd*20),(2*dd)+100,bit,color);
					button(overlay,70,10+(dd*20),(2*dd)+140,bit,color);
					for (tl=0;tl<4;tl++){ bit[tl]=words[4+tl+(2*dd*4)];}
					button(overlay,140,10+(dd*20),(2*dd)+101,bit,color);
					button(overlay,200,10+(dd*20),(2*dd)+141,bit,color);
				}
				button(overlay,10,230,180,"SNAP",color);
				button(overlay,10,250,181,"BACK",color);
				button(overlay,10,270,182,"FWRD",color);
				button(overlay,10,290,183,"QUIT",color);
				button(overlay,10,310,184,"1OR2",color);
				button(overlay,10,350,187,"LDWN",color);
				button(overlay,10,370,188,"LNUP",color);
				flag=1;
				XClearArea(dis, cin, 100, 230, 200, 20, 0);
				sprintf(nums, "%d of %d", counter,ecounter);
				XSetForeground(dis,gd,color[0].pixel);
				XDrawString(dis,cin,gd,100,250,nums,strlen(nums));
			}
			x_point=event.xbutton.x; y_point=event.xbutton.y;
			val=overlay[x_point+(255*y_point)]-100;
			if (val >=0 && val <=80){ 
				if (val<40) { film[counter][val+(25*onetwo)]-=3 ; }else{ film[counter][val-40+(25*onetwo)]+=3;}
				saveit=1;
			}
			// Here we take the picture.
			if (val == 80){ 
				button(overlay,10,230,0,"SNAP",color);
				for (checker=0;checker<=ecounter;checker++)
				{
				//XClearWindow(dis, win);
				clear_buffer(window);
				for (join=0;join<50;join++){ params[join]=film[checker][join];}
				line (window,0,film[checker][50], 1024, film[checker][50], 2,3);
				bodym(window,params,1);
				bodyf(window,params+25,2);
				sprintf(nums,"%s%s%d.jpg",directory,sys1,checker+100);
				jayit(window,window_x,window_y,nums); 
				}
				button(overlay,10,230,180,"SNAP",color);
			}
			// Go BACK
			if (val == 81 && counter>0){ 
				counter--;
				if (saveit==1)
				{
					rewind(tracker); fwrite(film,sizeof(int), 25500 , tracker);
					saveit=0;
				}
			 }
			// Go FORWARD 
			if (val == 82 && counter >= 0){ counter++; 
				// NEW frame
				if (film[counter][0]==0 && film[counter][1]==0)
				{
					ecounter=counter;
					for (join=0;join<50;join++){ film[counter][join]=params[join];}
					//LINE on new frame
					film[counter][50]=film[counter-1][50];
				}
				if (saveit==1)
				{
					rewind(tracker); fwrite(film,sizeof(int), 25500 , tracker);
					saveit=0;
				}
			}
			// QUIT 
			if (val == 83 ){  break; }
			if (val == 84 ){  
				button(overlay,10,310,185,"1OR2",color);
				onetwo=1;
			}
			if (val == 85 ){  
				button(overlay,10,310,184,"1OR2",color);
				onetwo=0;
			}
			if (val == 87){ film[counter][50]+=4; }
			if (val == 88){ film[counter][50]-=4; }

			XClearArea(dis, cin, 100, 230, 200, 20, 0);
			sprintf(nums, "%d of %d", counter,ecounter);
			XSetForeground(dis,gd,color[0].pixel);
			XDrawString(dis,cin,gd,100,250,nums,strlen(nums));
			XClearWindow(dis, win);
			clear_buffer(window);
			line (window,0,film[counter][50], 1024, film[counter][50], 2,10);
			if (counter-1>=0)
			{
				for (join=0;join<50;join++){ params[join]=film[counter-1][join];}
				bodyf(window,params+25,3);
				bodym(window,params,4);
			}
			for (join=0;join<50;join++){ params[join]=film[counter][join];}
			bodyf(window,params+25,2);
			bodym(window,params,1);
			paint_it(window,color);
		}
	}
	rewind(tracker); fwrite(film,sizeof(int), 25500 , tracker);
	fclose (tracker);
	free (overlay);
	free (window);
	close_x();
}

/* The end */


int jayit(int *screen,int image_width, int image_height, char *name)
{

int row_stride,ex,why,cmp,div,set;
unsigned char *image,**row_pointer,*cr,*cg,*cb;
image=(unsigned char *)malloc(3*image_height*image_width);
cr=(unsigned char *)malloc(3*65536);
cg=(unsigned char *)malloc(3*65536);
cb=(unsigned char *)malloc(3*65536);
row_pointer=(unsigned char **)malloc(1);

/* Colour map */

cr[0]=245;cg[0]=250;cb[0]=40;
cr[0]=255;cg[0]=255;cb[0]=255;
cr[1]=25;cg[1]=25;cb[1]=210;
cr[2]=210;cg[2]=25;cb[2]=25;
cr[3]=25;cg[3]=210;cb[3]=25;
cr[4]=25;cg[4]=210;cb[4]=25;

for (why=0;why<image_height;why++)
{
	for (ex=0;ex<image_width;ex++)
	{
		int point=3*ex+(why*image_width*3);
		image[point]=cr[screen[ex+(why*image_width)]];
		image[point+1]=cg[screen[ex+(why*image_width)]];
		image[point+2]=cb[screen[ex+(why*image_width)]];
	}
}
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
cinfo.input_components = 3;		/* # of color components per pixel */
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
    row_pointer[0] = & image[cinfo.next_scanline * row_stride];
    (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }
jpeg_finish_compress(&cinfo);
fclose(outfile);
jpeg_destroy_compress(&cinfo);
free(image);
}

double dist(int x1,int y1, int x2, int y2)
{
	return sqrt(((x2-x1)*(x2-x1))+((y2-y1)*(y2-y1)));
}


int *pline (int *wind,int *ends, int len, int angle, int w,int col)
{	
	int x2,y2;
	
	x2=ends[0]+len*(sin(M_PI*angle/180));
	y2=ends[1]+len*(cos(M_PI*angle/180));
	line (wind,ends[0],ends[1],x2,y2,w,col);
	ends[0]=x2;ends[1]=y2;
}

int line (int *wind,int x1,int y1, int x2, int y2, int w,int col)
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
			wobbling=10+rand()%15;
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
}

int circ (int *wind, int *cords, int r,int theta,int el,int w,int col)
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
}

int point (int *window, int x, int y, int col)
{
	int loc;
	loc=x+(y*window_x);
	if (x<window_x && x>=0 && y>=0 && y<window_y && col>0)
	{
		window[loc]=col;
	}
}


int clear_buffer (int *buffer)
{
	int p;
	for (p=0;p<max;p++)
	{
		buffer[p]=0;
	}
}

 
void init_x()
{
/* get the colors black and white (see section for details) */        
	unsigned long black,white;

	dis=XOpenDisplay((char *)0);
   	screen=DefaultScreen(dis);
	black=BlackPixel(dis,screen),
	white=WhitePixel(dis,screen);
   	win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,	
		window_x, window_y, 5, white,black);
   	cin=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,	
		255, 512, 5,black,white);
	XSetStandardProperties(dis,win,"Flicker","Flicker",None,NULL,0,NULL);
	XSetStandardProperties(dis,cin,"Control","Control",None,NULL,0,NULL);
	//XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);
	XSelectInput(dis, cin, ExposureMask|ButtonPressMask|KeyPressMask);
        gc=XCreateGC(dis, win, 0,0);        
        gd=XCreateGC(dis, cin, 0,0);        
	XSetBackground(dis,gc,black); XSetForeground(dis,gc,white);
	XSetBackground(dis,gd,black); XSetForeground(dis,gd,white);
	XClearWindow(dis, win);
	XClearWindow(dis, cin);
	XMapRaised(dis, win);
	XMapRaised(dis, cin);
	XMoveWindow(dis, cin,(window_x/2)-10,100);
	XMoveWindow(dis, win,window_x,100);
};

void close_x() {
	XFreeGC(dis, gc);
	XFreeGC(dis, gd);
	XDestroyWindow(dis,win);
	XDestroyWindow(dis,cin);
	XCloseDisplay(dis);	
	exit(1);				
};

void redraw() {
	XClearWindow(dis, win);
};

void paint_it(int *window, XColor *color){
	XClearWindow(dis, win);
	int x,y;
	for (x=0;x<window_x;x++)
	{
		for (y=0;y<window_y;y++)
		{	
			int col;
			col=window[x+y*(window_x)];
			if (col>0){
			XSetForeground(dis,gc,color[col].pixel);
			XDrawPoint(dis, win, gc, x, y);
			}
		}
	}
};

int arc (int *wind,int x1,int y1, int x2, int y2, int b, int w,int col)
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

int bodym (int *window, int *p,int col )
{
	int hangle,angle,*endp,w,h,lleg,rleg,larm,rarm,lelbow,relbow,lhand,rhand,lfoot,rfoot,neck,eyes,slant,mouth,lknee,rknee;
	int *safe,*safe1,*safe2,*safe3;
	safe=(int *)malloc(sizeof(int)*2);
	safe1=(int *)malloc(sizeof(int)*2);
	safe2=(int *)malloc(sizeof(int)*2);
	safe3=(int *)malloc(sizeof(int)*2);
	endp=(int *)malloc(sizeof(int)*2);

	endp[0]=p[0];endp[1]=p[1];
	w=p[2];h=p[3];
	angle=90-p[4];
	neck=p[5];
	eyes=p[6];
	slant=p[7];
	mouth=p[8];
	hangle=p[9];
	larm=p[10];
	rarm=p[11];
	lelbow=p[12];
	relbow=p[13];
	lhand=p[14]*2;
	rhand=p[15]*2;
	lleg=p[16];
	rleg=p[17];
	lknee=p[18];
	rknee=p[19];
	lfoot=p[20];
	rfoot=p[21];

	// centre to top left	
	int zangle=(180*atan((double)w/(double)h))/M_PI;
	int l=(sqrt((w*w)+(h*h)))/2;
	pline(window,endp,l,90+angle+zangle,1,0);
	// left arm
	memcpy(safe, endp,sizeof endp); 
	pline(window,safe,h/2,angle-90-larm,2,col);
	pline(window,safe,h/2,angle-90-larm-lelbow,2,col);
	pline(window,safe,14,angle-90-larm-lelbow+lhand,2,col);
	// to neck
	pline(window,endp,w/2,angle,2,col);
	// neck and head
	memcpy(safe, endp,sizeof endp); 
	pline(window,safe,20,angle+90-neck,2,col);
	pline(window,safe,38,angle+90-neck,2,0);
	memcpy(safe2,safe ,sizeof endp); 
	//head
	circ (window,safe,44,90-angle,85,3,col);
	pline(window,safe,17,angle+90-neck,1,0);
	memcpy(safe1,safe ,sizeof endp); 
	//eyes
	if (14+hangle<33 && 14+hangle>-33)
	{
	pline(window,safe,14+hangle,angle+180-neck,1,0);
	circ (window,safe,7,slant+90+neck-angle,eyes,2,col);
	}
	if (14-hangle>-33 && 14-hangle <33)
	{
	pline(window,safe1,14-hangle,angle-neck,1,0);
	circ (window,safe1,7,90+neck-angle-slant,eyes,2,col);
	}
	//mouth
	pline(window,safe2,16,angle+270-neck,1,0);
	pline(window,safe2,hangle,angle+180-neck,1,0);
	memcpy(safe3,safe2 ,sizeof endp); 
	if (hangle < 35 && hangle > -25)
	{
	pline(window,safe2,15,angle+180-neck-mouth,2,col);
	}
	if (hangle < 25 && hangle > -35)
	{
	pline(window,safe3,15,angle-neck+mouth,2,col);
	}
	// to right arm
	pline(window,endp,w/2,angle,2,col);
	// right arm
	memcpy(safe, endp,sizeof endp); 
	pline(window,safe,h/2,angle-90+rarm,2,col);
	pline(window,safe,h/2,angle-90+rarm+relbow,2,col);
	pline(window,safe,14,angle-90+rarm-rhand+relbow,2,col);
	// right chest
	pline(window,endp,h,angle+270,2,col);
	// right leg
	memcpy(safe, endp,sizeof endp); 
	pline(window,safe,h/2,270+angle+rleg,2,col);
	pline(window,safe,h/2,270+angle+rleg+rknee,2,col);
	pline(window,safe,18,270+angle+rleg+rfoot,2,col);
	// to left leg
	pline(window,endp,w,angle+180,2,col);
	// left leg
	memcpy(safe, endp,sizeof endp); 
	pline(window,safe,h/2,270+angle-lleg,2,col);
	pline(window,safe,h/2,270+angle-lleg-lknee,2,col);
	pline(window,safe,18,270+angle-lleg-lfoot,2,col);
	// left chest
	pline(window,endp,h,angle+90,2,col);
}


int button (int *mark,int x, int y, int col, char *text, XColor *color)
{
	int a,b,w,h;
	w=40;h=18;
	for(a=0;a<w;a++)
	{
		for (b=0;b<h;b++)
		{
			XSetForeground(dis,gd,color[col].pixel);
			XDrawPoint(dis, cin, gd, a+x, b+y);
			mark[a+x+((b+y)*255)]=col;
		}
	}
	XSetForeground(dis,gd,color[0].pixel);
	XDrawString(dis,cin,gd,x+2,y+h-2,text,4);
}

int bodyf (int *window, int *p,int col )
{
	int hangle,angle,*endp,tw,w,h,lleg,rleg,larm,rarm,lelbow,relbow,lhand,rhand,lfoot,rfoot,neck,eyes,slant,mouth,lknee,rknee;
	int *safe,*safe1,*safe2,*safe3;
	safe=(int *)malloc(sizeof(int)*2);
	safe1=(int *)malloc(sizeof(int)*2);
	safe2=(int *)malloc(sizeof(int)*2);
	safe3=(int *)malloc(sizeof(int)*2);
	endp=(int *)malloc(sizeof(int)*2);

	endp[0]=p[0];endp[1]=p[1];
	w=p[2];h=p[3];
	angle=90-p[4];
	neck=p[5];
	eyes=p[6];
	slant=p[7];
	mouth=p[8];
	hangle=p[9];
	larm=p[10];
	rarm=p[11];
	lelbow=p[12];
	relbow=p[13];
	lhand=p[14]*2;
	rhand=p[15]*2;
	lleg=p[16];
	rleg=p[17];
	lknee=p[18];
	rknee=p[19];
	lfoot=p[20];
	rfoot=p[21];

	tw=w/2;

	// centre to top left	
	int zangle=(180*atan((double)tw/(double)h))/M_PI;
	int l=(sqrt((tw*tw)+(h*h)))/2;
	int sl=(sqrt((tw*tw)+(h*h)));
	pline(window,endp,l,90+angle+zangle,1,0);
	// left arm
	memcpy(safe, endp,sizeof endp); 
	pline(window,safe,h/2,angle-90-larm,2,col);
	pline(window,safe,h/2,angle-90-larm-lelbow,2,col);
	pline(window,safe,14,angle-90-larm-lelbow+lhand,2,col);
	// to neck
	pline(window,endp,tw/2,angle,2,col);
	// neck and head
	memcpy(safe, endp,sizeof endp); 
	pline(window,safe,20,angle+90-neck,2,col);
	pline(window,safe,38,angle+90-neck,2,0);
	memcpy(safe2,safe ,sizeof endp); 
	//head
	circ (window,safe,40,90-angle,85,3,col);
	pline(window,safe,17,angle+90-neck,1,0);
	memcpy(safe1,safe ,sizeof endp); 
	//eyes
	if (14+hangle<33 && 14+hangle>-33)
	{
	pline(window,safe,14+hangle,angle+180-neck,1,0);
	circ (window,safe,7,slant+90+neck-angle,eyes,2,col);
	}
	if (14-hangle>-33 && 14-hangle <33)
	{
	pline(window,safe1,14-hangle,angle-neck,1,0);
	circ (window,safe1,7,90+neck-angle-slant,eyes,2,col);
	}
	//mouth
	pline(window,safe2,16,angle+270-neck,1,0);
	pline(window,safe2,hangle,angle+180-neck,1,0);
	memcpy(safe3,safe2 ,sizeof endp); 
	if (hangle < 35 && hangle > -25)
	{
	pline(window,safe2,15,angle+180-neck-mouth,2,col);
	}
	if (hangle < 25 && hangle > -35)
	{
	pline(window,safe3,15,angle-neck+mouth,2,col);
	}
	// to right arm
	pline(window,endp,tw/2,angle,2,col);
	// right arm
	memcpy(safe, endp,sizeof endp); 
	pline(window,safe,h/2,angle-90+rarm,2,col);
	pline(window,safe,h/2,angle-90+rarm+relbow,2,col);
	pline(window,safe,14,angle-90+rarm-rhand+relbow,2,col);
	// right chest
	pline(window,endp,sl,angle+272+(zangle/2),2,col);
	// right leg
	memcpy(safe, endp,sizeof endp); 
	pline(window,safe,h/2,270+angle+rleg,2,col);
	pline(window,safe,h/2,270+angle+rleg+rknee,2,col);
	pline(window,safe,18,270+angle+rleg+rfoot,2,col);
	// to left leg
	pline(window,endp,w,angle+180,2,col);
	// left leg
	memcpy(safe, endp,sizeof endp); 
	pline(window,safe,h/2,270+angle-lleg,2,col);
	pline(window,safe,h/2,270+angle-lleg-lknee,2,col);
	pline(window,safe,18,270+angle-lleg-lfoot,2,col);
	// left chest
	pline(window,endp,sl,angle+88-(zangle/2),2,col);
}
