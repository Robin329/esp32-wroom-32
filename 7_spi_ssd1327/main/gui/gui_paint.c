
#include <stdint.h>
#include <stdlib.h>
#include <string.h> //memset()
#include "gui_paint.h"

ssd1327_paint st_paint;

/******************************************************************************
function: Create Image
parameter:
    image   :   Pointer to the image cache
    width   :   The width of the picture
    Height  :   The height of the picture
    Color   :   Whether the picture is inverted
******************************************************************************/
void Paint_NewImage(uint8_t *image, uint16_t Width, uint16_t Height, uint16_t Rotate,
		    uint16_t Color)
{
	st_paint.Image = NULL;
	st_paint.Image = image;

	st_paint.WidthMemory = Width;
	st_paint.HeightMemory = Height;
	st_paint.Color = Color;
	st_paint.Scale = 2;

	st_paint.WidthByte = (Width % 8 == 0) ? (Width / 8) : (Width / 8 + 1);
	st_paint.HeightByte = Height;

	st_paint.Rotate = Rotate;
	st_paint.Mirror = MIRROR_NONE;

	if (Rotate == ROTATE_0 || Rotate == ROTATE_180) {
		st_paint.Width = Width;
		st_paint.Height = Height;
	} else {
		st_paint.Width = Height;
		st_paint.Height = Width;
	}
}

/******************************************************************************
function: Select Image
parameter:
    image : Pointer to the image cache
******************************************************************************/
void Paint_SelectImage(uint8_t *image)
{
	st_paint.Image = image;
}

/******************************************************************************
function: Select Image Rotate
parameter:
    Rotate : 0,90,180,270
******************************************************************************/
void Paint_SetRotate(uint16_t Rotate)
{
	if (Rotate == ROTATE_0 || Rotate == ROTATE_90 || Rotate == ROTATE_180 ||
	    Rotate == ROTATE_270) {
		printf("Set image Rotate %d\r\n", Rotate);
		st_paint.Rotate = Rotate;
	} else {
		printf("rotate = 0, 90, 180, 270\r\n");
	}
}

void Paint_SetScale(uint8_t scale)
{
	if (scale == 2) {
		st_paint.Scale = scale;
		st_paint.WidthByte = (st_paint.WidthMemory % 8 == 0) ?
					  (st_paint.WidthMemory / 8) :
					  (st_paint.WidthMemory / 8 + 1);
	} else if (scale == 4) {
		st_paint.Scale = scale;
		st_paint.WidthByte = (st_paint.WidthMemory % 4 == 0) ?
					  (st_paint.WidthMemory / 4) :
					  (st_paint.WidthMemory / 4 + 1);
	} else if (scale == 16) {
		st_paint.Scale = scale;
		st_paint.WidthByte = (st_paint.WidthMemory % 2 == 0) ?
					  (st_paint.WidthMemory / 2) :
					  (st_paint.WidthMemory / 2 + 1);
	} else if (scale == 65) {
		st_paint.Scale = scale;
		st_paint.WidthByte = st_paint.WidthMemory * 2;
	} else {
		printf("Set Scale Input parameter error\r\n");
		printf("Scale Only support: 2 4 16 65\r\n");
	}
}
/******************************************************************************
function:	Select Image mirror
parameter:
    mirror   :Not mirror,Horizontal mirror,Vertical mirror,Origin mirror
******************************************************************************/
void Paint_SetMirroring(uint8_t mirror)
{
	if (mirror == MIRROR_NONE || mirror == MIRROR_HORIZONTAL ||
	    mirror == MIRROR_VERTICAL || mirror == MIRROR_ORIGIN) {
		printf("mirror image x:%s, y:%s\r\n",
		      (mirror & 0x01) ? "mirror" : "none",
		      ((mirror >> 1) & 0x01) ? "mirror" : "none");
		st_paint.Mirror = mirror;
	} else {
		printf("mirror should be MIRROR_NONE, MIRROR_HORIZONTAL, \
        MIRROR_VERTICAL or MIRROR_ORIGIN\r\n");
	}
}

