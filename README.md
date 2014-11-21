eger
====

Kézzel vezérelt egér opencv és cvblob használatával


Ahhoz, hogy működjön opencv feltelepítése után Linuxban a Cobeblocks-ban a Settings->Search Directories-hoz hozzá kell adni az ...opencv-2.4.9./include/opencv és ...opencv-2.4.9./include/opencv2 könyvtárakat, a Linker settings-nél pedig az ...opencv-2.4.9./buil/lib/*.so fájlokat. A http://code.google.com/p/cvblob/ címről le kell szedni a cvblobot, majd az .../cvblob/cvBlob/cvblob.h -t átmásolni a ...opencv-2.4.9./include/opencv -be, a többi, *.cpp-t pedig a projekthez kell adni.

Plusz az egérműveletekhez fel kell telepíteni a libx11 és libxtst libeket:
sudo apt-get install libx11-dev
sudo apt-get install libxtst-dev
Ezután codeblocksban hozzá kell adni a következőket a Settings -> Compiler -> Linker Settings-ben: 
/usr/lib/x86_64-linux-gnu/libX11.so és 
/usr/lib/x86_64-linux-gnu/libXtst.so
Ha minden igaz ezek után futnia kell :D
