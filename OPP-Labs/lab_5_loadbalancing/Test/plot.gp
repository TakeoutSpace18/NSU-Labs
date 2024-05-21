set term qt

set xlabel "iterations"

set autoscale

set timefmt "%S"
set ylabel "time"
set ydata time

set title "Load balancing graph"
set key reverse Left outside
set grid

set style data lines


FILES = system("ls -1 *.dat")
plot for [data in FILES] data using 1:2 notitle