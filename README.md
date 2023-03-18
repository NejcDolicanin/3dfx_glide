# 3dfx glide

Continuing where 3dfx/KoolSmoky left off!

![glide0000](https://user-images.githubusercontent.com/62139759/226144180-59698280-4c5e-43f2-a879-befe11ca770d.png)

Its a different version from where sourceforge/glide project is atm, here on github can be found under sezeros repo. Its considered(and is) more "up to date". Check it here: [https://github.com/sezero/glide](https://github.com/sezero/glide). Or original [sourceForge/glide project](https://sourceforge.net/projects/glide/).

But since this one has better compatibility with Win98, it is the version I will push forward. 
Also provides a nice learning experience porting sourceForge/glide project changes to it.
Thanks to 3dfx and everyone involved in glide and mesa projects.

This glide3x.dll version is used in the [3dfx Wide driver v1.1](https://www.3dfxzone.it/dir/3dfx/drivers/?objid=1137)
"Discussion-ish" thread on [3dfxzone](https://www.3dfxzone.it/enboard/topic.asp?TOPIC_ID=36175)


## Tools needed to build:
(what I used)
- Ms Windows Xp sp3 (in virtual machine)
- Ms Visual cpp 6.0 (pro)
- Ms Windows XP DDK (5.1.2600)
- Ms DirectX 9 SDK

### Additional environment settings:
- Add msvc to Environment variables /system/PATH
  Or use "Win Me Free Build Environment" that comes with xpddk.
  
  What I had in PATH in the time of writing "...C:\DXSDK9.0\Bin\DXUtils;C:\Program Files\Microsoft Visual Studio\VC98\Bin"
  
### Additional settings:
- Open glide2x.bat and/or glide3x.bat and correct paths that reflect your own.

## Build Instructions:

"use: glide3x.bat [ all | gendate | minihwc | dll ] [ clean ]"

- Open "Win Me Free Build Environment" terminal from ddk
- Locate project files
- Enter:
	- glide3x gendate
	- glide3x minihwc
	- glide3x dll

	- glide3x all ... will run all 3

You are done, outputed .dll is located in: 3dfx_glide\h5\bin
	
![glide_build_complete](https://user-images.githubusercontent.com/62139759/226143765-1c165cde-eedb-4f91-94e1-30fb375ea636.png)
	
Same procedure for glide2x, just use glide2x.bat

