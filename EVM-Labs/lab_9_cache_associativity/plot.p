L1d_cache_size = 49152
L2_cache_size = 1310720
L3_cache_size = 12582912

set xlabel "banks number"
set ylabel "average access time"
set yzeroaxis
set xzeroaxis
set parametric
set arrow from 12, graph 0 to 12, graph 1 nohead


set datafile separator comma
plot "cmake-build-debug/measured_data.csv" with linespoints
pause -1 "Hit any key to continue"