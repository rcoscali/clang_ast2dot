#!/bin/bash

cd build
ninja -v all
./test_parser
