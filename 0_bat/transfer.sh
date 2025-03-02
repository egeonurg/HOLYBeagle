#!/bin/bash

cd ..
cd 5_cross
cd build

# Get the current timestamp
timestamp=$(date +"%Y%m%d_%H%M%S")

# Define the new filename with the timestamp
new_filename="tcp_app_${timestamp}"

# Copy the file with the new filename
scp tcp_app debian@192.168.7.2:/home/debian/${new_filename}