/*
 * Project Name: Dixie-Uhr  
 * File: main.cpp
 * Description: Steuerung der Dixie-Uhr nach Zeitschrift Make von Matthias Helneder mit einem Raspberry Pi Zero W
 * Author: Kersten Tams.
 * Created: February 2023
 * Description: based on projekt ST7735_TFT_RPI from Gavin Lyons URL: https://github.com/gavinlyonsrepo/ST7735_TFT_RPI
 * See URL for full details.
 * URL: https://github.com/ktams/DixieUhr
 */

// Section ::  libraries 
#include <bcm2835.h> // for SPI GPIO and delays.
#include <time.h> 
#include <iostream>
#include <string>
#include "ST7735_TFT.h"

using namespace std;

// Section :: Defines   
#define TEST_DELAY1 1000

// Section :: Globals 
ST7735_TFT myTFT;

//  Section ::  Function Headers 

int8_t Setup(int8_t cs);  // setup + user options
void IniDisplay(int8_t disp);
void writeCommand(uint8_t spicmdbyte, int8_t cs);
void writeData(uint8_t spidatabyte, int8_t cs);
void spiWriteSoftware(uint8_t spidata, int8_t cs);
void spiWriteDataBuffer(uint8_t* spidata, uint32_t len, int8_t cs);
void Rcmd1(int8_t cs);
void Rcmd2red(int8_t cs);
void Rcmd3(int8_t cs);
void Dixie(void); // Die Uhr
int64_t getTime(); 
uint8_t* loadImage(char* name); 

//  Section ::  MAIN loop

int main(void) 
{
	if(!Setup(8)) {	// initialisiert das erste Display (0) und resettet die anderen mit
		std::cout << "Problems with the Setup" << std::endl;
		return -1;
	}
	myTFT.TFTsetRotation(TFT_Degress_180);
	for(uint8_t i = 1; i < 5; i++) {
		IniDisplay(i);	//Initialisiert das zweite bis fünfte Display (1..4)
	}
	Dixie();
	return 0;
}
// *** End OF MAIN **


//  Section ::  Function Space 

void SetGPIO(int8_t cs) {	// set the chip select of the wanted TFT
	int8_t RST_TFT = 25;
	int8_t DC_TFT = 24;
	int8_t SCLK_TFT = 5; 
	int8_t SDIN_TFT = 6; 
	int8_t CS_TFT = cs ; 
	myTFT.TFTSetupGPIO(RST_TFT, DC_TFT, CS_TFT, SCLK_TFT, SDIN_TFT);
}

int8_t Setup(int8_t cs)
{
	TFT_MILLISEC_DELAY(TEST_DELAY1);
	std::cout << "Initialize TFT 1" << std::endl;
	if(!bcm2835_init())
	{
		std::cout << "Error : Problem with init bcm2835 library" << std::endl;
		return -1;
	}
	uint8_t OFFSET_COL = 0;  // 2, These offsets can be adjusted for any issues->
	uint8_t OFFSET_ROW = 0; // 3, with manufacture tolerance/defects
	uint16_t TFT_WIDTH = 128;// Screen width in pixels
	uint16_t TFT_HEIGHT = 160; // Screen height in pixels
	SetGPIO(cs);
	myTFT.TFTInitScreenSize(OFFSET_COL, OFFSET_ROW , TFT_WIDTH , TFT_HEIGHT);
	myTFT.TFTsetRotation(TFT_Degress_180);
	if(!myTFT.TFTInitPCBType(TFT_ST7735S_Black, 0, 0))return -1;
	return 0;
}

void writeCommand(uint8_t spicmdbyte, int8_t cs) {
	int8_t DC_TFT = 24;
	bcm2835_gpio_write(DC_TFT, LOW);
	bcm2835_gpio_write(cs, LOW);
	spiWriteSoftware(spicmdbyte, cs);
	bcm2835_gpio_write(cs, HIGH);
}

