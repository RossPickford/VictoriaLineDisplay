%comspec% /k "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat"
cl /c /I "C:\Users\Ross Pickford\AppData\Local\Programs\Python\Python314\include" trains.c
cl /c /I "C:\Users\Ross Pickford\AppData\Local\Programs\Python\Python314\include" main.c
link /machine:x64 /libpath:"C:\Users\Ross Pickford\AppData\Local\Programs\Python\Python314\libs" trains.obj main.obj /out:vicDisplay.exe