/******************************************************************************
function: Draw Pixels
parameter:
    Xpoint : At point X
    Ypoint : At point Y
    Color  : Painted colors
******************************************************************************/
void Paint_SetPixel(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color)
{
	if (Xpoint > st_paint.Width || Ypoint > st_paint.Height) {
		printf("Exceeding display boundaries (X,Y)=(%d, %d) (W, H)= (%d, %d)\r\n",
		      Xpoint, Ypoint, st_paint.Width, st_paint.Height);
		return;
	}
	uint16_t X, Y;

	switch (st_paint.Rotate) {
	case 0:
		X = Xpoint;
		Y = Ypoint;
		break;
	case 90:
		X = st_paint.WidthMemory - Ypoint - 1;
		Y = Xpoint;
		break;
	case 180:
		X = st_paint.WidthMemory - Xpoint - 1;
		Y = st_paint.HeightMemory - Ypoint - 1;
		break;
	case 270:
		X = Ypoint;
		Y = st_paint.HeightMemory - Xpoint - 1;
		break;
	default:
		return;
	}

	switch (st_paint.Mirror) {
	case MIRROR_NONE:
		break;
	case MIRROR_HORIZONTAL:
		X = st_paint.WidthMemory - X - 1;
		break;
	case MIRROR_VERTICAL:
		Y = st_paint.HeightMemory - Y - 1;
		break;
	case MIRROR_ORIGIN:
		X = st_paint.WidthMemory - X - 1;
		Y = st_paint.HeightMemory - Y - 1;
		break;
	default:
		return;
	}

	if (X > st_paint.WidthMemory || Y > st_paint.HeightMemory) {
		printf("Exceeding display boundaries (X,Y)=(%d, %d) (W, H)= (%d, %d)\r\n",
		      X, Y, st_paint.WidthMemory, st_paint.WidthMemory);
		return;
	}

	if (st_paint.Scale == 2) {
		uint32_t Addr = X / 8 + Y * st_paint.WidthByte;
		uint8_t Rdata = st_paint.Image[Addr];
		if (Color == BLACK)
			st_paint.Image[Addr] = Rdata & ~(0x80 >> (X % 8));
		else
			st_paint.Image[Addr] = Rdata | (0x80 >> (X % 8));
	} else if (st_paint.Scale == 4) {
		uint32_t Addr = X / 4 + Y * st_paint.WidthByte;
		Color = Color % 4; //Guaranteed color scale is 4  --- 0~3
		uint8_t Rdata = st_paint.Image[Addr];

		Rdata = Rdata & (~(0xC0 >> ((X % 4) * 2)));
		st_paint.Image[Addr] = Rdata | ((Color << 6) >> ((X % 4) * 2));
	} else if (st_paint.Scale == 16) {
		uint32_t Addr = X / 2 + Y * st_paint.WidthByte;
		uint8_t Rdata = st_paint.Image[Addr];
		Color = Color % 16;
		Rdata = Rdata & (~(0xf0 >> ((X % 2) * 4)));
		st_paint.Image[Addr] = Rdata | ((Color << 4) >> ((X % 2) * 4));
	} else if (st_paint.Scale == 65) {
		uint32_t Addr = X * 2 + Y * st_paint.WidthByte;
		st_paint.Image[Addr] = 0xff & (Color >> 8);
		st_paint.Image[Addr + 1] = 0xff & Color;
	}
}