void writeData(uint8_t spidatabyte, int8_t cs) {
	int8_t DC_TFT = 24;
	bcm2835_gpio_write(DC_TFT, HIGH);
	bcm2835_gpio_write(cs, LOW);
	spiWriteSoftware(spidatabyte, cs);
	bcm2835_gpio_write(cs, HIGH);
}

void spiWriteSoftware(uint8_t spidata, int8_t cs) {
	int8_t SCLK_TFT = 5; 
	int8_t SDIN_TFT = 6; 
	uint8_t i;
	for (i = 0; i < 8; i++) {
		bcm2835_gpio_write(SDIN_TFT, LOW);
		if (spidata & 0x80) {
			bcm2835_gpio_write(SDIN_TFT, HIGH);
		}
		bcm2835_gpio_write(SCLK_TFT, HIGH);
		TFT_MICROSEC_DELAY(TFT_HIGHFREQ_DELAY);
		spidata <<= 1;
		bcm2835_gpio_write(SCLK_TFT, LOW);
		TFT_MICROSEC_DELAY(TFT_HIGHFREQ_DELAY);
	}
}

void spiWriteDataBuffer(uint8_t* spidata, uint32_t len, int8_t cs) {
	int8_t DC_TFT = 24;
	bcm2835_gpio_write(DC_TFT, HIGH);
	bcm2835_gpio_write(cs, LOW);
	for(uint32_t i=0; i<len; i++) {
		spiWriteSoftware(spidata[i], cs);
		}
	bcm2835_gpio_write(cs, HIGH);
}

void Rcmd1(int8_t cs)
{
	uint8_t seq1[] { 0x01, 0x2C, 0x2D };
	uint8_t seq3[] { 0xA2, 0x02, 0x84 }; 
	writeCommand(ST7735_SWRESET, cs);
	TFT_MILLISEC_DELAY(150);
	writeCommand(ST7735_SLPOUT, cs);
	TFT_MILLISEC_DELAY(500);
	writeCommand(ST7735_FRMCTR1, cs);
	
	spiWriteDataBuffer(seq1, sizeof(seq1), cs);
	writeCommand(ST7735_FRMCTR2, cs);
	spiWriteDataBuffer(seq1, sizeof(seq1), cs);
	writeCommand(ST7735_FRMCTR3, cs);
	spiWriteDataBuffer(seq1, sizeof(seq1), cs);
	spiWriteDataBuffer(seq1, sizeof(seq1), cs);
	writeCommand(ST7735_INVCTR, cs);
	writeData(0x07, cs);
	writeCommand(ST7735_PWCTR1, cs);
	spiWriteDataBuffer(seq3, sizeof(seq3), cs);
	writeCommand(ST7735_PWCTR2, cs);
	writeData(0xC5, cs);
	writeCommand(ST7735_PWCTR3, cs);
	writeData(0x0A, cs);
	writeData(0x00, cs);
	writeCommand(ST7735_PWCTR4, cs);
	writeData(0x8A, cs);
	writeData(0x2A, cs);
	writeCommand(ST7735_PWCTR5, cs);
	writeData(0x8A, cs);
	writeData(0xEE, cs);
	writeCommand(ST7735_VMCTR1, cs);
	writeData(0x0E, cs);
	writeCommand(ST7735_INVOFF, cs);
	writeCommand(ST7735_MADCTL, cs);
	writeData(0xC8, cs);
	writeCommand(ST7735_COLMOD, cs);
	writeData(0x05, cs);
}

void Rcmd2red(int8_t cs) {
	uint8_t seq1[] { 0x00, 0x00, 0x00, 0x7F };
	uint8_t seq2[] { 0x00, 0x00, 0x00, 0x9F };
	writeCommand(ST7735_CASET, cs);
	spiWriteDataBuffer(seq1, sizeof(seq1), cs);
	writeCommand(ST7735_RASET, cs);
	spiWriteDataBuffer(seq2, sizeof(seq2), cs);
}

