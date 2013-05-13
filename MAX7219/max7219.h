/*
 * max7219.h
 * Author: Thomas Müller
 * Copyright 2013 Thomas Müller <tmuel at gmx.net>
 * $Id$
 */


#ifndef MAX7219_H_
#define MAX7219_H_

/******************************************************************************
***   Include                                                               ***
******************************************************************************/

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "SPI.h"

/******************************************************************************
***   Defines and Constants                                                 ***
******************************************************************************/

//MAX7219/MAX7221's memory register addresses:
// See Table 2 on page 7 in the Datasheet
const char NoOp        = 0x00;
const char Digit0      = 0x01;
const char Digit1      = 0x02;
const char Digit2      = 0x03;
const char Digit3      = 0x04;
const char Digit4      = 0x05;
const char Digit5      = 0x06;
const char Digit6      = 0x07;
const char Digit7      = 0x08;
const char DecodeMode  = 0x09;
const char Intensity   = 0x0A;
const char ScanLimit   = 0x0B;
const char ShutDown    = 0x0C;
const char DisplayTest = 0x0F;


#endif /* MAX7219_H_ */
