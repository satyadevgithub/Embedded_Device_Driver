#!/bin/bash

sudo insmod list_ioctl_alpha.ko
gcc app.c
input="a b c d e f g h i j k l m n o p Q r s t u v w x y z q"
echo "$input" | sudo ./a.out
sudo rmmod list_ioctl_alpha 