/******************************************************************************
function: Clear the color of the picture
parameter:
    Color : Painted colors
******************************************************************************/
void Paint_Clear(uint16_t Color)
{
	if (st_paint.Scale == 2 || st_paint.Scale == 4) {
		for (uint16_t Y = 0; Y < st_paint.HeightByte; Y++) {
			for (uint16_t X = 0; X < st_paint.WidthByte;
			     X++) { //8 pixel =  1 byte
				uint32_t Addr = X + Y * st_paint.WidthByte;
				st_paint.Image[Addr] = Color;
			}
		}
	} else if (st_paint.Scale == 16) {
		for (uint16_t Y = 0; Y < st_paint.HeightByte; Y++) {
			for (uint16_t X = 0; X < st_paint.WidthByte;
			     X++) { //8 pixel =  1 byte
				uint32_t Addr = X + Y * st_paint.WidthByte;
				Color = Color & 0x0f;
				st_paint.Image[Addr] = (Color << 4) | Color;
			}
		}
	} else if (st_paint.Scale == 65) {
		for (uint16_t Y = 0; Y < st_paint.HeightByte; Y++) {
			for (uint16_t X = 0; X < st_paint.WidthByte;
			     X++) { //8 pixel =  1 byte
				uint32_t Addr = X * 2 + Y * st_paint.WidthByte;
				st_paint.Image[Addr] = 0x0f & (Color >> 8);
				st_paint.Image[Addr + 1] = 0x0f & Color;
			}
		}
	}
}

/******************************************************************************
function: Clear the color of a window
parameter:
    Xstart : x starting point
    Ystart : Y starting point
    Xend   : x end point
    Yend   : y end point
    Color  : Painted colors
******************************************************************************/
void Paint_ClearWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend,
			uint16_t Color)
{
	uint16_t X, Y;
	for (Y = Ystart; Y < Yend; Y++) {
		for (X = Xstart; X < Xend; X++) { //8 pixel =  1 byte
			Paint_SetPixel(X, Y, Color);
		}
	}
}

/******************************************************************************
function: Draw Point(Xpoint, Ypoint) Fill the color
parameter:
    Xpoint		: The Xpoint coordinate of the point
    Ypoint		: The Ypoint coordinate of the point
    Color		: Painted color
    Dot_Pixel	: point size
    Dot_Style	: point Style
******************************************************************************/
void Paint_DrawPoint(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color,
		     DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_Style)
{
	if (Xpoint > st_paint.Width || Ypoint > st_paint.Height) {
		printf("Paint_DrawPoint Input exceeds the normal display range\r\n");
		printf("Xpoint = %d , st_paint.Width = %d  \r\n ", Xpoint,
		       st_paint.Width);
		printf("Ypoint = %d , st_paint.Height = %d  \r\n ", Ypoint,
		       st_paint.Height);
		return;
	}

	int16_t XDir_Num, YDir_Num;
	if (Dot_Style == DOT_FILL_AROUND) {
		for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++) {
			for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1;
			     YDir_Num++) {
				if ((((Xpoint + XDir_Num) < Dot_Pixel)) ||
				   ((((Ypoint + YDir_Num) < Dot_Pixel))))
					break;
				// printf("x = %d, y = %d\r\n", Xpoint + XDir_Num - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel);
				Paint_SetPixel(Xpoint + XDir_Num - Dot_Pixel,
					       Ypoint + YDir_Num - Dot_Pixel,
					       Color);
			}
		}
	} else {
		for (XDir_Num = 0; XDir_Num < Dot_Pixel; XDir_Num++) {
			for (YDir_Num = 0; YDir_Num < Dot_Pixel; YDir_Num++) {
				Paint_SetPixel(Xpoint + XDir_Num - 1,
					       Ypoint + YDir_Num - 1, Color);
			}
		}
	}
}

