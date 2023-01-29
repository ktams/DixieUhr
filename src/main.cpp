/*
 * Project Name: ST7735_TFT_RPI  
 * File: main.cpp
 * Description: library test file  
 * Author: Gavin Lyons.
 * Created May 2021
 * Description: See URL for full details.
 * NOTE :: USER OPTIONS 1-3 in SETUP function
 * URL: https://github.com/gavinlyonsrepo/ST7735_TFT_RPI
 */

// Section ::  libraries 
#include <bcm2835.h> // for SPI GPIO and delays.
#include <time.h> // for test 11 & FPS
#include <iostream>
#include <string>
#include "ST7735_TFT.h"
//#include "Bi_Color_Bitmap.h" // Data for test 11 and 12.

using namespace std;

// Section :: Defines   
//  Test timing related defines 
#define TEST_DELAY1 1000
#define TEST_DELAY2 2000
#define TEST_DELAY5 5000
#define CLOCK_DISPLAY_TIME 100

// Section :: Globals 
ST7735_TFT myTFT;
bool bTestFPS = false; // Optional ,runs FPS test at end if true.

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

void Test0(void);  // Print out all fonts 1-6
void Test1A(void); // defined 16-bit Colors, text
void Test1B(void); // print entire ASCII font 0 to 127, default font
void Test1C(void); // print numbers int and float using draw functions
void Test1D(void); // print numbers int and float using PRINT function
void Test1E(void); // Print out font 7-8 using draw functions
void Test2(void);  // font sizes(1-4) + character draw using draw functions
void Test8(void);  // More shapes, media buttons graphic.
void Test9(void);  // Rotate
//void Test11(void); // "clock demo" icons, small bi-color bitmaps, font 7-8
void EndTests(void);

int64_t getTime(); // Utility for FPS test
uint8_t* loadImage(char* name); // Utility for FPS test

//  Section ::  MAIN loop

int main(void) 
{

	if(!Setup(8)) {	// initialisiert das erste Display (0) und resettet die anderen mit
		std::cout << "Problems with the Setup" << std::endl;
		return -1;
	}
	myTFT.TFTsetRotation(TFT_Degress_180);
	for(uint8_t i = 1; i < 5; i++) {
		IniDisplay(i);	//Initialisiert das zweite Display (1)
	}
	Dixie();
	return 0;
}
// *** End OF MAIN **


//  Section ::  Function Space 

void SetGPIO(int8_t cs) {
	int8_t RST_TFT = 25;
	int8_t DC_TFT = 24;
	int8_t SCLK_TFT = 5; // 5, change to GPIO no for sw spi, -1 hw spi
	int8_t SDIN_TFT = 6; // 6, change to GPIO no for sw spi, -1 hw spi
	int8_t CS_TFT = cs ;  // 8, change to GPIO no for sw spi, -1 hw spi
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
// ** USER OPTION 2 Screen Setup **
	uint8_t OFFSET_COL = 0;  // 2, These offsets can be adjusted for any issues->
	uint8_t OFFSET_ROW = 0; // 3, with manufacture tolerance/defects
	uint16_t TFT_WIDTH = 128;// Screen width in pixels
	uint16_t TFT_HEIGHT = 160; // Screen height in pixels
// ***********************************
	// ** USER OPTION 1 GPIO/SPI TYPE HW OR SW **
	SetGPIO(cs);
//*********************************************
	myTFT.TFTInitScreenSize(OFFSET_COL, OFFSET_ROW , TFT_WIDTH , TFT_HEIGHT);
	myTFT.TFTsetRotation(TFT_Degress_180);
	if(!myTFT.TFTInitPCBType(TFT_ST7735S_Black, 0, 0))return -1;
//**********************************************
	return 0;
}

void writeCommand(uint8_t spicmdbyte, int8_t cs) {
	int8_t DC_TFT = 24;
//	TFT_DC_SetLow;
	bcm2835_gpio_write(DC_TFT, LOW);
//	TFT_CS_SetLow;
	bcm2835_gpio_write(cs, LOW);
	spiWriteSoftware(spicmdbyte, cs);
//	TFT_CS_SetHigh;
	bcm2835_gpio_write(cs, HIGH);
}

