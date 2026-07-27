%comspec% /k "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" 
cl /c /I "C:\Users\Ross Pickford\AppData\Local\Programs\Python\Python314\include" t_dataCollection.c
cl /c /I "C:\Users\Ross Pickford\AppData\Local\Programs\Python\Python314\include" /I "C:\ExternalLibraries\SDL\include" t_displayNodes.c
cl /c /I "C:\Users\Ross Pickford\AppData\Local\Programs\Python\Python314\include" /I "C:\ExternalLibraries\SDL\include" t_renderer.c
cl /c /I "C:\Users\Ross Pickford\AppData\Local\Programs\Python\Python314\include" /I "C:\ExternalLibraries\SDL\include" main.c
link /machine:x64 /libpath:"C:\ExternalLibraries\SDL\lib\x64" /libpath:"C:\Users\Ross Pickford\AppData\Local\Programs\Python\Python314\libs" C:\ExternalLibraries\SDL\lib\x64\SDL3.lib t_dataCollection.obj t_displayNodes.obj t_renderer.obj main.obj /out:vicDisplay.exe
