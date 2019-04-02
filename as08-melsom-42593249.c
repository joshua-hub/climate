// phys3071 ws14/15 melsom 42593249

/*
DESCRIPTION: This program will calculate the line of best fit through a dataset
of climate data over a user defined region.

INPUTS: The user is required to enter the name of the data file containing the
data columns: {year(int), month(int), temperature anomaly(double),
temp. mean error(double)}, and also a year range to calculate a line of best
fit over.

CALCULATIONS: The Program finds the line of best fit that minimimises the sum
of errors between the line and each data point (with error weighting).

OUTPUT: The program outputs to terminal the equation for the line of best fit
as well as the average squared error and the average error. A output.dat is
created that has the following data columns {year.month(double),
temperature anomaly(double), temp. mean error(double)}. This allows plotting of
the data over the whole range, the user can also use the equation printed to
terminal to plot a line of best fit for their given dat rage through the data.

Compiled as gcc as08-melsom-42593249.c -o as08 -Wall -lm
*/

// Import libraries ***********************************************************
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

// Global varibables **********************************************************
// I wish!

// Function prototypes ********************************************************

// Element by element multiplication. Returns an array pointer.
  double* multiply(double *x, double *y, int size);

// Calculates the inverse of each element of an array. Returns array pointer.
  double* inverse(double *x, int size);

// Element by element addition. Returns an array pointer.
  double sum(double *x, int start_year, int end_year);

