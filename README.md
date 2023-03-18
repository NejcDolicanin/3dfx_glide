3dfx glide

Continuing where 3dfx/KoolSmoky left off!

Its a different version from where sourceforge/glide project is atm, here on github user sezero is pushing it forward and is considered(and is) more "up to date".
Check it here: https://github.com/sezero/glide

But since it has better compatibility with Win98, it is the version I will push forward. 
Also provides a nice learning experience porting sourceForge/glide project changes to it.
Thanks to 3dfx and everyone involved in glide and mesa projects.

This glide3x.dll version is used in the 3dfx Wide driver v1.1 available here:
https://www.3dfxzone.it/dir/3dfx/drivers/?objid=1137
"Discussion-ish" thread:
https://www.3dfxzone.it/enboard/topic.asp?TOPIC_ID=36175


Tools needed to build(as used):
- Ms Windows Xp sp3 (in virtual machine)
- Ms Visual cpp 6.0 (pro)
- Ms Windows XP DDK (5.1.2600)
- Ms DirectX 9 SDK

Additional environment settings:
- Add msvc to Environment variables /system/PATH
  Or use "Win Me Free Build Environment" that comes with xpddk.
  
  What I had in PATH in the time of writing "...C:\DXSDK9.0\Bin\DXUtils;C:\Program Files\Microsoft Visual Studio\VC98\Bin"
  
Additional settings:
- Open glide2x.bat and/or glide3x.bat and correct paths that reflect your own.


Build Instructions:

"use: glide3x.bat [ all | gendate | minihwc | dll ] [ clean ]"

- Open "Win Me Free Build Environment" terminal from ddk
- Locate project files
- Enter:
	- glide3x gendate
	- glide3x minihwc
	- glide3x dll
	
	- If you get a linker warning, ignore it
	
You are done, outputed .dll is located in: 3dfx_glide\h5\bin
	
	ToDo printscreen
	
Same procedure for glide2x, just use glide2x.bat

