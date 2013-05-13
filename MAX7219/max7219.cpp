/*
 * max7219.cpp
 * Author: Thomas Müller
 * Copyright 2013 Thomas Müller <tmuel at gmx.net>
 * $Id$
 */


/******************************************************************************
***   Include                                                               ***
******************************************************************************/

#include <stdio.h>
#include <iostream>
#include <cstring>
//#include "max7219.h"
#include <wiringPi.h>
#include <wiringPiSPI.h>


using namespace std;

int DEBUG_ACTIVE = 0; // Global debug variable

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


const char numOfDevices = 2;

/******************************************************************************
***   Function Definitions                                                  ***
******************************************************************************/

void setup();
void loop();

/******************************************************************************
***   Global Variables                                                      ***
******************************************************************************/



/******************************************************************************
***   Class: RasPiSPI                                                       ***
******************************************************************************/

class RasPiSPI
{
	private:
		int SpiFd; // File descriptor of spi port
		
		char *TxBuffer;
		char *RxBuffer;
		
		int TxBufferIndex;
		int RxBufferIndex;
				
	public:
		RasPiSPI(); // Konstruktor
		~RasPiSPI(); // Destruktor
		
		void begin() { begin(0, 1000000); } // default use channel 0 and 1MHz clock speed
		void begin(int, int);
		
		void transfer(char);
		void endTransfer();
};

RasPiSPI::RasPiSPI() // CONSTRUCTOR
{
	if(DEBUG_ACTIVE > 0) {cout << "RasPiSPI Konstruktor" << endl;}
	
	TxBuffer = new char[1024]; // Buffer for TxData
	RxBuffer = new char[1024]; // Buffer for RxData
	
	TxBufferIndex = 0;
	RxBufferIndex = 0;
}
RasPiSPI::~RasPiSPI() // DESTRUCTOR
{
	if(DEBUG_ACTIVE > 0) {cout << "RasPiSPI Destruktor" << endl;}
	
	delete[] TxBuffer;
	delete[] RxBuffer;
	
	close(SpiFd); // Close SPI port
}

RasPiSPI SPI; // Create class SPI

void RasPiSPI::begin(int channel, int speed)
{
   if ((SpiFd = wiringPiSPISetup (channel, speed)) < 0)
	{	// Open port for reading and writing
		cout << "Failed to open SPI port " << channel << "! Please try with sudo" << endl;
	}
	if(DEBUG_ACTIVE > 0) {cout << "Filehandle opened" << endl;}
}

void RasPiSPI::transfer(char c)
{
	TxBuffer[TxBufferIndex] = c;
	TxBufferIndex++;
}
void RasPiSPI::endTransfer()
{
	int temp = write(SpiFd, TxBuffer, TxBufferIndex); // Write the data from TxBuffer to the SPI bus...
	if(DEBUG_ACTIVE > 1)
	{ // Debug level 2
		cout << "Written: " << temp << " Index: " << TxBufferIndex << " Buffer: ";
		for(int i = 0; i < TxBufferIndex; i++)
		{
			cout << int(TxBuffer[i]) << " ";
		}
		cout << endl;
	}
	TxBufferIndex = 0; // ...and reset the index
}

/*
 * End of class RasPiSPI
 */

/******************************************************************************
***   Main                                                                  ***
******************************************************************************/
int main(int argc, char **argv)
{
	int initOnly = 0;
	
	cout << argc << '\n'; 
	for(int i=0;i<argc;i++) 
	{
		if(DEBUG_ACTIVE > 0) {cout << argv[i] << '\n'; }
		
		if(strcmp (argv[i],"-init") == 0)
		{
			initOnly = 1;
			cout << "Attention only initialisation will be done" << endl;
		}
		if(strcmp (argv[i],"-DEBUG") == 0)
		{
			DEBUG_ACTIVE = 1;
			cout << "!!! DEBUG ACTIVE !!!" << endl;
		}
	}

	if(DEBUG_ACTIVE > 0) {cout << "Program Started" << endl;}
	if(DEBUG_ACTIVE > 0) {cout << "Begin Setup" << endl;}
	setup();
	if(DEBUG_ACTIVE > 0) {cout << "Setup done" << endl;}
	
	if(initOnly != 0) {return 0;}
	
	while(1)
	{
		if(DEBUG_ACTIVE > 0) {cout << "Begin Loop" << endl;}
		loop();
		if(DEBUG_ACTIVE > 0) {cout << "End Loop" << endl;}
	}

	return 0;
}



