# Introduction

MATOG Auto-Tuning on GPUs is a tool to automatically optimize performance of NVIDIA CUDA code. The aim is to optimize arbitrary CUDA applications with as few code adaptions and limitations as possible. MATOG is written in C++, platform independent and does require only a few external tools. For more details, please visit: www.matog.org.

MATOG is no longer maintained and might not work with newer program libraries or hardware.

# Building MATOG

To download and build:

	$ git clone https://github.com/mergian/matog.git
	$ cd matog
	$ mkdir build
	$ mkdir install
	$ cd build
	$ cmake "-DCMAKE_INSTALL_PREFIX:PATH=$BASEDIR/install" ..
	$ make -j install
	
System requirements:
	* Intel TBB 4.4 (www.threadingbuildingblocks.org)
	* CUDA 6.0
	* GCC 4.8 or Visual Studio 2012
	
# Documentation
For further information, please refer to the docs/MATOGv5.0.html documentation.
