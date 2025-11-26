/// 
//  mandel.c
//  Based on example code found here:
//  https://users.cs.fiu.edu/~cpoellab/teaching/cop4610_fall22/project3.html
//
//  Converted to use jpg instead of BMP and other minor changes
//  
///
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include "jpegrw.h"

// local routines
static int iteration_to_color( int i, int max );
static int iterations_at_point( double x, double y, int max );
static void compute_image( imgRawImage *img, double xmin, double xmax,
									double ymin, double ymax, int max, int threads);
static void *thread_compute(void *args);
static void show_help();

typedef struct {
	imgRawImage *img;
	double xmin;
	double xmax;
	double ymin;
	double ymax;
	int max;
	int width;
	int minheight;
	int maxheight;
} ImageArgs;


int main( int argc, char *argv[] )
{
	char c;

	// These are the default configuration values used
	// if no command line arguments are given.
	const char *outfile = "mandel.jpg";
	double xcenter = 0;
	double ycenter = 0;
	double xscale = 4;
	double yscale = 0; // calc later
	int    image_width = 1000;
	int    image_height = 1000;
	int    max = 1000;
	int threads = 1;

	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"x:y:s:W:H:m:t:o:h"))!=-1) {
		switch(c) 
		{
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				xscale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 't':
				int temp = atoi(optarg);
				if(temp >= 1 && temp <= 20) {
					threads = temp;
				}
				break;
			case 'o':
				outfile = optarg;
				break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}

	// Calculate y scale based on x scale (settable) and image sizes in X and Y (settable)
	yscale = xscale / image_width * image_height;

	// Display the configuration of the image.
	printf("mandel: x=%lf y=%lf xscale=%lf yscale=%1f max=%d outfile=%s\n",xcenter,ycenter,xscale,yscale,max,outfile);

	// Create a raw image of the appropriate size.
	imgRawImage* img = initRawImage(image_width,image_height);

	// Fill it with a black
	setImageCOLOR(img,0);

	// Compute the Mandelbrot image
	compute_image(img,xcenter-xscale/2,xcenter+xscale/2,ycenter-yscale/2,ycenter+yscale/2,max, threads);

	// Save the image in the stated file.
	storeJpegImageFile(img,outfile);

	// free the mallocs
	freeRawImage(img);

	return 0;
}

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iter;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void compute_image(imgRawImage* img, double xmin, double xmax, double ymin, double ymax, int max, int threads)
{
	pthread_t thread_list[threads];
	ImageArgs *arg = malloc(sizeof(ImageArgs) * threads);

	int base = img->height / threads;
	int extra = img->height % threads;

	int current = 0;
	for(int i = 0; i < threads; i++) {
		int rows = base + (i < extra ? 1 : 0);

		arg[i].img = img;
    	arg[i].max = max;
    	arg[i].xmin = xmin;
    	arg[i].xmax = xmax;
    	arg[i].ymin = ymin;
    	arg[i].ymax = ymax;
    	arg[i].width = img->width;
    	arg[i].minheight = current;
    	arg[i].maxheight = current + rows;

		pthread_create(&thread_list[i], NULL, thread_compute, &arg[i]);

		current = current + rows;
	}

	for(int i = 0; i < threads; i++) {
		pthread_join(thread_list[i], NULL);
	}

	free(arg);
}

static void *thread_compute(void *args) {
	ImageArgs *arg = (ImageArgs *)args;
	int i,j;

	// For every pixel in the image...

	for(j=arg->minheight;j<arg->maxheight;j++) {

		for(i=0;i<arg->width;i++) {

			// Determine the point in x,y space for that pixel.
			double x = arg->xmin + i*(arg->xmax-arg->xmin)/arg->img->width;
			double y = arg->ymin + j*(arg->ymax-arg->ymin)/arg->img->height;

			// Compute the iterations at that point.
			int iters = iterations_at_point(x,y,arg->max);

			// Set the pixel in the bitmap.
			setPixelCOLOR(arg->img,i,j,iteration_to_color(iters,arg->max));
		}
	}

	return NULL;
}


/*
Convert a iteration number to a color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/
int iteration_to_color( int iters, int max )
{
	int color = 0xFFFFFF*iters/(double)max;
	return color;
}


// Show help message
void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>    The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>  X coordinate of image center point. (default=0)\n");
	printf("-y <coord>  Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>  Scale of the image in Mandlebrot coordinates (X-axis). (default=4)\n");
	printf("-W <pixels> Width of the image in pixels. (default=1000)\n");
	printf("-H <pixels> Height of the image in pixels. (default=1000)\n");
	printf("-o <file>   Set output file. (default=mandel.bmp)\n");
	printf("-h          Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}
