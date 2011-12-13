// DOGSdisplay.c 
// Copyright (C) 2011
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.


///
/// \file DOGSdisplay.c
/// \brief
/// enter brief description of DOGSdisplay.c here
/// \date 10.12.2011
/// \author cord
/// \details
/// enter detailed description here

#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_ssp.h"
#include "DOGSdisplay.h"

#define DOGS_CS_OFF() GPIO_SetValue( 0, (1<<6) )
#define DOGS_CS_ON()  GPIO_ClearValue( 0, (1<<6) )
#define DOGS_DATA()   GPIO_SetValue( 0, (1<<2) )
#define DOGS_CMD()    GPIO_ClearValue( 0, (1<<2) )

void initDogs(void)
{
	SSP_CFG_Type SSP_ConfigStruct;
	PINSEL_CFG_Type PinCfg;

	/*
	 * Initialize SPI pin connect
	 * P0.7 - SCK;
	 * P0.8 - MISO
	 * P0.9 - MOSI
	 * P0.6 - SSEL - used as GPIO
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 7;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);
//	PinCfg.Funcnum = 0;
//	PinCfg.Portnum = 2;
//	PinCfg.Pinnum = 2;
//	PINSEL_ConfigPin(&PinCfg);
	GPIO_SetDir(0, (1<<2), 1);
	GPIO_SetDir(0, (1<<6), 1);

	SSP_ConfigStructInit(&SSP_ConfigStruct);

	// Initialize SSP peripheral with parameter given in structure above
	SSP_Init(LPC_SSP1, &SSP_ConfigStruct);

	// Enable SSP peripheral
	SSP_Cmd(LPC_SSP1, ENABLE);

	lcd_init();
}

void writeCommand(uint8_t data)
{
    SSP_DATA_SETUP_Type xferConfig;
    DOGS_CMD();
    DOGS_CS_ON();

	xferConfig.tx_data = &data;
	xferConfig.rx_data = NULL;
	xferConfig.length  = 1;

    SSP_ReadWrite(LPC_SSP1, &xferConfig, SSP_TRANSFER_POLLING);
    //SSPSend( (uint8_t *)&data, 1 );

    DOGS_CS_OFF();
}

void writeData(uint8_t data)
{
    SSP_DATA_SETUP_Type xferConfig;
    DOGS_DATA();
    DOGS_CS_ON();

	xferConfig.tx_data = &data;
	xferConfig.rx_data = NULL;
	xferConfig.length  = 1;

    SSP_ReadWrite(LPC_SSP1, &xferConfig, SSP_TRANSFER_POLLING);
    //SSPSend( (uint8_t *)&data, 1 );

    DOGS_CS_OFF();
}



unsigned char lcd_buffer[XPIXEL][YPIXEL/8];  //Buffer to store display data. The buffer organization is equal to
											 // the one of the display



/*--------------------------------------
Function:	lcd_init()
Input:		---
Output:		---
Description:Initializes and clears lcd
--------------------------------------*/
void lcd_init(void)
{
	//TODO Reset display here
//	DDRB |= (1<<DRESET); //Set display reset as output
//
//	PORTB|=(1<<DRESET);  //Set display reset to high -> LCD is running now

	//Initialize bottom view 3.3V (booster on) 8Bit SPI
	lcd_send_command(0x40); //Startline 0
	lcd_send_command(0xA1); //SEG reverse
	lcd_send_command(0xC0); //Set COM direction (COM0-COM63)
	lcd_send_command(0xA4); //Set all Pixel to on
	lcd_send_command(0xA6); //Display inverse off
	lcd_send_command(0xA2); //Set bias 1/9
	lcd_send_command(0x2F); //Booster, regulator, follower on
	lcd_send_command(0x27); //Set contrast
	lcd_send_command(0x41); //Set contrast 81
	lcd_send_command(0x10); //Set contrast
	lcd_send_command(0xFA); //Temperature compensation
	lcd_send_command(0x90); //Temperature compensation
	lcd_send_command(0xAF); //Display on

	lcd_clear_all(); //Clear display and set display buffer to 0

}

///*--------------------------------------
//Function:	lcd_send()
//Input:		data, cd
//Output:		---
//Description:send data, as command (cd=0)
//			or simple data (cd=1)
//--------------------------------------*/
//void lcd_send(uint8_t data, uint8_t cd)
//{
//
//	PORTB &= ~(1<<DCS);			//Slave select (DCS to gnd)
//	if (cd == 0) {
//		PORTB &= ~(1<<DCD);	//Send command (DCD to 0)
//	}
//	else {
//		PORTB |= (1<<DCD);	//Send data (DCD to 1)
//	}
//
//	spi_byte(data);			//Send data
//
//}

/*--------------------------------------
Function:	lcd_clear()
Input:		xs, ys, xe, ye
Output:		---
Description:clear a rectangle
--------------------------------------*/
void lcd_clear(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye)
{
	//i,j are index variables
	//ps, pe are page start and endadress
	//yr is restpixels
	uint8_t i=0,j=0, ps=0, pe=0, yr=0;


	ps=ys/8; //calculate startpage
	pe=ye/8; //calculate endpage


	//-------------Clear part of startpage--------------
	//Set coloumn adress
	lcd_send_command(xs&0x0F);      //LSB adress
	lcd_send_command(0x10+(xs>>4)); //MSB adress

	//set page adress
	lcd_send_command(0xB0+ps);

	j=0xFF; //use j as buffer
	if (pe == ps) //if start and endadress are in same page
	   			  //you have to make sure, not to delete to much
	{
		j=ye%8-0xFF; //calculate stop within first page
	}

	yr=ys%8; //calculate the start within first page
	for(i=xs; i<=xe; i++) //loop starting first colomn to last coloumn
	{

		lcd_buffer[i][ps]&=j>>(8-yr);     //clear the buffer
		lcd_send_data(lcd_buffer[i][ps]); //send the changed pages of the buffer to the display
	}

	//-------------Clear part of endpage----------------
	//Set coloumn adress
	lcd_send_command(xs&0x0F);		//LSB adress
	lcd_send_command(0x10+(xs>>4)); //MSB adress

	//set page adress
	lcd_send_command(0xB0+pe);
	yr=ye%8; //calculate the stop within last page

	for(i=xs; i<=xe; i++) //loop starting first colomn to last coloumn
	{
		lcd_buffer[i][pe]&=(0xFF<<(yr+1)); //clear the buffer
		lcd_send_data(lcd_buffer[i][pe]);  //send the changed pages of the buffer to the display
	}


	//-------------------Clear middle pages----------------------
	for(j=ps+1; j<pe; j++) //loop starting first middle page to last middle page
	{
		//Set coloumn adress
		lcd_send_command(xs&0x0F); 		//LSB adress
		lcd_send_command(0x10+(xs>>4));	//MSB adress

		//set page adress
		lcd_send_command(0xB0+j);

		for(i=xs; i<=xe; i++) //loop starting first colomn to last coloumn
		{

			lcd_buffer[i][j]=0x00; //clear the buffer
			lcd_send_data(0x00);   //clear display same as lcd_send_data(lcd_buffer[i][j]);
		}
	}

}


/*--------------------------------------
Function:	lcd_fill()
Input:		xs, ys, xe, ye
Output:		---
Description:fill a rectangle
--------------------------------------*/
void lcd_fill(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye)
{
	//i,j are index variables
	//ps, pe are page start and endadress
	//yr is restpixels
	uint8_t i=0,j=0, ps=0, pe=0, yr=0;

	ps=ys/8; //calculate startpage
	pe=ye/8; //calculate endpage


	//-------------Fill part of startpage--------------
	//Set coloumn adress
	lcd_send_command(xs&0x0F);      //LSB adress
	lcd_send_command(0x10+(xs>>4)); //MSB adress

	//set page adress
	lcd_send_command(0xB0+ps);

	j=0xFF; //use j as buffer
	if (pe == ps)
	{
		j=ye%8-0xFF; //calculate stop within first page
	}

	yr=ys%8; //calculate the start within first page

	for(i=xs; i<=xe; i++) //loop starting first colomn to last coloumn
	{

		lcd_buffer[i][ps]|=j<<yr;		 //fill the buffer
		lcd_send_data(lcd_buffer[i][ps]);//send the changed pages of the buffer to the display
	}

	//-------------Fill part of endpage--------------
	//Set coloumn adress
	lcd_send_command(xs&0x0F);      //LSB adress
	lcd_send_command(0x10+(xs>>4)); //MSB adress

	//set page adress
	lcd_send_command(0xB0+pe);
	yr=ye%8; ///calculate the stop within last page

	for(i=xs; i<=xe; i++)	//loop starting first colomn to last coloumn
	{
		lcd_buffer[i][pe]|=(0xFF>>(8-yr-1));//fill the buffer
		lcd_send_data(lcd_buffer[i][pe]);	//send the changed pages of the buffer to the display
	}


	//-------------------Fill middle pages----------------------
	for(j=ps+1; j<pe; j++) //loop starting first middle page to last middle page
	{
		//Set coloumn adress
		lcd_send_command(xs&0x0F); 		//LSB adress
		lcd_send_command(0x10+(xs>>4));	//MSB adress

		//set page adress
		lcd_send_command(0xB0+j);

		for(i=xs; i<=xe; i++)	//loop starting first colomn to last coloumn
		{

			lcd_buffer[i][j]=0xFF; //fill the buffer
			lcd_send_data(0xFF);   //fill display same as lcd_send_data(lcd_buffer[i][j]);
		}
	}

}


/*--------------------------------------
Function:	lcd_line()
Input:		xs, ys, xe, ye
Output:		---
Description:draw a stright line, please
			make sure, that the line is
			either horizontal or
			vertical
--------------------------------------*/
void lcd_line(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye)
{
	//i,j are index variables
	//ps, pe are page start and endadress
	//yr is restpixels
	uint8_t i=0,j=0, ps=0, pe=0, yr=0;

	ps=ys/8; //calculate startpage
	pe=ye/8; //calculate endpage


	//-------------Starpage--------------
	//Set coloumn adress
	lcd_send_command(xs&0x0F); 	   //LSB adress
	lcd_send_command(0x10+(xs>>4));//MSB adress

	//set page adress
	lcd_send_command(0xB0+ps);

	yr=ys%8; //calculate the start within first page


	if (ys==ye) //horizontal line
	{
		for(i=xs; i<=xe; i++) //loop starting first colomn to last coloumn
		{

			lcd_buffer[i][ps]|=0x01<<yr;     //fill the buffer
			lcd_send_data(lcd_buffer[i][ps]);//send the changed pages of the buffer to the display
		}
	}
	else //vertical line
	{
		i=0xFF; //use i as buffer
		if (ps == pe) //if line is only in one page, you have to make sure
					  //that you only fill a apart of the page
		{
			i=ye%8-0xFF; //calculate stop within first page
		}
		//Fill part of startpage
		lcd_buffer[xs][ps]|=i<<yr;        //fill the buffer
		lcd_send_data(lcd_buffer[xs][ps]);//send the changed pages of the buffer to the display

		if(ps==pe) //if line is only in one page, you can leave funtion here.
		 return;

		//Fill part of endpage
		//Set coloumn adress
		lcd_send_command(xs&0x0F);      //LSB adress
		lcd_send_command(0x10+(xs>>4));	//MSB adress
		//set page adress
		lcd_send_command(0xB0+pe);

		yr=ye%8; //calculate the stop within last page
		lcd_buffer[xs][pe]|=(0xFF>>(8-yr)); //fill the buffer
		lcd_send_data(lcd_buffer[xs][pe]);	//send the changed pages of the buffer to the display

		//-------------------Fill middle pages----------------------
		for(j=ps+1; j<pe; j++) //loop starting first middle page to last middle page
		{
			//Set coloumn adress
			lcd_send_command(xs&0x0F); 	    //LSB adress
			lcd_send_command(0x10+(xs>>4)); //MSB adress

			//set page adress
			lcd_send_command(0xB0+j);

			lcd_buffer[xs][j]=0xFF;//fill the buffer
			lcd_send_data(0xFF);   //fill display same as lcd_send_data(lcd_buffer[i][j]);
		}

	}

}

/*--------------------------------------
Function:	lcd_rect()
Input:		xs, ys, xe, ye
Output:		---
Description:draw a rectanlge
--------------------------------------*/
void lcd_rect(uint8_t xs, uint8_t ys, uint8_t xe, uint8_t ye)
{
	lcd_line(xs, ys, xe, ys); //upper line
	lcd_line(xs, ye, xe, ye); //lower line
	lcd_line(xs, ys, xs, ye); //left line
	lcd_line(xe, ys, xe, ye); //right line
}


/*--------------------------------------
Function:	lcd_char()
Input:		x, y, data, *font, linkmode
Output:		---
Description:print a single char on x,y
--------------------------------------*/
void lcd_char(uint8_t x, uint8_t y, uint8_t data, uint8_t *font, uint8_t linkmode)
{
	//i,j are index variables
	//n is flag, if char has to be shifted in one page
	//ps, pe are page start and endadress
	//yr is restpixels
	//byte buffer to store char-data
	uint8_t i=0, j=0, n=1, ps=0, pe=0, yr=0, byte=0;

	unsigned int font_pos=0; //postion of data within the font array


	ps=y/8; 						//calculate startpage
	pe=pgm_read_byte(font + 6)+ps; 	//calculate endpage
	yr=y%8;  //calculate the start within first page

	if(yr) //if you are not starting at 0 in a page, the
		pe++; //endpage is increased with one

	//calculate positon of data in font array
	//(data*Bytes pro Char)-(start of chars * bytes pro char)+bytes for header
	font_pos=(data*pgm_read_byte(font + 7))-(pgm_read_byte(font + 2)*pgm_read_byte(font + 7))+8;

	font_pos-=pgm_read_byte(font + 4); //subtract pixels for one char, because it's added again in loop

	if(linkmode==ADD)  //linkmode add, the pixels of char are added into display
	{
		for(j=ps; j<pe;j++)  //loop starting first page to last page
		{
			//Set coloumn adress
			lcd_send_command(x&0x0F);     //LSB adress
			lcd_send_command(0x10+(x>>4));//MSB adress

			//set page adress
			lcd_send_command(0xB0+j);

			font_pos+=pgm_read_byte(font + 4); //set new postion of data pointer to get next data for one page

			if(n) //jump alternating in if clause or else clause
			{
				for(i=x; i<pgm_read_byte(font + 4)+x; i++) //loop starting first colomn to last coloumn
				{
					byte=pgm_read_byte(font+i+font_pos-x); //get data from font buffer (subtract x because i is not started with 0)
					lcd_buffer[i][j]|=byte<<yr;      //write data into display buffer, but shifted if it is needed
					lcd_send_data(lcd_buffer[i][j]); //send the changed pages of the buffer to the display
				}

				if(yr) //next step is shifting the char the other direction, if the char is not writtin whithin one page
					n=0;
			}
			else //shift the char another direction
			{
				font_pos-=pgm_read_byte(font + 4); //set new postion of data pointer to get next data for one page (substract because it
												   //is added again in next loop
				for(i=x; i<pgm_read_byte(font + 4)+x; i++)
				{
					byte=pgm_read_byte(font+i+font_pos-x); //get data from font buffer (subtract x because i is not started with 0)
					lcd_buffer[i][j]|=byte>>(8-yr);  //write data into display buffer, but shifted if it is needed
					lcd_send_data(lcd_buffer[i][j]); //send the changed pages of the buffer to the display
				}
				n=1; //next step is shifting the char the other direction

				if(j<pe-1)
					j--;  //the page recently filled has content from last part, the rest would remain empty.
						  //Because of that, the page is decremented, because in next loop the page is inceased again to the
						  //actual written page. New data is written into same page
			}


		}
	}
	else //another linkmode, data is addad with xor
	{
		for(j=ps; j<pe;j++)  //loop starting first page to last page
		{
			//Set coloumn adress
			lcd_send_command(x&0x0F); 	   //LSB adress
			lcd_send_command(0x10+(x>>4)); //MSB adress

			//set page adress
			lcd_send_command(0xB0+j);

			font_pos+=pgm_read_byte(font + 4); //set new postion of data pointer to get next data for one page

			if(n) //jump alternating in if clause or else clause
			{
				for(i=x; i<pgm_read_byte(font + 4)+x; i++) //loop starting first colomn to last coloumn
				{
					byte=pgm_read_byte(font+i+font_pos-x); //get data from font buffer (subtract x because i is not started with 0)
					lcd_buffer[i][j]^=byte<<yr;      //write data into display buffer, but shifted if it is needed
					lcd_send_data(lcd_buffer[i][j]); //send the changed pages of the buffer to the display
				}

				if(yr) //next step is shifting the char the other direction, if the char is not writtin whithin one page
					n=0;
			}
			else //shift the char another direction
			{
				font_pos-=pgm_read_byte(font + 4); //set new postion of data pointer to get next data for one page (substract because it
												   //is added again in next loop
				for(i=x; i<pgm_read_byte(font + 4)+x; i++)
				{
					byte=pgm_read_byte(font+i+font_pos-x); //get data from font buffer (subtract x because i is not started with 0)
					lcd_buffer[i][j]^=byte>>(8-yr);	  //write data into display buffer, but shifted if it is needed
					lcd_send_data(lcd_buffer[i][j]);  //send the changed pages of the buffer to the display
				}
				n=1; //next step is shifting the char the other direction

				if(j<pe-1)
					j--; //the page recently filled has content from last part, the rest would remain empty.
						 //Because of that, the page is decremented, because in next loop the page is inceased again to the
						 //actual written page. New data is written into same page
			}
		}
	}

}


/*--------------------------------------
Function:	lcd_string()
Input:		x, y, data, *font, linkmode
Output:		---
Description:print a string on x,y
--------------------------------------*/
void lcd_string(uint8_t x, uint8_t y, char *data, uint8_t *font, uint8_t linkmode)
{
	//i is index variable
	//xe, ye is endpostion of string
	//length is number of chars
	uint8_t i=0, xe=0, ye=0, length=0;

	if(linkmode==DELETE)
	{
		//get number of chars
		while(data[i])
		{
			i++;
		}
		length=i;

		xe=length*pgm_read_byte(font + 4)+x; //claculate length of string
		ye=y+pgm_read_byte(font + 5);        //calcutlate hight of string
		lcd_clear(x,y,xe,ye); //clear a rectangle where the string is printed later on
	}
	else if(linkmode==INVERS)
	{
		//get number of chars
		while(data[i])
		{
			i++;
		}
		length=i;

		xe=length*pgm_read_byte(font + 4)+x; //claculate length of string
		ye=y+pgm_read_byte(font + 5);		 //calcutlate hight of string
		lcd_fill(x,y,xe,ye); //fill a rectangle where the string is printed later on
	}
	else
	{
	 //other modes
	}


	i=0; //reset i
	while(data[i])
	{
		lcd_char(x,y,data[i], font, linkmode); //write single char
		x+=pgm_read_byte(font + 4); //calculate next x postion for next char
		i++;
	}
}
