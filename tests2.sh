#! /bin/bash
echo "Building the project"
make tspmt
echo "Add execution rights to the binary"
#chmod +x tspmt
echo "Creating a result file: ./results_29/result.txt"
mkdir results_29/ || touch results_29/result.txt
echo "" > ./results_29/result.txt
echo "The tests are executed on the file ./text_data/wi29.tsp with 29 cities."
echo "Launching the tests !"
for i in {100..255..5}
do
  echo "Test with " $i " threads"
  ./tspmt ./test_data/wi29.tsp $i >> ./results_29/result.txt
done
echo "Everything's done!"

gnuplot -persist << EOF
set datafile separator ";"
set xlabel "Number of Threads"
set ylabel "Time"
set title "Speedup"
set key outside
set grid
set term pngcairo enhanced
set output "./results_29/speedup.png"
plot "./results_29/result.txt" using 1:(1 / column(2)) title "Speedup" with linespoints lt 2 pt 9 ps 1.2 lc rgb "#0000FF"
EOF

gnuplot -persist << EOF
set datafile separator ";"
set xlabel "Number of Threads"
set ylabel "Efficiency"
set title "Efficiency"
set key outside
set grid
set term pngcairo enhanced
set output "./results_29/efficiency.png"
plot "./results_29/result.txt" using 1:((column(2) / column(1))) title "Efficiency" with linespoints lt 1 pt 7 ps 1.2 lc rgb "#FF0000"

EOF
