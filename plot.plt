!/usr/bin/gnuplot -persist

a = 0.006441
b = -12.516323
c = 0.012306
d = -24.154829
f(x) = a * x + b
g(x) = c * x + d
h(y) = 1953 
set style line 1 lc rgb '#0060ad' linetype 1 linewidth 1
set style line 2 lc 'blue' pt 7   # circle
set style line 3 lc 'black' linetype 1 linewidth 1

plot "output.dat" using 1:2 title "Temperature Anomaly Measurements", \
     f(x) title "Line of best fit for entire data set" w l linestyle 1, \
     g(x) title "Line of best fit for final 60 years" w l linestyle 3
      


#  TITLES
set title "phys3071 as08 melsom 42593249"
set xlabel "Year"
set ylabel "Temperature Anomaly (K)"

# AXES SETTINGS
set yrange [-0.8:1]
#set xrange [18:38]
set size square
#set xtics ('-2pi' -2*pi, '-pi' -pi, 0, 'pi' pi, '2pi' 2*pi) 
#set ytics 1
#set tics scale 0.75

# LEGEND
#set key default
#set key top left
#set key outside
set key bottom right

# FILE OUTPUT
set term postscript color
set output "as08-problem2-melsom-42593249.ps"
replot
