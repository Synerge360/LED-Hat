#ifndef ADCMAIN_H
#define ADCMAIN_H

//Clock Register
#define CLOCKBASE (0x40000000)
#define HFCLKSTART (0x000)
#define EVENTHFCLKSTART (0x100)

//ADC Setup Register
#define ADCBASE (0x40007000)
#define ADCCONFIG (0x518)
#define ADCPSELP (0x510)
#define ADCPSELN (0x514)
#define ADCRESOLUTION (0x5F0)
#define ADCMAXCNT (0x630)
#define ADCPTR (0x62C)
#define ADCSAMPLERATE (0x5F8)
#define ADCENABLE (0x500)

//ADC Calibrate Register
#define ADCTASKCALIBRATE (0x00C)
#define ADCCALIBRATEDONE (0x110)
#define ADCSTATUS (0x400)

//ADC Sample Register
#define ADCSTART (0x000)
#define ADCEVENTSTART (0x100)
#define ADCSAMPLE (0x004)
#define ADCEVENTEND (0x104)
#define ADCSTOP (0x008)
#define ADCEVENTSTOP (0x114)

#endif