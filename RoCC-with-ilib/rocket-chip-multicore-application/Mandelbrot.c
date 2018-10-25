#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "mandelbrot.h"

typedef struct complextype
{
	double real, imag;
} Compl;

int mandelbrotCal(Compl z, Compl c);
//int color_buf[100][100];
void thread_entry(int rank, int task_n)
{
	int width = 100;
	int height = 100;
	int color_buf[width][height];
	int grid_size = 10;
	int gx, gy;
	int grid_width = (width % grid_size) ? width/grid_size + 1 : width/grid_size;
	int grid_height = (height % grid_size) ? height/grid_size + 1 : height/grid_size;
	Compl z, c;
	int repeats;
	int i, j;
	barrier(task_n);
	for(i = 0; i < width; i++) {
		for(j = 0; j < height; j++) {
			gx = i/grid_size;
			gy = j/grid_size;
			if((gx*grid_width+gy) % task_n == rank){
				z.real = 0.0;
				z.imag = 0.0;
				c.real = ((double)i - 50.0)/25.0; 
				c.imag = ((double)j - 50.0)/25.0; 
				repeats = mandelbrotCal(z, c);
				if(rank != 0){
					int send_buf[3];
					send_buf[0] = i;
					send_buf[1] = j;
					send_buf[2] = repeats;
					ilib_send(0, &send_buf, 3*sizeof(int));
				}
				else {
					color_buf[i][j]=repeats;
				}
				//color_buf[i][j]=repeats;
			}
			else if(rank == 0){
				int recv_buf[3];
				ilib_recv(&recv_buf, 3*sizeof(int));
				color_buf[recv_buf[0]][recv_buf[1]]=recv_buf[2];
			}
		}
	}
	printf("\n");
	barrier(task_n);
	if(rank==0){
		for(i=0; i<height; i++){
			for(j=0; j<width; j++){
				printf("%d", color_buf[j][i]);
			}
			printf("\n");
		}
	}
	barrier(task_n);
	exit(0);
}

int mandelbrotCal(Compl z, Compl c){
	int repeats = 0;
	int result;
	double lengthsq = 0.0;
	double temp;

	while(repeats < 9 && lengthsq < 4.0) { 
		temp = z.real*z.real - z.imag*z.imag + c.real;
		z.imag = 2*z.real*z.imag + c.imag;
		z.real = temp;
		mandelbrot(result, (int)(z.real*100), (int)(z.imag*100));
		lengthsq = (double)result/10000;
		//lengthsq = z.real*z.real + z.imag*z.imag;
		repeats++;
	}
	return repeats;
}
