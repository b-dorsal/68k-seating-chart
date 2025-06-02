set -e 
set -x 
make clean
make
hmount ../images/FD30_512\ 20MB.hda 
hcopy SeatingChart.bin :SeatingChart
hls
humount
cp ../images/FD30_512\ 20MB.hda /Volumes/BLUESCSI
make clean