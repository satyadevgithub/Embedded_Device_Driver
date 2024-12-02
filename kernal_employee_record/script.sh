#!/bin/bash

sudo insmod employee_record.ko
gcc app.c
input="1 50 2 3 4"
echo "$input" | sudo ./a.out
sudo rmmod employee_record 

