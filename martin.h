//#define X_SIZE 1920 
//#define Y_SIZE 1080 
#define X_SIZE 1720
#define Y_SIZE 970 
#define SECS 0 
#define NANOSECS 10000000L 
#define INST 5 

struct Image_Bundle {
	int instance;
	char control;
	int camframes;
	int imin;
	int imax;
	int icount;
	int dronep;
	int maint;
	int loadp;
	int noise[10];
	int jux[10];
	int hold;
	int speed;
	int stage;
	int slipper;
	int colour;
	char buffer[2];
	char dmode[10];
	char message[2000];
	int fsize;
	unsigned char p[6*X_SIZE*Y_SIZE];
	unsigned char drone[3*X_SIZE*Y_SIZE];
};

