GCC_FLAGS='gcc -DNDEBUG -O3 -Wall -fopenmp'
cd path
$GCC_FLAGS -c path.c
cd ../ll/
$GCC_FLAGS -c ll.c 
cd ../io/
$GCC_FLAGS -c io.c
cd ../kmerize/
$GCC_FLAGS -c kmerize.c
cd ../findmp/
$GCC_FLAGS -c findmp.c 
cd ..
$GCC_FLAGS ll/ll.o io/io.o kmerize/kmerize.o findmp/findmp.o path/path.o naiveAssembler.c -o naive
