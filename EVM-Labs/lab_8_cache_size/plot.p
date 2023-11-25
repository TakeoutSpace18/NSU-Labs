L1d_cache_size = 49152
L2_cache_size = 1310720
L3_cache_size = 12582912

set xlabel "array size (bytes)"
set ylabel "time"
set yzeroaxis
set xzeroaxis
set parametric
set arrow from L1d_cache_size, graph 0 to L1d_cache_size, graph 1 nohead
set arrow from L2_cache_size, graph 0 to L2_cache_size, graph 1 nohead
set arrow from L3_cache_size, graph 0 to L3_cache_size, graph 1 nohead

set datafile separator comma
plot "measured_data_random.csv" with lines
pause -1 "Hit any key to continue"