#*Install cross compiler:*
sudo apt-get install gcc-arm-linux-eabihf

#*Install kernel support packages:*
sudo apt-get install build-essential git libmpc-dev libncurses5-dev bison flex gettext
sudo apt-get install lz4 lzop lzma
sudo apt-get install libssl-dev sshfs
sudo apt-get install u-boot-tools

#*Downloading kernel source code*
git clone https://github.com/beagleboard/linux.git -b v5.10.168-ti-r71 --depth=1

#*Enter the linux directory*
cd linux

#*Configure the kernel*
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bb.org_defconfig

#*Compile Linux kernel for BBB*
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- uImage dtbs LOADADDR=0x82000000 -j `nproc`

#*Compile modules for BBB*
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- modules -j `nproc`

#*Clean up if you made a mistake*
$ make mrproper

#*make project*
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf-

#*clean project*
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- clean

#*project send to beaglebone*
scp -r kernal_employee_record/ debian@192.168.7.2:~/.

#*make excutable script.sh*
chmod +x script.sh