/******************************************************************************
function: Draw a line of arbitrary slope
parameter:
    Xstart ：Starting Xpoint point coordinates
    Ystart ：Starting Xpoint point coordinates
    Xend   ：End point Xpoint coordinate
    Yend   ：End point Ypoint coordinate
    Color  ：The color of the line segment
    Line_width : Line width
    Line_Style: Solid and dotted lines
******************************************************************************/
void Paint_DrawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend,
		    uint16_t Color, DOT_PIXEL Line_width, LINE_STYLE Line_Style)
{
	if (Xstart > st_paint.Width || Ystart > st_paint.Height ||
	    Xend > st_paint.Width || Yend > st_paint.Height) {
		printf("Paint_DrawLine Input exceeds the normal display range\r\n");
		return;
	}

	uint16_t Xpoint = Xstart;
	uint16_t Ypoint = Ystart;

	int dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
	int dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

	// Increment direction, 1 is positive, -1 is counter;
	int XAddway = Xstart < Xend ? 1 : -1;
	int YAddway = Ystart < Yend ? 1 : -1;

	//Cumulative error
	int Esp = dx + dy;
	char Dotted_Len = 0;

	for (;;) {
		Dotted_Len++;
		//Painted dotted line, 2 point is really virtual
		if (Line_Style == LINE_STYLE_DOTTED && Dotted_Len % 3 == 0) {
			//printf("LINE_DOTTED\r\n");
			if (Color)
				Paint_DrawPoint(Xpoint, Ypoint, BLACK,
						Line_width, DOT_STYLE_DFT);
			else
				Paint_DrawPoint(Xpoint, Ypoint, WHITE,
						Line_width, DOT_STYLE_DFT);
			Dotted_Len = 0;
		} else {
			Paint_DrawPoint(Xpoint, Ypoint, Color, Line_width,
					DOT_STYLE_DFT);
		}
		if (2 * Esp >= dy) {
			if (Xpoint == Xend)
				break;
			Esp += dy;
			Xpoint += XAddway;
		}
		if (2 * Esp <= dx) {
			if (Ypoint == Yend)
				break;
			Esp += dx;
			Ypoint += YAddway;
		}
	}
}

