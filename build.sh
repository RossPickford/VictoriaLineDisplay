#!/bin/bash
cc t_request.c -o t_request.exe
cc -c t_dataCol_simple.c -o datacol.o
cc -I ./include -Wall -O3 -march=native -mtune=native -flto=2 -g -Wextra -Wno-unused-parameter -c -o renderer.o t_renderer.c
cc -I ./include -Wall -O3 -march=native -mtune=native -flto=2 -g -Wextra -Wno-unused-parameter -c -o main.o main.c
cc datacol.o renderer.o main.o -o vl_final -march=native -mtune=native -flto=2 -L ./lib -lrgbmatrix -lrt -lm -lpthread -lstdc++


# cc -I ./include -Wall -O3 -march=native -mtune=native -flto=2 -g -Wextra -Wno-unused-parameter -c -o renderer.o t_renderer.c
# cc datacol.o renderer.o main.o -o vl_render_test -march=native -mtune=native -flto=2 -L ./lib -lrgbmatrix -lrt -lm -lpthread -lstdc++