// Begin main program *********************************************************
int main () {
  // VARIABLES DECLARATIONS ***************************************************

  char fname[100];
  FILE *dat; // input file
  FILE *out; // output file
  // num_lines counts the number of lines in the datafile. count and count2 are
  // generic counters I use, and dumm_var to dump info to that I no longer need
  int num_lines, count, count2, dummy_var_int;

  // Min and max year are min and max of the set, start and end are the bounds
  // on the user define date range. Year and month are from dataset
  int min_year, max_year, start_year, end_year, *year, *month;

  // finding the position of the user defined dat range
  int start_year_index, end_year_index;

  // Varaibles for finding the maximum temp dates.
  int *max_temp_index, min_max_temp_index;

  // temp_anom and temp_mean_err from dataset
  double *temp_anom, *temp_mean_err;

  // Intermediate calculations for equations (2)
  double *xx, *xy, *y_ax_b, *INV_temp_mean_err,*INV_temp_mean_err_SQU;
  double *xy_on_e_SQU, *x_on_e_SQU, *y_on_e_SQU, *xx_on_e_SQU, *y_ax_b_e;
  double SUM_INV_temp, SUM_xx_on_e, SUM_xy_on_e, SUM_x_on_e, SUM_y_on_e;

  // Final calculation variables for equations (1) and (2)
  double slope, intercept, Error;

  // Linear date for year and month combined as a single variable, a dummy var
  // as above and variables to find the 5 maximum temp times.
  double dummy_var, *linear_date, *max_temp, min_max_temp;


  // IMPORTING DATA ***********************************************************

  // Ask the user for a file name
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

  // Counts how long the file is so all the requires variables can be created.
  num_lines= 0;
  while (fscanf (dat," %i %i %lf %lf",
    &dummy_var_int, &dummy_var_int, &dummy_var, &dummy_var)== 4) (num_lines++);

  // Create all data arrays.
  // Allocated in order of shortest to longest line. Not programming logic.
  max_temp= malloc(5* sizeof(double)); // For finding 5 max temp dates
  year= malloc(num_lines* sizeof(int));
  month= malloc(num_lines* sizeof(int));
  max_temp_index= malloc(5* sizeof(int)); // For finding 5 max temp dates
  y_ax_b= malloc(num_lines* sizeof(double));
  y_ax_b_e= malloc(num_lines* sizeof(double));
  temp_anom= malloc(num_lines* sizeof(double));
  linear_date= malloc(num_lines* sizeof(double));
  temp_mean_err= malloc(num_lines* sizeof(double));
  INV_temp_mean_err= malloc(num_lines* sizeof(double));
  INV_temp_mean_err_SQU= malloc(num_lines* sizeof(double));

  // Populates year, month, temp_anom, temp_mean_err
  // Also create linear date which turns {year}, {month} to {year.month}
  rewind(dat); // First reset the pointer to the start of the array.
  count= 0;
  while (fscanf (dat," %i %i %lf %lf\n",
          year+count, month+count, temp_anom+count, temp_mean_err+count)== 4) {
    *(linear_date+ count) =
          (1.0* (*(year+ count)))+ (1.0/12.0)* (*(month+ count) - 1);
    count++;
  }


  // FIND THE 5 MAXIMUM TEMPERATURES AND THEIR INDEXES ************************
  // After writing this I have realised there is a slightly more efficient
  // Initialise the max set arrays.
  min_max_temp= *temp_anom;
  min_max_temp_index= 0;
  for (count= 0; count< 5; count++) {
    *(max_temp+ count)= *(temp_anom+ count);
    *(max_temp_index+ count)= count;
    if (*(max_temp+ count) > min_max_temp) {
      min_max_temp= *(max_temp+ count);
      min_max_temp_index= count;
    }
  }
  // Finds the maximum temperatures and their position in the dataset.
  for (count= 5; count< num_lines; count++) {

    // If the current temp is less than the minimum value in the maximum set
    if (*(temp_anom+ count)> min_max_temp) {
      min_max_temp = *(temp_anom+ count);
      *(max_temp+ min_max_temp_index) = min_max_temp;
      *(max_temp_index+ min_max_temp_index)= count;

      // Finding the new mimimum in the maximum set.
      for (count2= 0; count2< 5; count2++) {
        if (*(max_temp+ count2) < min_max_temp) {
          min_max_temp= *(max_temp+ count2);
          min_max_temp_index= count2;
        }
      }
    }
  }

  // All reading from file complete.
  fclose(dat);

  // TOTAL DATASET RANGE ******************************************************
  // Find the minimum and maximimum year of data.
  // Checking every value in the array in case data not in sequential order.
  min_year= *(year+ 0);
  max_year= *(year+ 0);
  for (count= 1; count< num_lines; count++) {
    if ( *(year+ count) < min_year ) {
      min_year = *(year+ count);
    }
    if ( *(year+ count) > max_year ) {
      max_year= *(year+ count);
    }
  }

  // AFTER PARTIAL READ OF DATA ASK FOR USER INPUT WITHIN BOUNDS **************
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
	  if (start_year== *(year+ count)) { // Look for start of the year
	    start_year_index= count; // Assign to the first month of the year
	    break;
	  }
	}
	// Because the last year of data may not have 12 months, manually set to the
	// final month if the final year of the data set picked
  if (end_year== max_year) {
    end_year_index= num_lines- 1;
  } else {
    for (count= start_year_index; count< num_lines; count++) {
      if (end_year== *(year+ count)) { // Look for start of the year
        end_year_index= count+ 11; // Assign the last month of the year
        break;
      }
    }
  }

  // Free up memory allocations.
  free(year);
  free(month);

  // BEGIN CALCULATING INTERMEDIATE VALUES ************************************

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

  // Calculate the individual sums used for calculations later.
  // Completed this way to keep bug checks and calculations more human readable
  SUM_INV_temp= sum(INV_temp_mean_err_SQU, start_year_index, end_year_index);
  SUM_xx_on_e= sum(xx_on_e_SQU,start_year_index, end_year_index);
  SUM_xy_on_e= sum(xy_on_e_SQU,start_year_index, end_year_index);
  SUM_x_on_e= sum(x_on_e_SQU, start_year_index, end_year_index);
  SUM_y_on_e= sum(y_on_e_SQU, start_year_index, end_year_index);

  // Free up memory allocations.
  free(xx);
  free(xy);
  free(x_on_e_SQU);
  free(y_on_e_SQU);
  free(xx_on_e_SQU);
  free(xy_on_e_SQU);

  // FINAL CALCULATIONS *******************************************************
  // Calculate the slope (a), the y-intercet (b)
  slope= (SUM_INV_temp* SUM_xy_on_e - SUM_x_on_e* SUM_y_on_e)/
        (SUM_INV_temp* SUM_xx_on_e - pow((SUM_x_on_e),2.0));
  intercept= (SUM_y_on_e - (slope* SUM_x_on_e))/ (SUM_INV_temp);

  for (count= 0; count<num_lines; count++) {
    *(y_ax_b+ count)=
         pow((*(temp_anom+ count)-
                (slope* (*(linear_date+ count))+ intercept)), 2.0);
    *(y_ax_b_e+ count)=
                ((*(y_ax_b+ count))* (*(INV_temp_mean_err_SQU+ count)));
  }
  Error= sum(y_ax_b_e, start_year_index, end_year_index);
  Error = Error/(end_year_index - start_year_index);

  // OUTPUT SECTION ***********************************************************
  // Output values to the terminal.
  printf("\nLine of best fit is %lfx + %lf.\n", slope, intercept);
  printf("The average squared error is %lf.\n", Error);
  printf("The average error is %lf.\n\n", sqrt(Error));
  printf("The 5 maximum temperature anomalies in this dataset are as follows\n");
  printf("Temperature anomaly  \t Date\n");
  for (count= 0; count< 5; count++) {
    printf("%lf \t\t %lf\n", *(max_temp+ count),
                             *(linear_date+ (*(max_temp_index+ count))));
  }

  // Output calculations to file for plotting.
  out= fopen("output.dat", "w");
  for (count= 0; count< num_lines; count++) {
     fprintf(out, "%lf\t%lf\t%lf\n",
          *(linear_date+ count), *(temp_anom+ count), *(temp_mean_err+ count));
  }
  // All writing to file complete.
  fclose(out);

  printf("\n");

  // Free up all remaining memory allocations.
  free(temp_anom);
  free(linear_date);
  free(INV_temp_mean_err);
  free(INV_temp_mean_err_SQU);

  return (EXIT_SUCCESS);
}
// Functions ******************************************************************

// Multiplication of elements of two arrays.
double* multiply(double *x, double *y, int size) {
  double *xy= malloc(size* sizeof(double));
  int i;

  for(i= 0; i< size; i++)
    (*(xy+ i) = (*(x+ i))* (*(y+ i)));
  return xy;
}

// Calculates the inverse of every element of an array.
double* inverse(double *x, int size) {
  double *inv_x= malloc(size* sizeof(double));
  int i;

  for(i= 0; i< size; i++)
    (*(inv_x+ i)= 1/ (*(x+ i)));
  return inv_x;
}

// Calculates the sum of an array
double sum(double *x, int start_year, int end_year) {
  double total= 0.0;
  int i;

  for(i= start_year; i< end_year; i++)
    (total+= *(x+ i));
  return total;
}