void Rcmd3(int8_t cs) {
	writeCommand(ST7735_GMCTRP1,cs);
	uint8_t seq4[] {0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10}; 
	spiWriteDataBuffer(seq4, sizeof(seq4), cs);
	writeCommand(ST7735_GMCTRN1, cs);
	uint8_t seq5[] {0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10}; 
	spiWriteDataBuffer(seq5, sizeof(seq5), cs);
	writeCommand(ST7735_NORON, cs);
	TFT_MILLISEC_DELAY(10);
	writeCommand(ST7735_DISPON, cs);
	TFT_MILLISEC_DELAY(100);
}

void IniDisplay(int8_t disp)
{
	std::cout << "Initialize next TFT " << std::endl;
	uint8_t OFFSET_COL = 0;  //  These offsets can be adjusted for any issues->
	uint8_t OFFSET_ROW = 0; // with manufacture tolerance/defects
	uint16_t TFT_WIDTH = 128;// Screen width in pixels
	uint16_t TFT_HEIGHT = 160; // Screen height in pixels
	int8_t cs = 0;
	switch(disp) {
	case 1:
		cs = 7;
		break;

	case 2:
		cs = 9;
		break;

	case 3:
		cs = 10;
		break;

	case 4:
		cs = 11;
		break;

	default:
		cs = 8;
	}
	SetGPIO(cs);
	//*********************************************
	myTFT.TFTInitScreenSize(OFFSET_COL, OFFSET_ROW , TFT_WIDTH , TFT_HEIGHT);

	int8_t DC_TFT = 24;
	int8_t SCLK_TFT = 5; 
	int8_t SDIN_TFT = 6;
//	TFT_DC_SetLow;
	bcm2835_gpio_write(DC_TFT, LOW);
//	TFT_DC_SetDigitalOutput;
	bcm2835_gpio_fsel(DC_TFT, BCM2835_GPIO_FSEL_OUTP);
//	TFT_CS_SetHigh;
	bcm2835_gpio_write(cs, HIGH);
//	TFT_CS_SetDigitalOutput;
	bcm2835_gpio_fsel(cs, BCM2835_GPIO_FSEL_OUTP);
//	TFT_SCLK_SetLow;
	bcm2835_gpio_write(SCLK_TFT, LOW);
//	TFT_SDATA_SetLow;
	bcm2835_gpio_write(SDIN_TFT, LOW);
//	TFT_SCLK_SetDigitalOutput;
	bcm2835_gpio_fsel(SCLK_TFT, BCM2835_GPIO_FSEL_OUTP);
//	TFT_SDATA_SetDigitalOutput;
	bcm2835_gpio_fsel(SDIN_TFT, BCM2835_GPIO_FSEL_OUTP);
	Rcmd1(cs);
	Rcmd2red(cs);
	Rcmd3(cs);
	writeCommand(ST7735_MADCTL, cs);
	writeData(0xC0, cs);
	myTFT.TFTsetRotation(TFT_Degress_180);
}


