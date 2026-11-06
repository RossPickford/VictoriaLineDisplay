cc -I ./include -Wall -O3 -march=native -mtune=native -flto=2 -g -Wextra -Wno-unused-parameter -c -o render_test.o render_test.c
cc render_test.o -o vl_render_test -march=native -mtune=native -flto=2 -L ./lib -lrgbmatrix -lrt -lm -lpthread -lstdc++
