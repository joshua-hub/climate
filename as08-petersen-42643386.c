/* PHYS3071 AS08 PETERSEN 42643386
 * 
 * Description:
 *  
 * 
 * Inputs:
 *  
 * 
 * Outputs:
 *  
 */

//============================================================================
// HEADERS

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

//============================================================================

// Strucutre to contain datafile lines.
typedef struct datafile_t {
  int year;
  int month;
  double temp_anomaly;
  double temp_mean_error;
} DataFile;

typedef struct linreg_t {
  double slope;
  double intercept;
  double avg_sq_err;
} LinReg;

//============================================================================

void Linear_Regression(DataFile *climate_data, 
                       int lower, int upper,
                       LinReg *regression);

double LinReg_Slope(double *x_inverrsq, 
                    double *y_inverrsq, 
                    double *xx_inverrsq, 
                    double *xy_inverrsq,
                    double *inv_err_sq,
                    size_t array_size);

double LinReg_Intercept(double *x_inverrsq, 
                        double *y_inverrsq, 
                        double *inv_err_sq,
                        size_t array_size, 
                        double slope);

double AvgSqrErr(const double *x, const double *y, double *inv_err_sq,
                 size_t array_size, double slope, double intercept);

double SumOver(double *array, size_t array_size);
double *ElementMulti(double *x, double *y, size_t array_size);
int compareDataFile(const void* p1, const void* p2);
//============================================================================

int main(){
  const size_t filename_length = 100, data_columns = 4;
  int linenum = 0;
  int i, lower, upper;
  
  char *in_fname = (char *)malloc(filename_length * sizeof(char)); 
  DataFile *climate_data;
  DataFile *highest_temps = (DataFile *)calloc(5, sizeof(DataFile));
  LinReg climate_reg;
  FILE *in_file;
  
  // Get filename for data and open file stream
  printf("Name of datafile? ");
  fgets(in_fname, filename_length, stdin);
  in_fname = strtok(in_fname, "\n");
  in_file = fopen(in_fname, "r");
  
  // Check file stream
  if (in_file == NULL)
  {
    fprintf(stderr, "Error opening file.\n");
    exit(EXIT_FAILURE);
  }
  
  // Find the number of lines in the file.
  while (fscanf(in_file, "%[^\n]\n", in_fname) != EOF) linenum++;
  free(in_fname);
  
  // Read datafile into memory.
  rewind(in_file);
  climate_data = (DataFile *)malloc(linenum * sizeof(DataFile));
  i = 0;
  while (fscanf(in_file, "%d %d %lf %lf", 
                &(*(climate_data + i)).year,
                &(*(climate_data + i)).month,
                &(*(climate_data + i)).temp_anomaly,
                &(*(climate_data + i)).temp_mean_error) == data_columns) {
    
    
    // Find the highest 5 temperatures within the data.
    if (i < 5) {
      (*(highest_temps)) = (*(climate_data + i));
    } 
    qsort((void *)highest_temps, 5, sizeof(DataFile), &compareDataFile);3333
    else if ((*(climate_data+ i)).temp_anomaly> (*highest_temps).temp_anomaly) {
        (*highest_temps)= (*(climate_data+ i));
        qsort((void *)highest_temps, 5, sizeof(DataFile), &compareDataFile);
    }
    
    i++;
  }
  fclose(in_file);
  
  // Get the year range for regression.
  printf("Year range to fit regression: ");
  if(scanf("%d %d", &lower, &upper) != 2 || 
    (upper > (*(climate_data + linenum -1)).year) ||
    (lower < (*climate_data).year) ||
    (lower > upper))
  {
    fprintf(stderr, "Error: Invalid year range.\n");
    exit(EXIT_FAILURE);
  }

  // Calculate linear regression.
  Linear_Regression(climate_data, lower, upper, &climate_reg);
  
  // Free memory
  free(climate_data);
  
  // Output results
  printf("f(x) = %5.4le * x + %5.4le\nAverage Square Error: %5.4le\n", 
         climate_reg.slope, climate_reg.intercept, climate_reg.avg_sq_err);
  
  // Print times of 5 highest temps
  printf("Highest Deviations:\n");
  for (i = 0; i < 5; i++) 
    printf("Year: %d Month: %d Dev: %lf\n",
           (*(highest_temps+i)).year, 
           (*(highest_temps+i)).month, 
           (*(highest_temps+i)).temp_anomaly
          );
  free(highest_temps);
  
  return EXIT_SUCCESS;
}

//----------------------------------------------------------------------------

