gcc -c libcurlTest.c -o libCurlTest.o -I  "external\libcurl\include"
gcc libCurlTest.o -o libcurlTest.exe -L "external\libcurl\lib" -lcurl