/******************************************************************************
function: Draw a rectangle
parameter:
    Xstart ：Rectangular  Starting Xpoint point coordinates
    Ystart ：Rectangular  Starting Xpoint point coordinates
    Xend   ：Rectangular  End point Xpoint coordinate
    Yend   ：Rectangular  End point Ypoint coordinate
    Color  ：The color of the Rectangular segment
    Line_width: Line width
    Draw_Fill : Whether to fill the inside of the rectangle
******************************************************************************/
void Paint_DrawRectangle(uint16_t Xstart, uint16_t Ystart, uint16_t Xend, uint16_t Yend,
			 uint16_t Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
{
	if (Xstart > st_paint.Width || Ystart > st_paint.Height ||
	    Xend > st_paint.Width || Yend > st_paint.Height) {
		printf("Input exceeds the normal display range\r\n");
		return;
	}

	if (Draw_Fill) {
		uint16_t Ypoint;
		for (Ypoint = Ystart; Ypoint < Yend; Ypoint++) {
			Paint_DrawLine(Xstart, Ypoint, Xend, Ypoint, Color,
				       Line_width, LINE_STYLE_SOLID);
		}
	} else {
		Paint_DrawLine(Xstart, Ystart, Xend, Ystart, Color, Line_width,
			       LINE_STYLE_SOLID);
		Paint_DrawLine(Xstart, Ystart, Xstart, Yend, Color, Line_width,
			       LINE_STYLE_SOLID);
		Paint_DrawLine(Xend, Yend, Xend, Ystart, Color, Line_width,
			       LINE_STYLE_SOLID);
		Paint_DrawLine(Xend, Yend, Xstart, Yend, Color, Line_width,
			       LINE_STYLE_SOLID);
	}
}

/******************************************************************************
function: Use the 8-point method to draw a circle of the
            specified size at the specified position->
parameter:
    X_Center  ：Center X coordinate
    Y_Center  ：Center Y coordinate
    Radius    ：circle Radius
    Color     ：The color of the ：circle segment
    Line_width: Line width
    Draw_Fill : Whether to fill the inside of the Circle
******************************************************************************/
void Paint_DrawCircle(uint16_t X_Center, uint16_t Y_Center, uint16_t Radius, uint16_t Color,
		      DOT_PIXEL Line_width, DRAW_FILL Draw_Fill)
{
	if (X_Center > st_paint.Width || Y_Center >= st_paint.Height) {
		printf("Paint_DrawCircle Input exceeds the normal display range\r\n");
		return;
	}

	//Draw a circle from(0, R) as a starting point
	int16_t XCurrent, YCurrent;
	XCurrent = 0;
	YCurrent = Radius;

	//Cumulative error,judge the next point of the logo
	int16_t Esp = 3 - (Radius << 1);

	int16_t sCountY;
	if (Draw_Fill == DRAW_FILL_FULL) {
		while (XCurrent <= YCurrent) { //Realistic circles
			for (sCountY = XCurrent; sCountY <= YCurrent;
			     sCountY++) {
				Paint_DrawPoint(X_Center + XCurrent,
						Y_Center + sCountY, Color,
						DOT_PIXEL_DFT,
						DOT_STYLE_DFT); //1
				Paint_DrawPoint(X_Center - XCurrent,
						Y_Center + sCountY, Color,
						DOT_PIXEL_DFT,
						DOT_STYLE_DFT); //2
				Paint_DrawPoint(X_Center - sCountY,
						Y_Center + XCurrent, Color,
						DOT_PIXEL_DFT,
						DOT_STYLE_DFT); //3
				Paint_DrawPoint(X_Center - sCountY,
						Y_Center - XCurrent, Color,
						DOT_PIXEL_DFT,
						DOT_STYLE_DFT); //4
				Paint_DrawPoint(X_Center - XCurrent,
						Y_Center - sCountY, Color,
						DOT_PIXEL_DFT,
						DOT_STYLE_DFT); //5
				Paint_DrawPoint(X_Center + XCurrent,
						Y_Center - sCountY, Color,
						DOT_PIXEL_DFT,
						DOT_STYLE_DFT); //6
				Paint_DrawPoint(X_Center + sCountY,
						Y_Center - XCurrent, Color,
						DOT_PIXEL_DFT,
						DOT_STYLE_DFT); //7
				Paint_DrawPoint(X_Center + sCountY,
						Y_Center + XCurrent, Color,
						DOT_PIXEL_DFT, DOT_STYLE_DFT);
			}
			if (Esp < 0)
				Esp += 4 * XCurrent + 6;
			else {
				Esp += 10 + 4 * (XCurrent - YCurrent);
				YCurrent--;
			}
			XCurrent++;
		}
	} else { //Draw a hollow circle
		while (XCurrent <= YCurrent) {
			Paint_DrawPoint(X_Center + XCurrent,
					Y_Center + YCurrent, Color, Line_width,
					DOT_STYLE_DFT); //1
			Paint_DrawPoint(X_Center - XCurrent,
					Y_Center + YCurrent, Color, Line_width,
					DOT_STYLE_DFT); //2
			Paint_DrawPoint(X_Center - YCurrent,
					Y_Center + XCurrent, Color, Line_width,
					DOT_STYLE_DFT); //3
			Paint_DrawPoint(X_Center - YCurrent,
					Y_Center - XCurrent, Color, Line_width,
					DOT_STYLE_DFT); //4
			Paint_DrawPoint(X_Center - XCurrent,
					Y_Center - YCurrent, Color, Line_width,
					DOT_STYLE_DFT); //5
			Paint_DrawPoint(X_Center + XCurrent,
					Y_Center - YCurrent, Color, Line_width,
					DOT_STYLE_DFT); //6
			Paint_DrawPoint(X_Center + YCurrent,
					Y_Center - XCurrent, Color, Line_width,
					DOT_STYLE_DFT); //7
			Paint_DrawPoint(X_Center + YCurrent,
					Y_Center + XCurrent, Color, Line_width,
					DOT_STYLE_DFT); //0

			if (Esp < 0)
				Esp += 4 * XCurrent + 6;
			else {
				Esp += 10 + 4 * (XCurrent - YCurrent);
				YCurrent--;
			}
			XCurrent++;
		}
	}
}

/******************************************************************************
function: Show English characters
parameter:
    Xpoint           ：X coordinate
    Ypoint           ：Y coordinate
    Acsii_Char       ：To display the English characters
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void Paint_DrawChar(uint16_t Xpoint, uint16_t Ypoint, const char Acsii_Char,
		    sFONT *Font, uint16_t Color_Foreground, uint16_t Color_Background)
{
	uint16_t Page, Column;

	if (Xpoint > st_paint.Width || Ypoint > st_paint.Height) {
		printf("Paint_DrawChar Input exceeds the normal display range\r\n");
		return;
	}

	uint32_t Char_Offset = (Acsii_Char - ' ') * Font->Height *
			       (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
	const unsigned char *ptr = &Font->table[Char_Offset];

	for (Page = 0; Page < Font->Height; Page++) {
		for (Column = 0; Column < Font->Width; Column++) {
			//To determine whether the font background color and screen background color is consistent
			if (FONT_BACKGROUND ==
			    Color_Background) { //this process is to speed up the scan
				if (*ptr & (0x80 >> (Column % 8)))
					Paint_SetPixel(Xpoint + Column,
						       Ypoint + Page,
						       Color_Foreground);
				// Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
			} else {
				if (*ptr & (0x80 >> (Column % 8))) {
					Paint_SetPixel(Xpoint + Column,
						       Ypoint + Page,
						       Color_Foreground);
					// Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
				} else {
					Paint_SetPixel(Xpoint + Column,
						       Ypoint + Page,
						       Color_Background);
					// Paint_DrawPoint(Xpoint + Column, Ypoint + Page, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
				}
			}
			//One pixel is 8 bits
			if (Column % 8 == 7)
				ptr++;
		} // Write a line
		if (Font->Width % 8 != 0)
			ptr++;
	} // Write all
}

/******************************************************************************
function:	Display the string
parameter:
    Xstart           ：X coordinate
    Ystart           ：Y coordinate
    pString          ：The first address of the English string to be displayed
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void Paint_DrawString_EN(uint16_t Xstart, uint16_t Ystart, const char *pString,
			 sFONT *Font, uint16_t Color_Foreground,
			 uint16_t Color_Background)
{
	uint16_t Xpoint = Xstart;
	uint16_t Ypoint = Ystart;

	if (Xstart > st_paint.Width || Ystart > st_paint.Height) {
		printf("Paint_DrawString_EN Input exceeds the normal display range\r\n");
		return;
	}

	while (*pString != '\0') {
		//if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the Height of the character
		if ((Xpoint + Font->Width) > st_paint.Width) {
			Xpoint = Xstart;
			Ypoint += Font->Height;
		}

		// If the Y direction is full, reposition to(Xstart, Ystart)
		if ((Ypoint + Font->Height) > st_paint.Height) {
			Xpoint = Xstart;
			Ypoint = Ystart;
		}
		Paint_DrawChar(Xpoint, Ypoint, *pString, Font, Color_Background,
			       Color_Foreground);

		//The next character of the address
		pString++;

		//The next word of the abscissa increases the font of the broadband
		Xpoint += Font->Width;
	}
}

/******************************************************************************
function: Display the string
parameter:
    Xstart  ：X coordinate
    Ystart  ：Y coordinate
    pString ：The first address of the Chinese string and English
              string to be displayed
    Font    ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void Paint_DrawString_CN(uint16_t Xstart, uint16_t Ystart, const char *pString,
			 cFONT *font, uint16_t Color_Foreground,
			 uint16_t Color_Background)
{
	const char *p_text = pString;
	int x = Xstart, y = Ystart;
	int i, j, Num;

	/* Send the string character by character on EPD */
	while (*p_text != 0) {
		if (*p_text <= 0x7F) { //ASCII < 126
			for (Num = 0; Num < font->size; Num++) {
				if (*p_text == font->table[Num].index[0]) {
					const char *ptr =
						&font->table[Num].matrix[0];

					for (j = 0; j < font->Height; j++) {
						for (i = 0; i < font->Width;
						     i++) {
							if (FONT_BACKGROUND ==
							    Color_Background) { //this process is to speed up the scan
								if (*ptr &
								    (0x80 >>
								     (i % 8))) {
									Paint_SetPixel(
										x + i,
										y + j,
										Color_Foreground);
									// Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
								}
							} else {
								if (*ptr &
								    (0x80 >>
								     (i % 8))) {
									Paint_SetPixel(
										x + i,
										y + j,
										Color_Foreground);
									// Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
								} else {
									Paint_SetPixel(
										x + i,
										y + j,
										Color_Background);
									// Paint_DrawPoint(x + i, y + j, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
								}
							}
							if (i % 8 == 7) {
								ptr++;
							}
						}
						if (font->Width % 8 != 0) {
							ptr++;
						}
					}
					break;
				}
			}
			/* Point on the next character */
			p_text += 1;
			/* Decrement the column position by 16 */
			x += font->ASCII_Width;
		} else { //Chinese
			for (Num = 0; Num < font->size; Num++) {
				if ((*p_text == font->table[Num].index[0]) &&
				    (*(p_text + 1) ==
				     font->table[Num].index[1])) {
					const char *ptr =
						&font->table[Num].matrix[0];

					for (j = 0; j < font->Height; j++) {
						for (i = 0; i < font->Width;
						     i++) {
							if (FONT_BACKGROUND ==
							    Color_Background) { //this process is to speed up the scan
								if (*ptr &
								    (0x80 >>
								     (i % 8))) {
									Paint_SetPixel(
										x + i,
										y + j,
										Color_Foreground);
									// Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
								}
							} else {
								if (*ptr &
								    (0x80 >>
								     (i % 8))) {
									Paint_SetPixel(
										x + i,
										y + j,
										Color_Foreground);
									// Paint_DrawPoint(x + i, y + j, Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
								} else {
									Paint_SetPixel(
										x + i,
										y + j,
										Color_Background);
									// Paint_DrawPoint(x + i, y + j, Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
								}
							}
							if (i % 8 == 7) {
								ptr++;
							}
						}
						if (font->Width % 8 != 0) {
							ptr++;
						}
					}
					break;
				}
			}
			/* Point on the next character */
			p_text += 2;
			/* Decrement the column position by 16 */
			x += font->Width;
		}
	}
}

