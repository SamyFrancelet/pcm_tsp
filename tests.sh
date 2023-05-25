#! /bin/bash
echo "Building the project"
make tspmt
echo "Add execution rights to the binary"
#chmod +x tspmt
echo "Creating a result file: ./results/result.txt"
mkdir results/ || touch results/result.txt
echo "" > ./results/result.txt
echo "The tests are executed on the file ./text_data/dj38.tsp with 16 cities."
echo "Launching the tests !"
for i in {1..255..10}
do
  echo "Test with " $i " threads"
  ./tspmt ./test_data/lau15.tsp $i >> ./results/result.txt
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
set output "./results/speedup.png"
plot "./results/result.txt" using 1:(1 / column(3)) title "Speedup" with linespoints lt 2 pt 9 ps 1.2 lc rgb "#0000FF"
EOF

gnuplot -persist << EOF
set datafile separator ";"
set xlabel "Number of Threads"
set ylabel "Efficiency"
set title "Efficiency"
set key outside
set grid
set term pngcairo enhanced
set output "./results/efficiency.png"
plot "./results/result.txt" using 1:((column(3) / column(1))) title "Efficiency" with linespoints lt 1 pt 7 ps 1.2 lc rgb "#FF0000"

EOF