void writeData(uint8_t spidatabyte, int8_t cs) {
	int8_t DC_TFT = 24;
//	TFT_DC_SetHigh;
	bcm2835_gpio_write(DC_TFT, HIGH);
//	TFT_CS_SetLow;
	bcm2835_gpio_write(cs, LOW);
	spiWriteSoftware(spidatabyte, cs);
//	TFT_CS_SetHigh;
	bcm2835_gpio_write(cs, HIGH);
}

void spiWriteSoftware(uint8_t spidata, int8_t cs) {
	int8_t SCLK_TFT = 5; // 5, change to GPIO no for sw spi, -1 hw spi
	int8_t SDIN_TFT = 6; // 6, change to GPIO no for sw spi, -1 hw spi
	uint8_t i;
	for (i = 0; i < 8; i++) {
//		TFT_SDATA_SetLow;
		bcm2835_gpio_write(SDIN_TFT, LOW);
		if (spidata & 0x80) {
//			TFT_SDATA_SetHigh; // b1000000 Mask with 0 & all zeros out.
			bcm2835_gpio_write(SDIN_TFT, HIGH);
		}
//		TFT_SCLK_SetHigh;
		bcm2835_gpio_write(SCLK_TFT, HIGH);
		TFT_MICROSEC_DELAY(TFT_HIGHFREQ_DELAY);
		spidata <<= 1;
//		TFT_SCLK_SetLow;
		bcm2835_gpio_write(SCLK_TFT, LOW);
		TFT_MICROSEC_DELAY(TFT_HIGHFREQ_DELAY);
	}
}

