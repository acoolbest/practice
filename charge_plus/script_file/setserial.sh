#!/bin/sh 
#===================== 
#set com port
#===================== 
#setserial /dev/ttyS0 -a; setserial /dev/ttyS1 -a; setserial /dev/ttyS2 -a; setserial /dev/ttyS3 -a; setserial /dev/ttyS4 -a; setserial /dev/ttyS5 -a

setserial /dev/ttyS2 port 0x3E8 irq 0 uart 16550a baud_base 115200
setserial /dev/ttyS3 port 0x2E8 irq 0 uart 16550a baud_base 115200 
setserial /dev/ttyS4 port 0x2F0 irq 0 uart 16550a baud_base 115200 
setserial /dev/ttyS5 port 0x2E0 irq 0 uart 16550a baud_base 115200 
