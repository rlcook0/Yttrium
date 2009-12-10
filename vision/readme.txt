SURF Bag-O'-Words Object Detector

Chris Anderson, Juan Batiz-Benet, Ivan Lee
SUNet IDs: cadander, jbenet, iylee

TO MAKE:

make

TO RUN:

setenv LD_LIBRARY_PATH /afs/ir/user/c/a/cadander/public/opencv/lib
cp /afs/ir/users/c/a/cadander/public/needed_dats/* .
./train -t -o 4000 -k 4000 /afs/ir/class/cs221/vision/data/vision_all
./run -o performance.xml -x -t -k 4000 -g /afs/ir/class/cs221/vision/data/easy.xml /afs/ir/class/cs221/vision/data/easy/
./evaluate -m performance.xml -g /afs/ir/class/cs221/vision/data/easy.xml

TO UNDERSTAND:

We will explain in the coming write-up.

(but -t means train and use boosted decision trees)