void spiWriteDataBuffer(uint8_t* spidata, uint32_t len, int8_t cs) {
	int8_t DC_TFT = 24;
//	TFT_DC_SetHigh;
	bcm2835_gpio_write(DC_TFT, HIGH);
//	TFT_CS_SetLow;
	bcm2835_gpio_write(cs, LOW);
	for(uint32_t i=0; i<len; i++) {
		spiWriteSoftware(spidata[i], cs);
		}
//	TFT_CS_SetHigh;
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
// ** USER OPTION 2 Screen Setup **
	uint8_t OFFSET_COL = 0;  // 2, These offsets can be adjusted for any issues->
	uint8_t OFFSET_ROW = 0; // 3, with manufacture tolerance/defects
	uint16_t TFT_WIDTH = 128;// Screen width in pixels
	uint16_t TFT_HEIGHT = 160; // Screen height in pixels
// ***********************************
	// ** USER OPTION 1 GPIO/SPI TYPE HW OR SW **
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
	int8_t SCLK_TFT = 5; // 5, change to GPIO no for sw spi, -1 hw spi
	int8_t SDIN_TFT = 6; // 6, change to GPIO no for sw spi, -1 hw spi
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
	uint8_t ui8DateTime = 0;
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
	bcm2835_gpio_write(RPI_BPLUS_GPIO_J8_36, HIGH);
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

			if(!button[1]) {
				if(!ui8DateTime) ui8DateTime = 1; else ui8DateTime = 0;
				break;
			}
			if(!ui8DateTime) {
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
			switch (i) // select file
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
				std::cout << "Error: File does not exist: ";
				std::cout << Digits[i].c_str() << std::endl;
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

void Test0(void) {

	char teststr1[] = "Default 1";
	char teststr2[] = "THICK 2";
	char teststr3[] = "Seven 3";
	char teststr4[] = "WIDE 4";
	char teststr5[] = "Tiny 5";
	char teststr6[] = "Home 6";

	myTFT.TFTfillScreen(ST7735_BLACK);
	
	myTFT.TFTFontNum(TFTFont_Default);
	myTFT.TFTdrawText(0, 4, teststr1, ST7735_WHITE, ST7735_BLACK, 2);
	myTFT.TFTFontNum(TFTFont_Thick);
	myTFT.TFTdrawText(0, 20, teststr2, ST7735_GREEN, ST7735_BLACK, 2);
	myTFT.TFTFontNum(TFTFont_Seven_Seg);
	myTFT.TFTdrawText(0, 36, teststr3, ST7735_BLUE, ST7735_BLACK, 2);
	myTFT.TFTFontNum(TFTFont_Wide);
	myTFT.TFTdrawText(0, 52, teststr4, ST7735_CYAN, ST7735_BLACK, 2);
	myTFT.TFTFontNum(TFTFont_Tiny);
	myTFT.TFTdrawText(0, 68, teststr5, ST7735_RED, ST7735_BLACK, 2);
	myTFT.TFTFontNum(TFTFont_HomeSpun);
	myTFT.TFTdrawText(0, 84, teststr6, ST7735_YELLOW, ST7735_BLACK, 2);
	TFT_MILLISEC_DELAY(TEST_DELAY5);
	myTFT.TFTfillScreen(ST7735_BLACK);
	myTFT.TFTFontNum(TFTFont_Default);
}

void Test1A(void) {
	char teststr1[] = "WHITE";
	char teststr2[] = "BLUE";
	char teststr3[] = "RED";
	char teststr4[] = "GREEN";
	char teststr5[] = "CYAN";
	char teststr6[] = "MAGENTA";
	char teststr7[] = "YELLOW";
	char teststr8[] = "GREY";
	char teststr9[] = "TAN";
	char teststr10[] = "BROWN";
	char teststr11[] = "ORANGE";
	myTFT.TFTdrawText(5, 5, teststr1, ST7735_WHITE, ST7735_BLACK, 1);
	myTFT.TFTdrawText(5, 15, teststr2, ST7735_BLUE, ST7735_BLACK, 1);
	myTFT.TFTdrawText(5, 25, teststr3, ST7735_RED, ST7735_BLACK, 1);
	myTFT.TFTdrawText(5, 35, teststr4, ST7735_GREEN, ST7735_BLACK, 1);
	myTFT.TFTdrawText(5, 45, teststr5, ST7735_CYAN, ST7735_BLACK, 1);
	myTFT.TFTdrawText(5, 55, teststr6, ST7735_MAGENTA, ST7735_BLACK, 1);
	myTFT.TFTdrawText(5, 65, teststr7, ST7735_YELLOW, ST7735_BLACK, 1);
	myTFT.TFTdrawText(5, 75, teststr8, ST7735_GREY, ST7735_BLACK, 1);
	myTFT.TFTdrawText(5, 85, teststr9, ST7735_TAN, ST7735_BLACK, 1);
	myTFT.TFTdrawText(5, 95, teststr10 , ST7735_BROWN, ST7735_BLACK, 1);
	myTFT.TFTdrawText(5, 105, teststr11 , ST7735_ORANGE, ST7735_BLACK, 1);
	TFT_MILLISEC_DELAY(TEST_DELAY5);
	myTFT.TFTfillScreen(ST7735_BLACK);
}

void Test1B(void) {
	uint8_t row = 5;
	uint8_t col = 5;
	for (char i = 0; i < 126; i++) {
		
		myTFT.TFTdrawChar(col, row, i, ST7735_GREEN, ST7735_BLACK, 1);
		col += 10;
		if (col > 115) {
			row += 10;
			col = 5;
		}
	}
	TFT_MILLISEC_DELAY(TEST_DELAY5);
	myTFT.TFTfillScreen(ST7735_BLACK);
	
	row = 5;
	col = 5;
	for (unsigned char j = 127; j < 254; j++) {
		
		myTFT.TFTdrawChar(col, row, j, ST7735_GREEN, ST7735_BLACK, 1);
		col += 10;
		if (col > 115) {
			row += 10;
			col = 5;
		}
	}
	TFT_MILLISEC_DELAY(TEST_DELAY5);
	myTFT.TFTfillScreen(ST7735_BLACK);
}


void Test1C(void)
{
	int myInt=931;
	char myStr[5];
	myTFT.TFTFontNum(TFTFont_Seven_Seg);
	sprintf(myStr, "%d", myInt);
	myTFT.TFTdrawText(5, 5, myStr, ST7735_BLUE, ST7735_BLACK, 3);

	float  myPI = 3.171592;
	char myStr2[8];
	sprintf(myStr2, "%0.3f", myPI); // 3.172
	myTFT.TFTdrawText(5, 65, myStr2, ST7735_RED, ST7735_BLACK, 3);
	
	TFT_MILLISEC_DELAY(TEST_DELAY2);
	myTFT.TFTfillScreen(ST7735_BLACK);

}

void Test1E(void)
{
	// Note fonts 7 and 8 are numbers only + : 
	char teststr1[] = "12:81";
	char teststr2[] = "72:83";
	
	myTFT.TFTFontNum(TFTFont_Bignum);
	myTFT.TFTdrawCharNumFont(0, 0, '6',  ST7735_RED, ST7735_BLACK);
	myTFT.TFTdrawTextNumFont(0, 40, teststr1, ST7735_YELLOW, ST7735_RED);
	
	myTFT.TFTFontNum(TFTFont_Mednum);
	myTFT.TFTdrawCharNumFont(0, 75, '7',  ST7735_WHITE, ST7735_BLACK);
	myTFT.TFTdrawTextNumFont(0, 100, teststr2, ST7735_GREEN, ST7735_RED);
	
	TFT_MILLISEC_DELAY(TEST_DELAY5);
	myTFT.TFTfillScreen(ST7735_BLACK);
}

void Test2(void) {
	myTFT.TFTFontNum(TFTFont_Default);
	char teststr1[] = "TEST";
	myTFT.TFTdrawText(0, 5, teststr1, ST7735_WHITE, ST7735_BLACK, 2);
	myTFT.TFTdrawText(0, 25, teststr1, ST7735_WHITE, ST7735_BLACK, 3);
	myTFT.TFTdrawText(0, 55, teststr1, ST7735_WHITE, ST7735_BLACK, 4);
	myTFT.TFTdrawChar(0, 85, 'G', ST7735_WHITE, ST7735_BLACK, 5);
	myTFT.TFTdrawChar(45, 85, 'L', ST7735_WHITE, ST7735_BLACK, 5);

	TFT_MILLISEC_DELAY(TEST_DELAY2);
	myTFT.TFTfillScreen(ST7735_BLACK);
}


void Test8() 
{
	char teststr1[] = "Press Play";
	//Draw play button 
	myTFT.TFTfillRoundRect(25, 10, 78, 60, 8, ST7735_WHITE);
	myTFT.TFTfillTriangle(42, 20, 42, 60, 90, 40, ST7735_RED);
	myTFT.TFTdrawText(5, 80, teststr1, ST7735_GREEN, ST7735_BLACK, 2);
	TFT_MILLISEC_DELAY(TEST_DELAY1);
	
	// change play color
	myTFT.TFTfillTriangle(42, 20, 42, 60, 90, 40, ST7735_GREEN);
	TFT_MILLISEC_DELAY(TEST_DELAY1);
	// change play color
	myTFT.TFTfillTriangle(42, 20, 42, 60, 90, 40, ST7735_BLUE);
	TFT_MILLISEC_DELAY(TEST_DELAY1);
}

void Test9()
{
	char teststr0[] = "Rotate 0"; //normal
	char teststr1[] = "Rotate 90"; // 90
	char teststr2[] = "Rotate 180"; // 180
	char teststr3[] = "Rotate 270"; // 270
	
	myTFT.TFTfillScreen(ST7735_BLACK);
	myTFT.TFTsetRotation(TFT_Degress_0);
	myTFT.TFTdrawText(20, 20, teststr0, ST7735_GREEN, ST7735_BLACK, 1);
	TFT_MILLISEC_DELAY(TEST_DELAY2);
	
	myTFT.TFTfillScreen(ST7735_BLACK);
	myTFT.TFTsetRotation(TFT_Degress_90);
	myTFT.TFTdrawText(20, 20, teststr1, ST7735_GREEN, ST7735_BLACK, 1);
	TFT_MILLISEC_DELAY(TEST_DELAY2);
	
	myTFT.TFTfillScreen(ST7735_BLACK);
	myTFT.TFTsetRotation(TFT_Degress_180);
	myTFT.TFTdrawText(20, 20, teststr2, ST7735_GREEN, ST7735_BLACK, 1);
	TFT_MILLISEC_DELAY(TEST_DELAY2);
	
	myTFT.TFTfillScreen(ST7735_BLACK);
	myTFT.TFTsetRotation(TFT_Degress_270);
	myTFT.TFTdrawText(20, 20, teststr3, ST7735_GREEN, ST7735_BLACK, 1);
	TFT_MILLISEC_DELAY(TEST_DELAY2);
	
	myTFT.TFTsetRotation(TFT_Degress_0);
	myTFT.TFTchangeMode(TFT_Normal_mode);
	myTFT.TFTfillScreen(ST7735_BLACK);
}
/*
void Test11(void)
{
	time_t now;
	struct tm *timenow;
	char value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	uint16_t count=CLOCK_DISPLAY_TIME;
	char myStr[5];
	char teststr1[] = "G Lyons";
	
	myTFT.TFTfillScreen(ST7735_BLACK);
	
	myTFT.TFTdrawIcon(2, 2, 16, ST7735_BLACK, ST7735_WHITE, SignalIcon);
	myTFT.TFTdrawIcon(20, 2, 16, ST7735_BLACK, ST7735_WHITE, MsgIcon);
	myTFT.TFTdrawIcon(40, 2, 8, ST7735_BLACK, ST7735_WHITE, AlarmIcon);
	myTFT.TFTdrawIcon(102, 2, 16, ST7735_BLACK, ST7735_WHITE, BatIcon);
	
	myTFT.TFTdrawIcon(5, 20, 12, ST7735_GREEN , ST7735_BLACK,powerIcon);
	myTFT.TFTdrawIcon(20, 20, 12, ST7735_RED, ST7735_YELLOW, speedIcon);
	myTFT.TFTdrawBitmap(105, 20, 20 , 20, ST7735_CYAN , ST7735_BLACK, smallImage);
	
	myTFT.TFTdrawText(80, 90, teststr1, ST7735_BLUE, ST7735_BLACK, 1);
	myTFT.TFTFontNum(TFTFont_Bignum);
	while(1)
	{
		time(&now);
		timenow = localtime(&now); 
		count--; 
		sprintf(myStr, "%03d", count);
		
		myTFT.TFTdrawCharNumFont(0, 45, value[timenow->tm_hour / 10],ST7735_RED, ST7735_BLACK);
		myTFT.TFTdrawCharNumFont(16, 45, value[timenow->tm_hour % 10],ST7735_RED, ST7735_BLACK);
		myTFT.TFTdrawCharNumFont(32, 45, ':' , ST7735_GREEN, ST7735_BLACK);
		myTFT.TFTdrawCharNumFont(48, 45, value[timenow->tm_min / 10], ST7735_RED, ST7735_BLACK);
		myTFT.TFTdrawCharNumFont(64, 45, value[timenow->tm_min % 10], ST7735_RED, ST7735_BLACK);
		myTFT.TFTdrawCharNumFont(80, 45, ':' , ST7735_GREEN, ST7735_BLACK);
		myTFT.TFTdrawCharNumFont(96, 45, value[timenow->tm_sec / 10],  ST7735_RED, ST7735_BLACK);
		myTFT.TFTdrawCharNumFont(112, 45, value[timenow->tm_sec % 10],  ST7735_RED, ST7735_BLACK);
		myTFT.TFTdrawTextNumFont(0, 85, myStr, ST7735_YELLOW, ST7735_RED);
		
		if(count==1)break;
	}
	TFT_MILLISEC_DELAY(TEST_DELAY2);
	myTFT.TFTfillScreen(ST7735_BLACK);
	myTFT.TFTFontNum(TFTFont_Default);
}*/

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