void Dixie(void)
{
	time_t now;
	struct tm *timenow;
	uint8_t  Seconds, OldSeconds, date, button[3];
	const char value[10][36] = {"/home/dixie/DixieUhr/Pictures/0.bmp", 
								"/home/dixie/DixieUhr/Pictures/1.bmp", 
								"/home/dixie/DixieUhr/Pictures/2.bmp", 
								"/home/dixie/DixieUhr/Pictures/3.bmp", 
								"/home/dixie/DixieUhr/Pictures/4.bmp", 
								"/home/dixie/DixieUhr/Pictures/5.bmp", 
								"/home/dixie/DixieUhr/Pictures/6.bmp", 
								"/home/dixie/DixieUhr/Pictures/7.bmp", 
								"/home/dixie/DixieUhr/Pictures/8.bmp", 
								"/home/dixie/DixieUhr/Pictures/9.bmp"};
	char dat[] = "Datum";

	FILE *pFile ;
	string Digits[5];
	string str = "";
	size_t pixelSize = 3;
	uint8_t ui8DateTime = 0, ui8DotNoAnimation = 0;
	uint8_t* bmpBuffer = NULL;

	bmpBuffer = (uint8_t*)malloc((128 * 160) * pixelSize);
	if (bmpBuffer == NULL)
	{
		std::cout << "Error: MALLOC could not assign memory " << std::endl;
		return;
	}

	time(&now);
	timenow = localtime(&now); 
	Digits[0] = value[timenow->tm_hour / 10];
	Digits[1] = value[timenow->tm_hour % 10];
	Digits[2] = ":";//0xFF;
	Digits[3] = value[timenow->tm_min / 10];
	Digits[4] = value[timenow->tm_min % 10];

	Seconds = 0;
	myTFT.TFTFontNum(TFTFont_Default);
// Set GPIO 16, 20, 21 to input with pull up
	bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_36, BCM2835_GPIO_FSEL_INPT);	
	bcm2835_gpio_set_pud(RPI_BPLUS_GPIO_J8_36, BCM2835_GPIO_PUD_UP);
	bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_38, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_set_pud(RPI_BPLUS_GPIO_J8_38, BCM2835_GPIO_PUD_UP);
	bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_40, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_set_pud(RPI_BPLUS_GPIO_J8_40, BCM2835_GPIO_PUD_UP);
	while(1)
	{
		while(Seconds) {
			button[0] = bcm2835_gpio_lev(RPI_BPLUS_GPIO_J8_36);
			button[1] = bcm2835_gpio_lev(RPI_BPLUS_GPIO_J8_38);
			button[2] = bcm2835_gpio_lev(RPI_BPLUS_GPIO_J8_40);

			time(&now);
			timenow = localtime(&now); 
			Seconds = timenow->tm_sec;
			if(Seconds == OldSeconds) continue;
			OldSeconds = Seconds;
			if(!button[0]) {
				if(ui8DotNoAnimation) {
					ui8DotNoAnimation = 0;
				} else {
					ui8DotNoAnimation = 1;
					SetGPIO(9);					
					pFile = fopen("/home/dixie/DixieUhr/Pictures/DOT.bmp", "r"); 
					fseek(pFile, 128, 10); // Put file in Buffer
					fread(bmpBuffer, pixelSize, 128 * 160, pFile);
					fclose(pFile);
					myTFT.TFTdrawBitmap24(0, 0, bmpBuffer, 128, 160); 
				}
			}
			if(!button[1]) {
				if(!ui8DateTime) ui8DateTime = 1; else ui8DateTime = 0;
				break;
			}
			if(!ui8DateTime && !ui8DotNoAnimation) {
				switch (Seconds) {
				case 5:
				case 15:
				case 25:
					SetGPIO(9);					
					pFile = fopen("/home/dixie/DixieUhr/Pictures/DOT_u.bmp", "r"); 
					fseek(pFile, 128, 10); // Put file in Buffer
					fread(bmpBuffer, pixelSize, 128 * 160, pFile);
					fclose(pFile);
					myTFT.TFTdrawBitmap24(0, 0, bmpBuffer, 128, 160); 
					break;
				case 35:
				case 45:
				case 55:
					SetGPIO(9);
					pFile = fopen("/home/dixie/DixieUhr/Pictures/DOT_o.bmp", "r"); 
					fseek(pFile, 128, 10); // Put file in Buffer
					fread(bmpBuffer, pixelSize, 128 * 160, pFile);
					fclose(pFile);
					myTFT.TFTdrawBitmap24(0, 0, bmpBuffer, 128, 160); 
					break;
				case 7:
				case 17:
				case 27:
				case 37:
				case 47:
					pFile = fopen("/home/dixie/DixieUhr/Pictures/DOT.bmp", "r"); 
					fseek(pFile, 128, 0); // Put file in Buffer
					fread(bmpBuffer, pixelSize, 128 * 160, pFile);
					fclose(pFile);
					myTFT.TFTdrawBitmap24(0, 0, bmpBuffer, 128, 160); 
					break;
				}
			}
		}
		if(!ui8DateTime) {
			Digits[0] = value[timenow->tm_hour / 10];
			Digits[1] = value[timenow->tm_hour % 10];
			Digits[2] = "/home/dixie/DixieUhr/Pictures/DOT.bmp";
			Digits[3] = value[timenow->tm_min / 10];
			Digits[4] = value[timenow->tm_min % 10];
		} else {
			Digits[0] = value[timenow->tm_mday / 10];
			Digits[1] = value[timenow->tm_mday % 10];
			Digits[2] = "/home/dixie/DixieUhr/Pictures/DOT_u.bmp";
			date = timenow->tm_mon + 1;
			Digits[3] = value[date / 10];
			Digits[4] = value[date % 10];
		}

		for (uint8_t i = 0 ; i < 5 ; i++)
		{
			switch (i) 
			{
			case 0: 
				SetGPIO(8);
				pFile = fopen(Digits[0].c_str(), "r"); 
				break;
			case 1: 
				SetGPIO(7);
				pFile = fopen(Digits[1].c_str(), "r"); 
				break;
			case 2: 
				SetGPIO(9);
				pFile = fopen(Digits[2].c_str(), "r"); 
				break;
			case 3: 
				SetGPIO(10);
				pFile = fopen(Digits[3].c_str(), "r"); 
				break;
			case 4: 
				SetGPIO(11);
				pFile = fopen(Digits[4].c_str(), "r"); 
				if(ui8DateTime) ui8DateTime++;
				break;
			}
			
			if (pFile == NULL)  // Check file exists
			{
				std::cout << "Error: File does not exist ";
				return;
			}
			
			fseek(pFile, 128, 0); // Put file in Buffer
			fread(bmpBuffer, pixelSize, 128 * 160, pFile);
			fclose(pFile);
			myTFT.TFTdrawBitmap24(0, 0, bmpBuffer, 128, 160); 
			if((i == 2) && ui8DateTime) {
				myTFT.TFTdrawText(0, 52, dat, ST7735_YELLOW, ST7735_BLACK, 2);	
			}
			if(ui8DateTime > 2) {
				ui8DateTime = 0;
				Seconds = 66;	
			}
		}
		if(Seconds == 66) Seconds = 0; else Seconds = 1;
	}
	free(bmpBuffer);  // Free Up Buffer
}

int64_t getTime() {
	struct timespec tms;
	if (clock_gettime(CLOCK_REALTIME,&tms)) return -1;
	int64_t micros = tms.tv_sec * 1000000;
	micros += tms.tv_nsec/1000;
	if (tms.tv_nsec % 1000 >= 500) ++micros;
	return micros;
}

uint8_t* loadImage(char* name) {
	FILE *pFile ;
	size_t pixelSize = 3;
	uint8_t* bmpBuffer1 = NULL;

	pFile = fopen(name, "r");
	if (pFile == NULL) {
		std::cout << "Error TestFPS : File does not exist" << std::endl;
		return NULL;
	} else {
		bmpBuffer1 = (uint8_t*)malloc((160 * 80) * pixelSize);
		if (bmpBuffer1 == NULL)
		{
			std::cout << "Error TestFPS : MALLOC could not assign memory " << std::endl;
			return NULL;
		}
		fseek(pFile, 132, 0);
		fread(bmpBuffer1, pixelSize, 160 * 80, pFile);
		fclose(pFile);
	}
	
	return bmpBuffer1;
}

// *************** EOF ****************
