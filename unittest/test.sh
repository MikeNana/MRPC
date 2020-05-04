#!/bin/bash
#by author qianxinkui


g++ -c xxx1.cc
g++ -c xxx2.cc
g++ xxx1.o xxx2.o -o xxx -lgtest -lpthread

