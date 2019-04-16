#!/bin/bash

sudo apt-get install libgoogle-glog-dev
sudo apt-get install libevent-dev
sudo apt-get install libgtest-dev
sudo apt-get install libboost-all-dev
sudo apt-get install libgflags-dev

sudo export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./lib/evpp_lib/
