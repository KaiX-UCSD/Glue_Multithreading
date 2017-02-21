//blinkTransition// blink.c
//
// Example program for bcm2835 library
// Blinks a pin on an off every 0.5 secs
//
// After installing bcm2835, you can build this
// with something like:
// make or gcc -o led led.c -lbcm2835
// sudo ./led
/*
             define from bcm2835.h                       define from Board DVK511
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
RPI_V2_GPIO_P1_13->RPI_GPIO_P1_13:

*/



/*
 * Process for displaying emotions:
 * openEye()->transitionBlink()-> printEmotion() x2-> transitionBlink(reverse)->
 */

#include <bcm2835.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define uchar unsigned char
#define uint unsigned int


//#define Max7219_pinCLK  RPI_GPIO_P1_11
#define Max7219_pinCS  RPI_GPIO_P1_24
//#define Max7219_pinDIN  RPI_V2_GPIO_P1_13

// Constants for Switch Case
#define HAPPY   0
#define SAD 		1
#define OPEN    2
#define CLOSE   3
#define BETWEEN 4


// Eye Timer Variables
unsigned long openEyeTimer = 0;
unsigned long openEyeDelayTime = 1; // 20 milliseconds between each frame
unsigned int openEyeFrameCounter = 0;


unsigned long closeEyeTimer = 0;
unsigned long closeEyeDelayTime = 1; // 20 milliseconds between each frame
int closedEyeFrameCounter = 2;

// char characters[37] = {
// 	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
// 	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
// 	'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
// 	'U', 'V', 'W', 'X', 'Y', 'Z', ' '
// };

// Arguments to call eyes. Note: Displays 1 at a time on alternating eyes
char characters[8] = {
	 '0',
	 '1',
	 '2', // openEye
	 '3', //closedEye
	 '4', //inBetween
	 '5', //angryEye
	 '6', //happyEye
	 '7' //sadEye'
 };

char emotion[8][8] = {
	{0xC0,0x88,0x94,0xAA,0x94,0x89,0x82,0xC4}, //0 Porter eyes left
	{0x03,0x11,0x29,0x55,0x29,0x91,0x41,0x23}, //1 Porter eyes right
	{0, 60, 102, 102, 126, 126, 60, 0}, //2 openEye
	{0, 8, 8, 8, 8, 8, 8, 0}, //3 closedEye
	{0, 24, 60, 60, 60, 60, 24, 0}, //4 inBetween
	{0x00,0x06,0x0e,0x1e,0x3e,0x7e,0x7e,0x00}, //5 angryEye
	{0x04,0x08,0x10,0x20,0x20,0x10,0x08,0x04} //6 happyEye
	{0x00,0x7e,0x7e,0x3e,0x1e,0x0e,0x06,0x00} //7 sadEye
};

void Delay_xms(uint x)
{
	bcm2835_delay(x);
}
//------------------------

/* -------------------------------------------------------------------- */
void Write_Max7219_byte(uchar DATA)
{
	bcm2835_gpio_write(Max7219_pinCS,LOW);
	bcm2835_spi_transfer(DATA);
}

void Write_Max7219(uchar address1,uchar dat1,uchar address2,uchar dat2)
{
	bcm2835_gpio_write(Max7219_pinCS,LOW);
	Write_Max7219_byte(address1);
	Write_Max7219_byte(dat1);
	Write_Max7219_byte(address2);
	Write_Max7219_byte(dat2);
	bcm2835_gpio_write(Max7219_pinCS,HIGH);
}

void Init_MAX7219()
{
	Write_Max7219(0x09,0x00,0x09,0x00);
	Write_Max7219(0x0a,0x03,0x0a,0x03);
	Write_Max7219(0x0b,0x07,0x0b,0x07);
	Write_Max7219(0x0c,0x01,0x0c,0x01);
	Write_Max7219(0x0f,0x00,0x0f,0x00);
}

/* -------------------------------------------------------------------- */

uchar find_char(char letter) {
	for (uchar index = 0; index < 8; index++) {
		if (letter == characters[index]) {
			return index;
		}
	}
	//Character not found, return index of space
	return 36;
}

