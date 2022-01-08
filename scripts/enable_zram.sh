#!/usr/bin/env bash

sudo modprobe zram
echo lz4 | sudo tee /sys/block/zram0/comp_algorithm
echo 32G | sudo tee /sys/block/zram0/disksize
sudo mkswap --label zram0 /dev/zram0
sudo swapon --priority 100 /dev/zram0
