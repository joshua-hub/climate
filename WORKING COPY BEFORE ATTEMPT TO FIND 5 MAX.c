// phys3071 ws14/15 melsom 42593249

/*
DESCRIPTION: 

INPUTS:

CALCULATIONS:

OUTPUT: 
*/

// Import libraries ***********************************************************
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// Global varibables **********************************************************
// I wish!

// Function prototypes ********************************************************
double* multiply(double *x, double *y, int size);
double* inverse(double *x, int size);
double sum(double *x, int start_year, int end_year);

// Begin main program *********************************************************
int main () {

  char fname[100];

  FILE *dat;
  FILE *out;
  
  int num_lines, count, dummy_var_int;

  int min_year, max_year, start_year, end_year, *linear_date_int, *year,*month;
  int start_year_index, end_year_index, max_index, min_index;
  
  double *temp_anom, *temp_mean_err, *INV_temp_mean_err,*INV_temp_mean_err_SQU;
  double *xx, *xy, *y_ax_b;
  double *xy_on_e_SQU, *x_on_e_SQU, *y_on_e_SQU, *xx_on_e_SQU, *y_ax_b_e;
  double SUM_INV_temp, SUM_xx_on_e, SUM_xy_on_e, SUM_x_on_e, SUM_y_on_e;
  double slope, intercept, Error;
  double dummy_var;
  double *linear_date;
  
  //double *max_temp= malloc(5* sizeof(double));
  // int count2;
  // int *max_temp_index= malloc(5* sizeof(int));
  
  // PLACEHOLDERS WHILST THE NEXT BLOCK IS EDITED OUT.
  dat= fopen("monthly_climate(1).dat", "r");
  
// THIS SECTION COMMENTED OUT TO STOP ASKING QUESTIONS EVERY TIME I RUN
/*
  printf("Please enter a file name: ");
  
  // Reads in file name, if line return character at end, replaces with a zero.
  fgets(fname,100,stdin);
  if (fname[strlen(fname)-1] == '\n') {
    fname[strlen(fname)-1] = '\0';
  }
 
  // Verification that the file exists and can be opened.
  dat= fopen(fname, "r");
  if (dat== NULL) {
    fprintf(stderr, "ERROR: problem opening file %s.\n", fname);
    exit(EXIT_FAILURE); 
  }
*/
  
  // Counts how long the file is so all the requires variables can be created.
  num_lines= 0;
  while (fscanf (dat," %i %i %lf %lf",
    &dummy_var_int, &dummy_var_int, &dummy_var, &dummy_var)== 4) (num_lines++);
  
  // Create all data arrays.
  // Allocated in order of shortest to longest line. Not programming logic.
  year= malloc(num_lines* sizeof(int));
  month= malloc(num_lines* sizeof(int));
  y_ax_b= malloc(num_lines* sizeof(double));  
  temp_anom= malloc(num_lines* sizeof(double));  
  linear_date= malloc(num_lines* sizeof(double));
  linear_date_int= malloc(num_lines* sizeof(int));
  temp_mean_err= malloc(num_lines* sizeof(double));
  INV_temp_mean_err= malloc(num_lines* sizeof(double));
  INV_temp_mean_err_SQU= malloc(num_lines* sizeof(double));

  // Populates year, month, temp_anom, temp_mean_err
  // Also create linear date which turns {year}, {month} to {year.month}
  rewind(dat); // First reset the pointer to the start of the array.
  count= 0;
  while (fscanf (dat," %i %i %lf %lf\n", 
          year+count, month+count, temp_anom+count, temp_mean_err+count)== 4) {
    *(linear_date_int+ count) = 12* (*(year+count)) + (*(month+count));
    *(linear_date+ count) =
          (1.0* (*(linear_date_int+ count)) - (1.0* (*(month))))/12.0; 
    count++;    
  }
  
  // WANT TO SOLVE THIS WITH QSORT INSTEAD.
/*  
  // Finds the 5 highest temperatures in the data set and when they occurred
  // First initialise the values
  for (count=0;count<5; count++ ) {
    *(max_temp+ count)= *(temp_anom+ count);
    *(max_temp_index+ count)= *(linear_date+ count);
  }
  
  //need to make it so the new max value replaces the minimum of the last maximums.
  for (count= 5; count< num_lines; count++ ) {
    for (count2= 0; count2< 5; count2++ ) {
      if (*(temp_anom+count) > *(max_temp+ count2)) {
        *(max_temp+ count2)= *(temp_anom+ count);
        *(max_temp_index+ count)= count;
      }
    }
  }
*/  

  
  // All reading from file complete.
  fclose(dat);

  // Find the minimum and maximimum year of data.
  // Checking every value in the array in case data not in sequential order.
  min_year= *(year+0);
  max_year= *(year+0);
  for (count= 1; count< num_lines; count++) {
    if ( *(year+ count) < min_year ) {
      min_year = *(year+ count);
      min_index = count+1;
    }
    if ( *(year+ count) > max_year ) {
      max_year= *(year+ count);
      max_index= count+1;
    }
  } 
  
  // Ask user for year range input.
  // This is performed after reading in data so that the values given by the 
  // user can also be checked to be within range of the data.
  printf("The data set is for between the years %d and %d.\n",
          min_year, max_year);
  printf("Please enter a range of years that you wish to analyse in the form "
          "yyyy,yyyy: ");
    
  // Verifies that the user enteres a correct date range. In order of checks:
  // That the user enters 2 integers.
  // That the first date is before the second date
  // That the smallest date appears in the full data set.
  // That the largest date appears in the full data set.
  // I have left out the option of looking at a single year (min= max)
  if ( (scanf("%i%*[ ,;\t:]%i",&start_year, &end_year) !=2)
    || (end_year< start_year)
    || (start_year< min_year)
    || (end_year> max_year)) {
         fprintf(stderr, "ERROR: Invalid entry, Please start again.\n");
		     exit(EXIT_FAILURE);
	}
	
  // Finding the index positions of the start and end of the user provided
  // date range. This is used as the bounds in the summations.
	for (count= 0; count< num_lines; count++ ) {
	  if (start_year== *(year+ count)) {
	    start_year_index= count;
	    break;
	  } 
	}             
	// Because the last year of data may not have 11 months, manually set to the 
	// final month if the final year of the data set picked
  if (end_year== max_year) {
    end_year_index= num_lines- 1;
  } else {
    for (count= start_year_index; count< num_lines; count++) {
      if (end_year== *(year+ count)) { // Look for start of the year
        end_year_index= count+11; // Assign the last month of the year
        break;
      }
    }
  }
  
  // Calculateing intermediate assignment values
  INV_temp_mean_err= inverse(temp_mean_err, num_lines);
  xx= multiply(linear_date, linear_date, num_lines);
  xy= multiply(linear_date, temp_anom, num_lines);
  INV_temp_mean_err_SQU= 
      multiply(INV_temp_mean_err, INV_temp_mean_err, num_lines);
  xx_on_e_SQU= multiply(xx, INV_temp_mean_err_SQU, num_lines);
  xy_on_e_SQU= multiply(xy, INV_temp_mean_err_SQU, num_lines);  
  x_on_e_SQU= multiply(linear_date, INV_temp_mean_err_SQU, num_lines);
  y_on_e_SQU= multiply(temp_anom, INV_temp_mean_err_SQU, num_lines);
  
// DEBUG  for (count=0; count<num_lines; count++) {
// DEBUG    printf("%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t\n",
// DEBUG     *(INV_temp_mean_err+count), *(xx+count), *(xy+count),
// DEBUG     *(INV_temp_mean_err_SQU+count), *(xx_on_e_SQU+count), 
// DEBUG     *(xy_on_e_SQU+count), *(x_on_e_SQU+count), *(y_on_e_SQU+count));
// DEBUG  }

// DEBUG  for (count=0; count<num_lines; count++) {
// DEBUG    printf("%lf\t%lf\t%i\t\n", *(linear_date+count),
// DEBUG     *(INV_temp_mean_err_SQU+count), num_lines);
// DEBUG  }

  
  // Calculate the individual sums used for calculations later.
  // Completed this way to keep bug checks and calculations more human readable
  SUM_INV_temp= sum(INV_temp_mean_err_SQU, start_year_index, end_year_index);
  SUM_xx_on_e= sum(xx_on_e_SQU,start_year_index, end_year_index);
  SUM_xy_on_e= sum(xy_on_e_SQU,start_year_index, end_year_index);
  SUM_x_on_e= sum(x_on_e_SQU, start_year_index, end_year_index);
  SUM_y_on_e= sum(y_on_e_SQU, start_year_index, end_year_index);
  
  // Free up memory allocations.
  free(INV_temp_mean_err_SQU);
  free(INV_temp_mean_err);
  free(xx_on_e_SQU);
  free(xy_on_e_SQU);
  free(x_on_e_SQU);
  free(y_on_e_SQU);
  free(xx);
  free(xy);

  
  // Calculate the slope (a), the y-intercet (b)
  slope= (SUM_INV_temp* SUM_xy_on_e - SUM_x_on_e* SUM_y_on_e)/ 
        (SUM_INV_temp* SUM_xx_on_e - pow((SUM_x_on_e),2.0));      
  intercept= (SUM_y_on_e - (slope* SUM_x_on_e))/ (SUM_INV_temp);          
 
 
  // Calculate the average square error (E)
  for (count= start_year_index; count< end_year_index; count++) {
    *(y_ax_b+ count)= 
          pow(((*(temp_anom)- slope* (*(linear_date))+ intercept)), 2.0);
  }
  y_ax_b_e= multiply(y_ax_b, INV_temp_mean_err_SQU, num_lines);
  Error= (sum(y_ax_b_e, start_year_index, end_year_index))/ 
              (fabs(end_year_index- start_year_index));
  
  // Output values to the terminal.
  printf("\nLine of best fit is %lfx + %lf\n", slope, intercept);
  printf("The error is %lf\n\n", Error);
  
  // Output calculations to file for plotting. 
  out= fopen("output.dat", "w");
  for (count= 0; count< num_lines; count++) {
     fprintf(out, "%lf\t%lf\t%lf\n", 
          *(linear_date+ count), *(temp_anom+ count), *(temp_mean_err+ count));
  }

  // All writing to file complete.
  fclose(out);
  return (EXIT_SUCCESS);
}

// Functions ******************************************************************

// Multiplication of elements of two arrays.
double* multiply(double *x, double *y, int size) {
  double *xy= malloc(size* sizeof(double));
  int i;
  
  for(i= 0; i< size; i++)  {
    *(xy+ i) = (*(x+ i))* (*(y+ i)); 
  }
    
  return xy;
}

// Calculates the inverse of every element of an array.
double* inverse(double *x, int size) {
  double *inv_x= malloc(size* sizeof(double));
  int i;
  
  for(i= 0; i< size; i++) {
    *(inv_x+ i)= 1/ (*(x+ i));
  }
  
  return inv_x;
}

// Calculates the sum of an array
double sum(double *x, int start_year, int end_year) {
  double total= 0.0;
  int i;
   
  for(i= start_year; i< end_year; i++)  {
    total+= *(x+ i);       
  }
  
  return total;
}
