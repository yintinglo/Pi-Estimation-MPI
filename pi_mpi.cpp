#include "mpi.h"
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <math.h>

# define TOTAL_POINTS 1000000 // amount of tries

int main (int argc,  char *argv[]) {

   int myid, numprocs;
   int namelen;
   char processor_name[MPI_MAX_PROCESSOR_NAME];

   MPI_Init(&argc, &argv);
   MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
   MPI_Comm_rank(MPI_COMM_WORLD, &myid);
   MPI_Get_processor_name(processor_name, &namelen);
 
   printf("Process %d on %s\n", myid, processor_name);
   
   srand((unsigned)(myid) + time(NULL)); // finitialize random seed = use computer time + rank
   
   int s = (int)floor(TOTAL_POINTS / numprocs); //calculates indices for parallel processing
   int s0 = s + TOTAL_POINTS % numprocs;
   
   int startIndex = s0 + (myid - 1) * s;
   int endIndex = startIndex + s; // calculates ending index of the chunk for processes
   
   double startwtime;
   if (myid == 0) {
      startwtime = MPI_Wtime();
   }

   int i;
   int part_circle_points = 0;
   
   double x, y;
   
   if (myid == 0) {
      // master worker - compute the master's numbers
	    
      for (i = 0; i < s0; i++) {
		  x = ((double)rand()) / ((double)RAND_MAX); // generating coordinates
   	   	  y = ((double)rand()) / ((double)RAND_MAX);
		  
		  if (sqrt((x * x) + (y * y)) <= 1) {
			  part_circle_points++;
		  }
		  
      }
      printf("Process %d - startIndex 0 endIndex %d; part_circle_points %ld\n",
             myid, s0-1, part_circle_points);
   } else {
      //slave's work
      for (i= startIndex; i<endIndex; i++) {
		  x = ((double)rand()) / ((double)RAND_MAX); // generating coordinates
   	   	  y = ((double)rand()) / ((double)RAND_MAX);
		  
		  if (sqrt((x * x) + (y * y)) <= 1) {
			  part_circle_points++;
		  }
      }
      printf ("Process %d - startIndex %d endIndex %d; part_circle_points %ld\n",
              myid, startIndex, endIndex-1, part_circle_points);
   }

   int min;
   int circle_points;
   MPI_Reduce(&part_circle_points, &circle_points, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
   
   double ratio, estimate, delta, pii;
   ratio = (double)circle_points / TOTAL_POINTS;
   estimate = ratio * 4;
   pii = M_PI;
   
   if (estimate > pii) {
	  	 delta = estimate - pii;
   }

   if (estimate < pii) {
	     delta = pii - estimate;
   }


   if (myid == 0) {
      double runTime = MPI_Wtime() - startwtime;
      printf("Execution time (sec) = %f estimate = %f delta = %f \n",
             runTime, estimate, delta);
			 	 
   }
   
   MPI_Finalize();
}

