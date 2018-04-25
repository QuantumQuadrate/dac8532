/*
 * dac8532_set.c:
 *	Very simple program to write the DAC8532 serial port. Expects
 *	the port to be looped back to itself

 * Matt Ebert 04/2018
/*
             define from bcm2835.h                       define from Board
DVK511
                 3.3V | | 5V               ->                 3.3V | | 5V
    RPI_V2_GPIO_P1_03 | | 5V               ->                  SDA | | 5V
    RPI_V2_GPIO_P1_05 | | GND              ->                  SCL | | GND
       RPI_GPIO_P1_07 | | RPI_GPIO_P1_08   ->                  IO7 | | TX
                  GND | | RPI_GPIO_P1_10   ->                  GND | | RX
       RPI_GPIO_P1_11 | | RPI_GPIO_P1_12   ->                  IO0 | | IO1
    RPI_V2_GPIO_P1_13 | | GND              ->                  IO2 | | GND
       RPI_GPIO_P1_15 | | RPI_GPIO_P1_16   ->                  IO3 | | IO4
                  VCC | | RPI_GPIO_P1_18   ->                  VCC | | IO5
       RPI_GPIO_P1_19 | | GND              ->                 MOSI | | GND
       RPI_GPIO_P1_21 | | RPI_GPIO_P1_22   ->                 MISO | | IO6
       RPI_GPIO_P1_23 | | RPI_GPIO_P1_24   ->                  SCK | | CE0
                  GND | | RPI_GPIO_P1_26   ->                  GND | | CE1

::if your raspberry Pi is version 1 or rev 1 or rev A
RPI_V2_GPIO_P1_03->RPI_GPIO_P1_03
RPI_V2_GPIO_P1_05->RPI_GPIO_P1_05
RPI_V2_GPIO_P1_13->RPI_GPIO_P1_13
::
*/

#include <bcm2835.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>

//CS      -----   SPICS
//DIN     -----   MOSI
//DOUT  -----   MISO
//SCLK   -----   SCLK
//DRDY  -----   ctl_IO     data  starting
//RST     -----   ctl_IO     reset


#define	SPICS	RPI_GPIO_P1_16	//P4

#define CS_1() bcm2835_gpio_write(SPICS,HIGH)
#define CS_0()  bcm2835_gpio_write(SPICS,LOW)
#define uint8_t unsigned char
#define uint16_t unsigned short

#define VREF        5.0   // voltage ref used
#define DAC_BITS    16    // 16b DAC word
// output cannot exceed vref (2**16- 1b)/2**16, change if lower value is necessary
#define MAX_V       VREF-VREF/(1<<(DAC_BITS-1))
#define MAX_CHANNEL 2     // 2 DAC channels
#define CHANNEL_0   0x30  // dac0, ch A
#define CHANNEL_1   0x34  // dac1, ch B

void  bsp_DelayUS(uint64_t micros);
void Write_DAC8532(uint8_t channel, uint16_t Data);
uint16_t Voltage_Convert(float voltage);


void  bsp_DelayUS(uint64_t micros){
  bcm2835_delayMicroseconds (micros);
}


void Write_DAC8532(uint8_t channel, uint16_t Data){
  uint8_t i;
  CS_1() ;
  CS_0() ;
  bcm2835_spi_transfer(channel);
  bcm2835_spi_transfer((Data>>8));
  bcm2835_spi_transfer((Data&0xff));
  CS_1() ;
}


uint16_t Voltage_Convert(float voltage){
  uint16_t _D_;
  _D_ = (uint16_t)round(65536 * voltage / VREF);
  return _D_;
}


int  main(int argc, char **argv){
  // process and sanitize inputs
  if (argc != 3){
    printf("Expecting 2 arguments: channel voltage\n");
    return 1;
  }
  errno = 0;  // catch errors, from errno.h
  int channel = atoi(argv[1]);
  if (errno != 0){
    printf("errno %d encoutered converting %s to `int`\n", errno, argv[1]);
    return 1;
  }
  float voltage = atof(argv[2]);
  if (errno != 0){
    printf("errno %d encoutered converting %s to `float`\n", errno, argv[2]);
    return 1;
  }
  if (channel < 0 || channel > MAX_CHANNEL-1){
    printf("channel designation: `%d` exceeds MAX_CHANNEL spec: `%d`.\n", channel, MAX_CHANNEL);
    return 1;
  }
  if (voltage > MAX_V){
    printf("voltage designation: `%f` exceeds MAX_VOLTAGE spec: `%f`.\n", voltage, MAX_V);
    voltage = MAX_V;
  } else if (voltage < 0){
    printf("voltage designation: `%f` exceeds MIN_VOLTAGE spec: `%f`.\n", voltage, 0);
    voltage = 0;
  }

  // set up the spi
  if (!bcm2835_init()){
    return 1;
  }
  bcm2835_spi_begin();
  bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_LSBFIRST );      // The default
  bcm2835_spi_setDataMode(BCM2835_SPI_MODE1);                   // The default;
  bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_1024); // The default
  bcm2835_gpio_fsel(SPICS, BCM2835_GPIO_FSEL_OUTP);//
  bcm2835_gpio_write(SPICS, HIGH);

  //Write channel buffer
  Write_DAC8532(
    ((char[]){CHANNEL_0, CHANNEL_1})[channel],
    Voltage_Convert(voltage)
  );

  // close down
  bcm2835_spi_end();
  bcm2835_close();
  return 0;
}
