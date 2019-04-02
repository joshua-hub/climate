// phys3071 ws14/15 melsom 42593249

/*
This program reads in a file with 2 data sets formatted in a column per 
variable. The file computes the line of best fit and outputs the equation to 
the terminal window. 

INPUTS: The user is required to input the name of the file containing the data

CALCULATIONS: The program consists 
*/

// Import libraries ***********************************************************
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// Global varibables **********************************************************
// I wish!

// Function prototypes ********************************************************
double sum(double *x, int size);
double* multiply(double *x, double *y, int size);
double* inverse(double *x, int size);

// Begin main program *********************************************************
int main () {

  char fname[100];

  FILE *dat;
  
  int num_lines, count, dummy_var_int;
  int *linear_date_int;
  int *year, *month, *month_count;
  int min_year, max_year, max_index, min_index, start_year, end_year;
  
 double *temp_anom, *temp_mean_err, *INV_temp_mean_err, *INV_temp_mean_err_SQU;
  double *xx, *xy, *y_ax_b;
  double *xy_on_e_SQU, *x_on_e_SQU, *y_on_e_SQU, *xx_on_e_SQU, *y_ax_b_e;
  double SUM_INV_temp, SUM_xx_on_e, SUM_xy_on_e, SUM_x_on_e, SUM_y_on_e;
  double slope, intercept, Error;
  double dummy_var;
  double *linear_date;

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
  month_count= malloc(num_lines* sizeof(int));
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
    *(linear_date_int+ count) = 12* (*(year+count)) + (*(month_count));
    *(linear_date+ count) = (*(linear_date_int+ count) - *(month_count))/ 12.0;
    count++;
  }
  
  // All reading from file complete.
  fclose(dat);
  
  // Find the minimum and maximi year of data.
  // Checking every value in the array in case data not in sequential order.
  min_year= *year;
  max_year= *year;
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
 
/* DEBUGGING  
  for (count=0; count<num_lines; count++) {
    printf("%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t\n",
     *(INV_temp_mean_err+count), *(xx+count), *(xy+count),
     *(INV_temp_mean_err_SQU+count), *(xx_on_e_SQU+count), 
     *(xy_on_e_SQU+count), *(x_on_e_SQU+count), *(y_on_e_SQU+count));
  }
*/

  for (count=0; count<num_lines; count++) {
    printf("%lf\t%lf\t%i\t\n", *(linear_date+count),
     *(INV_temp_mean_err_SQU+count), num_lines);
  }

  // Calculate the individual sums used for calculations later.
  // Completed this way to keep bug checks and calculations more human readable
  SUM_INV_temp= sum(INV_temp_mean_err_SQU, num_lines);
  SUM_xx_on_e= sum(xx_on_e_SQU,num_lines);
  SUM_xy_on_e= sum(xy_on_e_SQU, num_lines);
  SUM_x_on_e= sum(x_on_e_SQU, num_lines);
  SUM_y_on_e= sum(y_on_e_SQU, num_lines);
  
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
  for(count= 0; count< num_lines; count++) {
    *(y_ax_b+ count)= 
          pow(((*(temp_anom)- slope* (*(linear_date))+ intercept)), 2.0);
  }
  y_ax_b_e= multiply(y_ax_b, INV_temp_mean_err_SQU, num_lines);
  Error= (sum(y_ax_b_e, num_lines))/num_lines;
  
  // Output values to the terminal.
  printf("\nLine of best fit is %2.3lfx + %2.3lf\n", slope, intercept);
  printf("The error is %lf\n\n", Error);
   
  return (EXIT_SUCCESS);
}

// Functions ******************************************************************

// Dot product of 2 arrays.
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
double sum(double *x, int size) {
  double total= 0.0;
  int i;
   
  for(i= 0; i< size; i++)  {
    total+= *(x+ i);       
  }
  
  return total;
}
