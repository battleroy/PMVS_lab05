sudo rmmod sysdev
make
sudo insmod ./sysdev.ko
sudo chmod 666 /sys/kernel/dev_timer/timer
echo "4" > /sys/kernel/dev_timer/timer