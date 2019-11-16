#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

struct letters {
	int count;
	int cords[200];
	size_t dsize;
	char *data;
};
struct letters lets;

int main(int argc,char *argv[])
{
	FILE *df;
	

	lets.data=(char *)malloc(200);

  	if ((df = fopen("test.dat", "wb")) == NULL) {
    	fprintf(stderr, "can't open %s\n", "file");}

	lets.count=1;
	lets.dsize=1;
	lets.data[0]='m';


	fwrite(&lets.count, sizeof (int), 1, df);
	fwrite(&lets.cords, sizeof (int), 200, df);
	fwrite(&lets.dsize, sizeof (size_t), 1, df);
	fwrite(lets.data, sizeof (char), lets.dsize,df);
        free (lets.data);

	fclose (df);


  	if ((df = fopen("basic.jpg.dat", "rb")) == NULL) {
    	fprintf(stderr, "can't open %s\n", "file");}
	fread(&lets.count, sizeof (int), 1, df);
	fread(&lets.cords, sizeof (int), 200, df);
	fread(&lets.dsize, sizeof (size_t), 1, df);
	lets.data=(char *)malloc(lets.dsize);
	lets.data[0]='n';
	fread(lets.data, sizeof (char), lets.dsize,df);
	int loop;
	for (loop=0;loop<lets.dsize;loop++)
	{
	printf ("DAT %c \n",lets.data[loop]);
	}

}