/******************************************************************************
function:	Display nummber
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    Nummber          : The number displayed
    Font             ：A structure pointer that displays a character size
	Digit						 : Fractional width
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
#define ARRAY_LEN 255
void Paint_DrawNum(uint16_t Xpoint, uint16_t Ypoint, double Nummber,
		   sFONT *Font, uint16_t Digit, uint16_t Color_Foreground,
		   uint16_t Color_Background)
{
	int16_t Num_Bit = 0, Str_Bit = 0;
	uint8_t Str_Array[ARRAY_LEN] = { 0 }, Num_Array[ARRAY_LEN] = { 0 };
	uint8_t *pStr = Str_Array;
	int temp = Nummber;
	float decimals;
	uint8_t i;
	if (Xpoint > st_paint.Width || Ypoint > st_paint.Height) {
		printf("Paint_DisNum Input exceeds the normal display range\r\n");
		return;
	}
	if (Digit > 0) {
		decimals = Nummber - temp;
		for (i = Digit; i > 0; i--) {
			decimals *= 10;
		}
		temp = decimals;
		//Converts a number to a string
		for (i = Digit; i > 0; i--) {
			Num_Array[Num_Bit] = temp % 10 + '0';
			Num_Bit++;
			temp /= 10;
		}
		Num_Array[Num_Bit] = '.';
		Num_Bit++;
	}

	temp = Nummber;
	//Converts a number to a string
	while (temp) {
		Num_Array[Num_Bit] = temp % 10 + '0';
		Num_Bit++;
		temp /= 10;
	}

	//The string is inverted
	while (Num_Bit > 0) {
		Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
		Str_Bit++;
		Num_Bit--;
	}

	//show
	Paint_DrawString_EN(Xpoint, Ypoint, (const char *)pStr, Font,
			    Color_Background, Color_Foreground);
}

/******************************************************************************
function:	Display time
parameter:
    Xstart           ：X coordinate
    Ystart           : Y coordinate
    pTime            : Time-related structures
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void Paint_DrawTime(uint16_t Xstart, uint16_t Ystart, PAINT_TIME *pTime, sFONT *Font,
		    uint16_t Color_Foreground, uint16_t Color_Background)
{
	uint8_t value[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

	uint16_t Dx = Font->Width;

	//Write data into the cache
	Paint_DrawChar(Xstart, Ystart, value[pTime->Hour / 10], Font,
		       Color_Background, Color_Foreground);
	Paint_DrawChar(Xstart + Dx, Ystart, value[pTime->Hour % 10], Font,
		       Color_Background, Color_Foreground);
	Paint_DrawChar(Xstart + Dx + Dx / 4 + Dx / 2, Ystart, ':', Font,
		       Color_Background, Color_Foreground);
	Paint_DrawChar(Xstart + Dx * 2 + Dx / 2, Ystart, value[pTime->Min / 10],
		       Font, Color_Background, Color_Foreground);
	Paint_DrawChar(Xstart + Dx * 3 + Dx / 2, Ystart, value[pTime->Min % 10],
		       Font, Color_Background, Color_Foreground);
	Paint_DrawChar(Xstart + Dx * 4 + Dx / 2 - Dx / 4, Ystart, ':', Font,
		       Color_Background, Color_Foreground);
	Paint_DrawChar(Xstart + Dx * 5, Ystart, value[pTime->Sec / 10], Font,
		       Color_Background, Color_Foreground);
	Paint_DrawChar(Xstart + Dx * 6, Ystart, value[pTime->Sec % 10], Font,
		       Color_Background, Color_Foreground);
}

/******************************************************************************
function:	Display monochrome bitmap
parameter:
    image_buffer ：A picture data converted to a bitmap
info:
    Use a computer to convert the image into a corresponding array,
    and then embed the array directly into Imagedata.cpp as a .c file.
******************************************************************************/
void Paint_DrawBitMap(const unsigned char *image_buffer)
{
	uint16_t x, y;
	uint32_t Addr = 0;

	for (y = 0; y < st_paint.HeightByte; y++) {
		for (x = 0; x < st_paint.WidthByte; x++) { //8 pixel =  1 byte
			Addr = x + y * st_paint.WidthByte;
			st_paint.Image[Addr] = (unsigned char)image_buffer[Addr];
		}
	}
}

void Paint_DrawBitMap_Block(const unsigned char *image_buffer, uint8_t Region)
{
	uint16_t x, y;
	uint32_t Addr = 0;
	for (y = 0; y < st_paint.HeightByte; y++) {
		for (x = 0; x < st_paint.WidthByte; x++) { //8 pixel =  1 byte
			Addr = x + y * st_paint.WidthByte;
			st_paint.Image[Addr] = (unsigned char)
				image_buffer[Addr + (st_paint.HeightByte) *
							    st_paint.WidthByte *
							    (Region - 1)];
		}
	}
}
