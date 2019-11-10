#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <jerror.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <time.h>
#include </home/martin/Impartial/code/martin.h>

int read_JPEG_file (char *, unsigned char *, int *);
int jayit(unsigned char *,int, int, char *);
void usage(char *);
void load_image(unsigned char*, int* ,char *);
int get_cam_file_name (char *,int);
int load_filenames(FILE *,char *);

int main(int argc,char *argv[]) 
{ 
    struct Image_Bundle *shared_image;
    struct timespec tim, tim2;

    int *dims,cols,go,max_files,next_file_counter;
    unsigned char *image1,*bigbuff;
    char *next_file;
    char *allfiles;
    FILE *list_file;
    char file_name[200];
    char cached[200];
    int THREE=X_SIZE*Y_SIZE*3;

    tim.tv_sec = SECS;
    tim.tv_nsec = NANOSECS;

    next_file=(char *)malloc(200*sizeof (char));
    dims=(int *)malloc(3*sizeof (int));
    image1=(unsigned char *)malloc(sizeof (char)*7096*7096*3);
    bigbuff=(unsigned char *)malloc(sizeof (char)*THREE*400);
    // ftok to generate unique key 
    key_t key = ftok("shmfile",65); 
  
    // shmget returns an identifier in shmid 
    int shmid = shmget(key,sizeof(struct Image_Bundle),0666|IPC_CREAT); 
  
    // shmat to attach to shared memory 
    shared_image = (struct Image_Bundle *) shmat(shmid,(void*)0,0); 

    // Open the cotrol file and cache the files
    // even newer approach. Cache everything first
    if ((list_file=fopen(argv[1], "r")) == NULL) { usage ("I need a file name"); }
    max_files=0;
    while (fscanf(list_file,"%s",next_file) != EOF)
    {
    	printf ("Got %s \n",next_file);
        read_JPEG_file (next_file,image1,dims);
    	memcpy(image1,image1+(dims[0]*3),dims[0]*3);
	load_image(image1,dims,bigbuff+(THREE*max_files));
	max_files++;
    }
    fclose(list_file);
    printf ("Found %d files \n",max_files);
    memcpy(shared_image->drone,bigbuff,THREE);

    int direction;
    int buff;
    char ctl;
    go=1;
    next_file_counter=0;
    direction=1;
    shared_image->icount=max_files;

    while ( go == 1 )
    {
      // Process any controls.
      ctl=shared_image->control;
      switch(ctl) {
      case 'q' :
         go=0;
	 break;
      default :
	 break; }


		// Which buffer?
		//  1 then I do this.
		buff=2;
		if ( shared_image->buffer[1]=='s' ) { buff=1; }
		else if ( shared_image->buffer[0]=='s' ) { buff=0; }
		if (buff != 2)
		{
		        memcpy(shared_image->p+(buff*THREE),bigbuff+(THREE*next_file_counter),THREE);
			printf ("putting frame %d into buff %d\n",next_file_counter,buff);
			shared_image->buffer[buff]='l';
			if (shared_image->maint != 1)
			{
				int span;
				span=shared_image->imax-shared_image->imin;
       				next_file_counter=next_file_counter+(shared_image->slipper*(rand()%span));
       				if (next_file_counter>=shared_image->imax){ next_file_counter=next_file_counter-span;}
		  		next_file_counter+=direction;
      				if (next_file_counter>=shared_image->imax){next_file_counter=shared_image->imax-2;direction=-1;}
      				if (next_file_counter<shared_image->imin){next_file_counter=shared_image->imin+1;direction=1;}
			}
		}

         // sleep without  burning CPU
         //printf ("Sleeping \n");
         if(nanosleep(&tim , &tim2) < 0 )
          {
              printf("Nano sleep system call failed \n");
          }
     }

    free (dims);
    free (image1);
    free (bigbuff);
    //detach from shared memory
    shmdt(shared_image);
}
//Thee end

int load_filenames(FILE *fp, char *names)
{
	int start;
	start=0;
        while (fscanf(fp,"%s",names+(start*100)) != EOF)
        {
                printf ("Got %.50s \n",names+(start*100));
		start++;
        }
	return start;
}


int get_cam_file_name (char *file_name, int cf)
{
    FILE *fp;
    char path[1035];
    int which_file;

     /* Open the command for reading. */
    fp = popen("ls -t /home/martin/Impartial/cam/*.jpg", "r");
    if (fp == NULL) {
       printf("Failed to run command\n" );
      exit(1);
    }
    /* Read the output a line at a time - output it. */
    which_file=0;
    while (fgets(file_name, sizeof(path)-1, fp) != NULL && which_file < cf) {
	which_file++;
        printf("%sxxx\n", file_name);
    }
    /* close */
   pclose(fp);
  file_name[strlen(file_name)-1]='\0'  ;
  return --which_file;
}

void load_image(unsigned char *image1, int *dims , char *p)
{

    	int sspot,image_x,image_y,x_stride,i_stride,x,y,ix,iy,sx,sy,mx,my;
	image_x=dims[0];image_y=dims[1];
	int xfact,yfact,fact;


	xfact=(1024*image_x)/X_SIZE;
	yfact=(1024*image_y)/Y_SIZE;
	fact=xfact;
	if ( yfact>xfact ){ fact=yfact; }
	// We have the biggest factor scales us from screen to image point
	// lets calculte the screen boundries based on the factor
	sx=X_SIZE-((image_x*1024)/fact);
	sy=Y_SIZE-((image_y*1024)/yfact);
	sx=sx/2;sy=sy/2;mx=X_SIZE-sx;my=Y_SIZE-sy;
	//printf ("Recalc X_SIZE %d Y_SZIE %d image_x %d image_y %d sx %d sy %d\n",
	//		X_SIZE,Y_SIZE,image_x,image_y,sx,sy);

        x_stride=image_x*3;
        i_stride=X_SIZE*3;
	sspot=0;
        for (y=0;y<Y_SIZE;y++)
        {
		for (x=0;x<X_SIZE;x++)
                {
                       	int screen_spot;
			int image_spot;
                       	screen_spot=(y*i_stride)+(x*3);
			if (x<sx )
			{
                        	image_spot=(((y*fact)/1024)*x_stride);
				p[screen_spot+2]=image1[image_spot]; 
				p[screen_spot+1]=image1[image_spot+1]; 
				p[screen_spot+0]=image1[image_spot+2];
			} else if (x>=mx) {
                        	image_spot=(((y*fact)/1024)*x_stride)+(x_stride-3);
				p[screen_spot+2]=image1[image_spot]; 
				p[screen_spot+1]=image1[image_spot+1]; 
				p[screen_spot+0]=image1[image_spot+2];
			} else if (y<sy || y>=my) {
                       		image_spot=(((y*fact)/1024)*x_stride);
				p[screen_spot+2]=255; 
				p[screen_spot+1]=0; 
				p[screen_spot+0]=0;
			} else {
                        	//image_spot=((((y-sy)*yfact)/1000)*x_stride)+((((x-sx)*fact)/1000)*3);
                        	image_spot=((((y-sy)*yfact)/1024)*x_stride)+((((x-sx)*fact)/1024)*3);
				p[screen_spot+2]=image1[image_spot]; 
				p[screen_spot+1]=image1[image_spot+1]; 
				p[screen_spot+0]=image1[image_spot+2]; 
			}
               	}
        }
} 

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


void usage (char *error_message)
{
	printf ("usage: display filename %s \n",error_message);
	exit (1);
}
