#ifndef _MCP23S18_H
#define _MCP23S18_H

#define MCP23S18_READ 0b01000001
#define MCP23S18_WRITE 0b01000000

#define MSG_ONE_BYTE_LEN 3
#define MSG_TWO_BYTE_LEN 4

#define ENABLE_ALL_BITS 0b11111111
#define DISABLE_ALL_BITS 0b00000000

// Address of registers when IOCON.BANK == 0
#define IODIRA 0x00
#define IODIRB 0x01
#define IPOLA 0x02
#define IPOLB 0x03
#define GPINTENA 0x04
#define GPINTENB 0x05
#define DEFVALA 0x06
#define DEFVALB 0x07
#define INTCONA 0x08
#define INTCONB 0x09
#define IOCON 0x0A
#define GPPUA 0x0C
#define GPPUB 0x0D
#define INTFA 0x0E
#define INTFB 0x0F
#define INTCAPA 0x10
#define INTCAPB 0x11
#define GPIOA 0x12
#define GPIOB 0x13
#define OLATA 0x14
#define OLATB 0x15

#define IOCON_BANK_SHIFT 7
#define IOCON_MIRROR_SHIFT 6
#define IOCON_SEQOP_SHIFT 5
#define IOCON_ODR_SHIFT 2
#define IOCON_INTPOL_SHIFT 1
#define IOCON_INTCC_SHIFT 0

#endif // _MCP23S18_H