// Actual printing function - emotion switch
void led_print(int c) {
	int char1, char2;
	switch(c) {
		case OPEN:
			char1 = 2;
			char2 = 2;
			break;
		case CLOSE:
			char1 = 3;
			char2 = 3;
			break;
		case HAPPY:
			char1 = 6;
			char2 = 6;
			break;
		case SAD:
			char1 = 7;
			char2 = 5;
			break;
		case ANGRY:
			char1 = 5;
			char2 = 7;
			break;
		case BETWEEN:
			char1 = 4;
			char2 = 4;
			break;
	}

	for(uchar i = 1;i < 9;i++) {
		Write_Max7219(i, disp1[char2][i-1],i,disp1[char1][i-1]);
	}
}

// Creates transitional array
// problem
void blinkTransition (int emotion) {
	byte resultingArray[8] = {0,0,0,0,0,0,0,0};

	// Switch case
	switch(emotion){
		case HAPPY:
			char1 = 6;
			char2 = 6;
			selection = 7;
			break;
		case SAD:
			char1 = 7;
			char2 = 5;
			selection = 6;
			break;
		case ANGRY:
			char1 = 5;
			char2 = 7;
			selection = 5;
			break;
		}

	for (int i = 0; i < 8; i++) {
		// displ1[4] is inBetween eye
		resultingArray[i] = emotion[selection][i] & disp1[4][i];
	}

	// Loop through resulting array
	for(uchar i = 1;i < 9;i++) {
		Write_Max7219(i, disp1[char2][i-1],i,disp1[char1][i-1]);
	}

}

/*
 * Initial function on wake.
 */
void openTheEye (int emotion) {
	while(openEyeFrameCounter < 3) {
    if(millis() - openEyeTimer > openEyeDelayTime) {
      switch(openEyeFrameCounter) {
        case 0:
				// Displays closed eye
				led_print(OPEN);
        openEyeFrameCounter++;
        break;
        case 1:
				// Displays transitional
				blinkTransition(emotion);
        openEyeFrameCounter++;
        break;
        case 2:
				// Displays specified emotion
				led_print(emotion);
        openEyeFrameCounter++;
        break;
      }
      openEyeTimer = millis();
    }
  }
  openEyeFrameCounter = 0;

}

void closeTheEye(int emotion) {
  while(closedEyeFrameCounter >= 0) {
    if(millis() - closeEyeTimer > closeEyeDelayTime) {
      switch(closedEyeFrameCounter) {
        case 0:
				// Displays closed eye
				led_print(CLOSE);
        closedEyeFrameCounter--;
        break;
				// Displays transitional
        case 1:
				blinkTransition(emotion);
        closedEyeFrameCounter--;
        break;
				// Displays specified eye
        case 2:
				led_print(emotion);
        closedEyeFrameCounter--;
        break;
      }
      Serial.println(closedEyeFrameCounter);
      closeEyeTimer = millis();
    }
  }
  closedEyeFrameCounter = 2;
}

// Wrapper can call this to display specified eye
void displayEye(int emotion){
  openTheEye(emotion);
  delay(1000);
  closeTheEye(emotion);
  delay(100);
}

void initialize() {

	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_256); // The default
	bcm2835_gpio_fsel(Max7219_pinCS, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_write(disp1[0][0],HIGH);
	Delay_xms(50);
	Init_MAX7219();

	//
	// size_t index = 0;
	// while (index < strlen(argv[1])) {
	// 	// Retrive first letter
	// 	char letter1 = toupper(argv[1][index]);
	// 	index++;
	// 	// Retrieve second letter
	// 	// If it does not exist use space
	// 	char letter2 = (index < strlen(argv[1])) ? toupper(argv[1][index]) : (char) 0;
	// 	led_print(find_char(letter1), find_char(letter2));
	// 	Delay_xms(1000);
	// 	index++;
	// }
	// // Clear screen
	// led_print(36, 36);
	// bcm2835_spi_end();
	// bcm2835_close();
	// return 0;
}

int main(){
	initialize();
	displayEye(HAPPY);
	displayEye(SAD);
}
