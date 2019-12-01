#!bin/bash
make
sudo mount /dev/loop0p1 /mnt
sudo cp ddr.nds /mnt/ddr.nds
sudo umount /mnt