// Writes data to the selected device or does broadcast if device number is 255
void SetData(char adr, char data, char device)
{
	// Count from top to bottom because first data which is sent is for the last device in the chain
	for (int i = numOfDevices; i > 0; i--)
	{
		if ((i == device) || (device == 255))
		{
			SPI.transfer(adr);
			SPI.transfer(data);
		}
		else // if its not the selected device send the noop command
		{
			SPI.transfer(NoOp);
			SPI.transfer(0);
		}
	}
	SPI.endTransfer();

	delay(1);
}

// Writes the same data to all devices
void SetData(char adr, char data) { SetData(adr, data, 255); } // write to all devices (255 = Broadcast) 

void SetShutDown(char Mode) { SetData(ShutDown, !Mode); }
void SetScanLimit(char Digits) { SetData(ScanLimit, Digits); }
void SetIntensity(char intense) { SetData(Intensity, intense); }
void SetDecodeMode(char Mode) { SetData(DecodeMode, Mode); }

/******************************************************************************
***   Setup                                                                 ***
******************************************************************************/

void setup()
{
	// The MAX7219 has officially no SPI / Microwire support like the MAX7221 but the
	// serial interface is more or less the same like a SPI connection

	SPI.begin();
  
	// Disable the decode mode because at the moment i dont use 7-Segment displays
	if(DEBUG_ACTIVE > 0) {cout << "SetDecodeMode(false);" << endl;}
	SetDecodeMode(false);
	// Set the number of digits; start to count at 0
	if(DEBUG_ACTIVE > 0) {cout << "SetScanLimit(7);" << endl;}
	SetScanLimit(7);
	// Set the intensity between 0 and 15. Attention 0 is not off!
	if(DEBUG_ACTIVE > 0) {cout << "SetIntensity(5);" << endl;}
	SetIntensity(5);
	// Disable shutdown mode
	if(DEBUG_ACTIVE > 0) {cout << "SetShutDown(false);" << endl;}
	SetShutDown(false);

	if(DEBUG_ACTIVE > 0) {cout << "Write Patterns" << endl;}

	// Write some patterns
	SetData(Digit0, 0b10000000, 1);
	SetData(Digit1, 0b01000000, 1);
	SetData(Digit2, 0b00100000, 1);
	SetData(Digit3, 0b00010000, 1);
	SetData(Digit4, 0b00001000, 1);
	SetData(Digit5, 0b00000100, 1);
	SetData(Digit6, 0b00000010, 1);
	SetData(Digit7, 0b00000001, 1);

	SetData(Digit0, 0b00000001, 2);
	SetData(Digit1, 0b00000010, 2);
	SetData(Digit2, 0b00000100, 2);
	SetData(Digit3, 0b00001000, 2);
	SetData(Digit4, 0b00010000, 2);
	SetData(Digit5, 0b00100000, 2);
	SetData(Digit6, 0b01000000, 2);
	SetData(Digit7, 0b10000000, 2);

	if(DEBUG_ACTIVE > 0) {cout << "Delay 1000" << endl;}
	delay(1000);

}

/******************************************************************************
***   Loop                                                                  ***
******************************************************************************/

void loop()
{
  
  //you may know this from space invaders
  unsigned int rowBuffer[]=
  {
    0b0010000010000000,
    0b0001000100000000,
    0b0011111110000000,
    0b0110111011000000,
    0b1111111111100000,
    0b1011111110100000,
    0b1010000010100000,
    0b0001101100000000
  };

   if(DEBUG_ACTIVE > 0) {cout << "Start with space invader animation" << endl;}

  while(1)
  {
    for (int shiftCounter = 0; 15 >= shiftCounter; shiftCounter++)
    {
      for (int rowCounter = 0; 7 >= rowCounter; rowCounter++)
      {
        // roll the 16bits...
        // The information how to roll is from http://arduino.cc/forum/index.php?topic=124188.0 
        rowBuffer[rowCounter] = ((rowBuffer[rowCounter] & 0x8000)?0x01:0x00) | (rowBuffer[rowCounter] << 1);
        
        // ...and then write them to the two devices
        SetData(rowCounter+1, char(rowBuffer[rowCounter]), 1);
        SetData(rowCounter+1, char(rowBuffer[rowCounter]>>8), 2);
      }    
      delay(100);
    }
  }
}