void Linear_Regression(DataFile *climate_data, 
                       int lower, int upper,
                       LinReg *regression)
{
  const int lowest = (*climate_data).year, month_in_year = 12;
  int i, array_size = 12 * (upper - lower);
  double *x_inverrsq, *y_inverrsq, *xy_inverrsq, *xx_inverrsq;
  double *data_x = (double *)calloc(array_size, sizeof(double));
  double *data_y = (double *)calloc(array_size, sizeof(double));
  double *inv_err_sq = (double *)calloc(array_size, sizeof(double));
  
  lower = month_in_year * (lower - lowest);

  for (i = 0; i < array_size; i++)
  {
    *(data_x + i) = (double)((*(climate_data + i + lower)).year * month_in_year 
                  + (*(climate_data + i + lower)).month);
    *(data_y + i) = (*(climate_data + i + lower)).temp_anomaly;
    *(inv_err_sq + i) = 
      pow((*(climate_data + i + lower)).temp_mean_error, -2.0);
  }
  
  x_inverrsq = ElementMulti(data_x, inv_err_sq, array_size);
  y_inverrsq = ElementMulti(data_y, inv_err_sq, array_size);
  xy_inverrsq = ElementMulti(data_x, y_inverrsq, array_size);
  xx_inverrsq = ElementMulti(data_x, x_inverrsq, array_size);
  
  // Calculate regression
  (*regression).slope = LinReg_Slope(x_inverrsq, y_inverrsq, 
                                     xx_inverrsq, xy_inverrsq,
                                     inv_err_sq, array_size);
  
  (*regression).intercept = LinReg_Intercept(x_inverrsq, y_inverrsq, inv_err_sq,
                                             array_size,
                                             (*regression).slope);
  
  (*regression).avg_sq_err = AvgSqrErr(data_x, data_y, inv_err_sq,
                                       array_size,
                                       (*regression).slope, 
                                       (*regression).intercept);
  
  free(data_x);
  free(data_y);
  free(inv_err_sq);
  free(x_inverrsq);
  free(y_inverrsq);
  free(xx_inverrsq);
  free(xy_inverrsq);
}

//----------------------------------------------------------------------------

double SumOver(double *array, size_t array_size)
{
  int i;
  double temp = 0.0;
  
  for (i = 0; i < array_size; i++) temp += *(array + i);
  return temp;
}

//----------------------------------------------------------------------------

double *ElementMulti(double *x, double *y, 
                      size_t array_size)
{
  int i;
  double *xy = (double *)calloc(array_size, sizeof(double));
  
  for(i = 0; i < array_size; i++) *(xy + i) = *(x + i) * *(y + i);
  return xy;
}

//----------------------------------------------------------------------------

double LinReg_Slope(double *x_inverrsq, double *y_inverrsq, 
                    double *xx_inverrsq, double *xy_inverrsq,
                    double *inv_err_sq, size_t array_size)
{ 
  return (SumOver(inv_err_sq, array_size) * SumOver(xy_inverrsq, array_size) 
       - (SumOver(x_inverrsq, array_size) * SumOver(y_inverrsq, array_size))) 
       / (SumOver(inv_err_sq, array_size) * SumOver(xx_inverrsq, array_size) 
       - pow(SumOver(x_inverrsq, array_size), 2.0));
}

//----------------------------------------------------------------------------

double LinReg_Intercept(double *x_inverrsq, double *y_inverrsq, 
                        double *inv_err_sq,
                        size_t array_size, double slope)
{
  return (SumOver(y_inverrsq, array_size) 
       - slope * SumOver(x_inverrsq, array_size)) 
       / (SumOver(inv_err_sq, array_size));
}

//----------------------------------------------------------------------------

double AvgSqrErr(const double *x, const double *y, double *inv_err_sq, 
                 size_t array_size, double slope, double intercept)
{
  int i;
  double error = 0.0;
  
  for (i = 0; i < array_size; i++) 
    error += *(inv_err_sq+i) * 
              pow((*(y+i) - (slope * *(x+i) + intercept)), 2.0);
  
  return error / ((double)array_size);
}

//----------------------------------------------------------------------------

int compareDataFile(const void* p1, const void* p2)
{
  if ((*((DataFile*)p1)).temp_anomaly < 
      (*((DataFile*)p2)).temp_anomaly) return -1;
  
  if ((*((DataFile*)p1)).temp_anomaly == 
      (*((DataFile*)p2)).temp_anomaly) return 0;
  
  return 1;
}

