#include <nrf.h>

char hello_world[] = "Hello World, How is it going?\n";
//char rxbuffer[sizeof(hello_world)];
char rxbuffer[255];

void setup() {

  /*Start HF crystal for improved clock tolerance*/
#define NRF_CLOCK_BASE_ADDRESS  (0x40000000UL)
#define NRF_CLOCK_TASKS_HFCLKSTART_OFFSET  (0x000)
#define NRF_CLOCK_EVENTS_HFCLKSTARTED_OFFSET  (0x100)
  if (*((volatile uint32_t *)((uint8_t *)NRF_CLOCK_BASE_ADDRESS + (uint32_t)NRF_CLOCK_EVENTS_HFCLKSTARTED_OFFSET)) == 0) {
    *((volatile uint32_t *)((uint8_t *)NRF_CLOCK_BASE_ADDRESS + (uint32_t)NRF_CLOCK_TASKS_HFCLKSTART_OFFSET)) = 1;
    while (*((volatile uint32_t *)((uint8_t *)NRF_CLOCK_BASE_ADDRESS + (uint32_t)NRF_CLOCK_EVENTS_HFCLKSTARTED_OFFSET)) == 0) { }
  }

  /*Set up UARTE*/
  //Set 9600 baud rate
  NRF_UARTE0->BAUDRATE = (0x00275000UL);
  //Select TX and RX pins
  NRF_UARTE0->PSEL.TXD = (0x7FFFFFE3UL);
  NRF_UARTE0->PSEL.RXD = (0x7FFFFFEAUL);
  //Configure UARTE
  NRF_UARTE0->CONFIG = 0;
  //Enable the UART (starts using the TX/RX pins)
  NRF_UARTE0->ENABLE = (8UL);

  Serial.begin(9600);
  while (!Serial);
}

void loop() {

  /*START TX*/
  //Configure transmit buffer and start transmission
  NRF_UARTE0->TXD.MAXCNT = sizeof(hello_world);
  NRF_UARTE0->TXD.PTR = (uint32_t)hello_world;
  NRF_UARTE0->TASKS_STARTTX = 1;

//  int dummy = NRF_UARTE0->TASKS_STARTTX;
//  
//  while (NRF_UARTE0->EVENTS_TXSTARTED == 0);

  delay(100);

  //Wait until the transfer is complete
  while (NRF_UARTE0->EVENTS_ENDTX == 0);

  //Stop the UART TX
  NRF_UARTE0->TASKS_STOPTX = 1;

  //Wait until we receive the stopped event
  while (NRF_UARTE0->EVENTS_TXSTOPPED == 0);

  /*START RX*/
  //Configure receive buffer and start reception
  NRF_UARTE0->RXD.MAXCNT = sizeof(rxbuffer);
  NRF_UARTE0->RXD.PTR = (uint32_t)rxbuffer;

  while (1) {

    NRF_UARTE0->TASKS_STARTRX = 1;

    delay(100);

    while(!NRF_UARTE0->EVENTS_RXDRDY);

    //Wait until receive is complete
    while (NRF_UARTE0->EVENTS_ENDRX == 0);

    //Stop the UART RX
    NRF_UARTE0->TASKS_STOPRX = 1;
    NRF_UARTE0->EVENTS_RXDRDY = 0;

    Serial.write(rxbuffer);
    memset(rxbuffer, 0, 10);
  }
}