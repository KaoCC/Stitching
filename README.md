# Stitching
Image stitching !!!


## System requirements and Dependencies


- [OpenCV 3.2](http://opencv.org/)

The following compilers are supported:

- Visual Studio 2015 or 2017


## Build

### Windows

- Create Visual Studio 2017 Solution

`./Tool/premake/win/premake5.exe vs2017`

- After that, open the visual studio solution and build it !


### Linux  (Experimental !!!)

In theory, The premake configuration can also generate Makefiles for GCC as well, but I have not tried it yet for this project.

- Create the Makefile

`./Tool/premake/linux64/premake gmake`

- Build it !

`make config=release_x64`


## Usage

`./ST (BaseDirPath) (FileListName)`

Note: The default base path is "../Resource/input_image/" and the default name of the file list is "list.txt". You do not have to type it explicitly.






