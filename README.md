# dac8532
code for interfacing with DAC8532 on High-Precision_AD/DA_Board

## Setup
Get and install bcm2835 library (C library for Broadcom BCM 2835 as used in Raspberry Pi):

```bash
$ mkdir ~/bcm2835; cd ~/bcm2835
$ wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.55.tar.gz
$ tar xvf bcm2835-1.55.tar.gz
$ cd bcm2835-1.55
$ ./configure
$ make
$ sudo make install
```

Compile and run script (set channel 0 to 2.5 V):

```bash
$ make
$ sudo ./dac8532_set 0 2.5
```

Note the `sudo`, the bcm2835 library requires access to `/dev/memgpio` which requires root access.
