gcc t_request.c -o t_request.exe
gcc -c t_dataCol_simple.c -o datacol.o
gcc -c t_renderer.c -o renderer.o -I "C:\ExternalLibraries\SDL\include"
gcc -c main.c -o main.o -I "C:\ExternalLibraries\SDL\include"
gcc datacol.o renderer.o main.o -o vld_software.exe -L "c:\ExternalLibraries\SDL\lib" -lsdl3