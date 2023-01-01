#!/bin/bash

cd src
qmake -makefile
qmake
make
cd ..
