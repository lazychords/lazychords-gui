![logo](logo.png)

# Gui of the LazyChords project
[![Build Status](https://travis-ci.org/lazychords/lazychords-gui.svg?branch=master)](https://travis-ci.org/lazychords/lazychords-gui)

This is provides a userfrendly inteface to the Lazychords project.

## Goal of the project

This project aims at providing a tool to allow musicians to find suitable accompaniments for their melodies. Currently, we focus on providing a good Chord Progression that fits the melody, and that sounds as well as possible.

This can be useful independently of the user's musical skills : 
* For beginners, it allows a quick generation of an a ready-to-play accompaniement without any theoretical background about composition.
* For advanced musicians, the software can be a source of interesting/unusual ideas, and speed up the overall composition process by providing a strong basis to start with.


## How does it work ?

The software relies on a dynamic algorithm, that tries to apply expertly designed music rules to the melody. The rules allow to score a given chord progression with respect to a melody, and the algorithm's task is to find the chord progression that maximizes this score.

## What does it look like ?

The interface consist of a rendering of the melody score, the chord progression generated, and a toolbox to playback the melody and the chord progression simultaneously.

![gui](GUI.jpg)


## Build instruction

To build the project, you need a recent C++ compiler (this project uses C++11, supported since gcc4.8), cmake, make, Qt5 and the Boost library.

```
git clone https://github.com/lazychords/lazychords-gui.git
cd lazychords-gui
sudo apt-get install cmake make g++ libboost-dev  libboost−filesystem−dev qt5-default libqt5webkit5
git submodule update --init --recursive
mkdir build && cd build
cmake ..
make -j
```

On older systems, like Ubuntu 12.04, you can use the following instructions, derived from the build process on travis : 
```
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo add-apt-repository -y "deb http://archive.ubuntu.com/ubuntu trusty universe"
sudo add-apt-repository -y "deb http://archive.ubuntu.com/ubuntu trusty main"
sudo apt-get update -qq
sudo apt-get install -qq git libboost-dev libboost-test-dev libboost-program-options-dev cmake make
sudo apt-get install qdbus qmlscene qt5-default qt5-qmake qtbase5-dev-tools qtchooser qtdeclarative5-dev xbitmaps xterm libqt5svg5-dev qttools5-dev qtscript5-dev qtdeclarative5-folderlistmodel-plugin qtdeclarative5-controls-plugin -y
sudo apt-get install -qq qtbase5-dev qtdeclarative5-dev libqt5webkit5-dev
sudo apt-get install -qq qt5-default qttools5-dev-tools
sudo apt-get update; sudo apt-get install gcc-4.8 g++-4.8
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 20
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 20
sudo update-alternatives --config gcc
sudo update-alternatives --config g++
git clone https://github.com/lazychords/lazychords-gui.git
cd lazychords-gui
git submodule update --init --recursive
mkdir build && cd build
cmake ..
make -j
```
