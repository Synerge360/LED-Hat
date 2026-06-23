/* BT.h Header file for Bluetooth Module

**TRANSMISSION**
1) Trigger the STARTTX task
2) Write bytes to TXD register
3) Check TXDRDY event if TXD register is ready
4) Write new byte to TXD register
5) Trigger the STOPTX task if TXDRDY is ready

**RECEPTION**
* Stores 6 RXD bytes before data is overwritten
1) Trigger the STARTRX task
2) Check RXDRDY event if new byte is moved into RXD register
3) Clear RXDRDY event if new byte is moved into RXD register
3) Extract bytes by reading RXD register (FIFO)

**EASYDMA**
* TXD.PTR and RXD.PTR must point to Data RAM region
* .PTR and .MAXCNT registers are double-buffered
* ENDRX/ENDTX indicates EasyDMA has finished accessing RX/TX buffer in RAM
*/

#ifndef _LEDPROJECT_BT_H_INCLUDED
#define _LEDPROJECT_BT_H_INCLUDED

#include <LEDPROJECT_NVIC.h>

// CLOCK registers
#define NRF_CLOCK_BASE_ADDRESS  (0x40000000UL)
#define NRF_CLOCK_TASKS_HFCLKSTART_OFFSET  (0x000)
#define NRF_CLOCK_EVENTS_HFCLKSTARTED_OFFSET  (0x100)

// UARTE register
#define UARTEBASE 0x40002000
#define BAUDRATE (0x524)
#define BAUD9600 (0x00275000)
#define PSELTXD (0x50C)
#define PSELRXD (0x514)
#define CONFIGURE (0x56C)
#define ENABLEUARTE (0x500)
#define INTENSETS (0x304)

// UARTE TXD register
#define TXDMAXCNT (0x548)
#define TXDPTR (0x544)
#define STARTTXD (0x008)
#define EVENTSENDTX (0x120)
#define TASKSSTOPTX (0x00C)
#define EVENTSTXSTOPPED (0x158)

// UARTE RXD register
#define RXDMAXCNT (0x538)
#define RXDPTR (0x534)
#define STARTRXD (0x000)
#define EVENTSRXDRDY (0x108)
#define EVENTSENDRX (0x110)
#define TASKSSTOPRX (0x004)
#define TASKSSTOPRX (0x004)

#endif