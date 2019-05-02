//========================================================================================
//
//	Edge-lit Hexagon Frame
//
//
//========================================================================================

#include <Adafruit_NeoPixel.h>

#define PIN      6
#define LED_COUNT 60

Adafruit_NeoPixel hexring = Adafruit_NeoPixel(LED_COUNT, PIN, NEO_GRB + NEO_KHZ800);

//----------------------------------------------------------------------------------------
//	Geometry
//----------------------------------------------------------------------------------------
#define LEDS_PER_SIDE (LED_COUNT/6)

//----------------------------------------------------------------------------------------
//	Fractional pixels
//----------------------------------------------------------------------------------------
#define FIXEDPT_SHIFT 4
#define FIXEDPT_MASK ((1 << FIXEDPT_SHIFT) - 1)

void SetFractPixel(Adafruit_NeoPixel& afnp, uint16_t fpOffset, uint8_t fpWidth, uint32_t fpColor, uint8_t op);

// Some convenient constants
#define ONE_LED		(1 << FIXEDPT_SHIFT)
#define TWO_LEDS	(2 << FIXEDPT_SHIFT)
#define THREE_LEDS	(3 << FIXEDPT_SHIFT)
#define FOUR_LEDS	(4 << FIXEDPT_SHIFT)
#define HALF_LEDS	(LEDS_PER_SIDE/2 << FIXEDPT_SHIFT)
#define SIDE_LEDS	(LEDS_PER_SIDE << FIXEDPT_SHIFT)

//----------------------------------------------------------------------------------------
//	Color Operations
//----------------------------------------------------------------------------------------
#define OP_SET   0
#define OP_BLEND 1
#define OP_MAX   2

void UpdateColor(Adafruit_NeoPixel& afnp, uint16_t index, uint32_t color, uint8_t op, uint32_t value = 0);

//----------------------------------------------------------------------------------------
//	Color sequences
//----------------------------------------------------------------------------------------
uint32_t seqRRGGBB[6] = {0x400000, 0x400000, 0x004000, 0x004000, 0x000040, 0x000040};
uint32_t seqRGRGRG[6] = {0x100000, 0x001000, 0x100000, 0x001000, 0x100000, 0x001000};
uint32_t seqCMYCMY[6] = {0x004040, 0x400040, 0x404000, 0x004040, 0x400040, 0x404000};
uint32_t seqRYGCBM[6] = {0x400000, 0x404000, 0x004000, 0x004040, 0x000040, 0x400010};
uint32_t seqRGBRGB[6] = {0x400000, 0x004000, 0x000040, 0x400000, 0x004000, 0x000040};
uint32_t seqRoGoBo[6] = {0x600000,        0, 0x006000,        0, 0x000060,        0};
uint32_t seqoooooo[6] = {       0,        0,        0,        0,        0,        0};

uint32_t seqRooRoo[6] = {0x400000,        0,        0, 0x400000,        0,        0};
uint32_t seqoGooGo[6] = {       0, 0x004000,        0,        0, 0x004000,        0};
uint32_t seqooBooB[6] = {       0,        0, 0x000040,        0,        0, 0x000040};


typedef struct {
	uint32_t* colorList;
	uint8_t  colorWidth;
	
	int8_t  miscA;
	uint8_t  miscB;
} ColorSequenceV2;

ColorSequenceV2 csvtest[] = {
	{ seqRRGGBB, FOUR_LEDS, 1, 0}
};

typedef struct {
	uint32_t colorList[6];
	uint8_t  colorCount;
	uint8_t  colorWidth;
	
	int8_t  miscA;
	uint8_t  miscB;
} ColorSequence;

// Moving     RooRoo x1
// Moving     ooGooo x2
// Moving     ooooBo x3
ColorSequence cs01[] = {
	{{ 0x600000, 0x000000, 0x000000, 0x600000, 0x000000, 0x000000}, 6, (2 << FIXEDPT_SHIFT), 1, 0},
	{{ 0x000000, 0x000000, 0x006000, 0x000000, 0x000000, 0x000000}, 6, (2 << FIXEDPT_SHIFT), 2, 0},
	{{ 0x000000, 0x000000, 0x000000, 0x000000, 0x000060, 0x000000}, 6, (2 << FIXEDPT_SHIFT), 3, 0}
	};

// Moving     Rooooo x1
// Moving     GoooBo x2
ColorSequence cs02[] = {
	{{ 0x300000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000}, 6, FOUR_LEDS, 1, 0},
	{{ 0x003000, 0x000000, 0x000000, 0x000000, 0x000060, 0x000000}, 6, FOUR_LEDS, 2, 0}
	};

// Moving     Rooooo x1
// Moving     Gooooo x2
ColorSequence cs03[] = {
	{{ 0x300000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000}, 6, FOUR_LEDS, 1, 0},
	{{ 0x003000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000}, 6, FOUR_LEDS, 2, 0}
	};

// Moving     Rooooo x1
// Moving     Gooooo x2
// Moving     Booooo x3
ColorSequence cs04[] = {
	{{ 0x600000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000}, 6, SIDE_LEDS, 1, 0},
	{{ 0x006000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000}, 6, SIDE_LEDS, 2, 0},
	{{ 0x000060, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000}, 6, SIDE_LEDS, 3, 0}
	};

// Stationary oGoGoG
// Moving     Rooooo x1
ColorSequence cs05[] = {
	{{ 0x000000, 0x006000, 0x000000, 0x006000, 0x000000, 0x006000}, 6, SIDE_LEDS, 0, 0},
	{{ 0x600000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000}, 1, SIDE_LEDS, 1, 0}
	};

// Stationary oRoGoB
// Moving     YoMoCo x1
ColorSequence cs06[] = {
	{{ 0x000000, 0x400000, 0x000000, 0x004000, 0x000000, 0x000040}, 6, SIDE_LEDS, 0, 0},
	{{ 0x606000, 0x000000, 0x600060, 0x000000, 0x006060, 0x000000}, 6, TWO_LEDS, 1, 0}
	};


// Stationary YoMoCo
// Moving     YoMoCo x1
ColorSequence cs07[] = {
	{{ 0x303000, 0x000000, 0x300030, 0x000000, 0x003030, 0x000000}, 6, SIDE_LEDS, 0, 0},
	{{ 0x606000, 0x000000, 0x600060, 0x000000, 0x006060, 0x000000}, 6, TWO_LEDS, 1, 0}
	};

// Stationary oGoGoG
// Moving     RoRoRo x1
ColorSequence cs08[] = {
	{{ 0x000000, 0x006000, 0x000000, 0x006000, 0x000000, 0x006000}, 6, SIDE_LEDS, 0, 0},
	{{ 0x600000, 0x000000, 0x600000, 0x000000, 0x600000, 0x000000}, 6, SIDE_LEDS, 1, 0}
	};

// Moving     oGoGoG -1
// Moving     RoRoRo x1
ColorSequence cs09[] = {
	{{ 0x000000, 0x006000, 0x000000, 0x006000, 0x000000, 0x006000}, 6, HALF_LEDS,-1, 0},
	{{ 0x600000, 0x000000, 0x600000, 0x000000, 0x600000, 0x000000}, 6, HALF_LEDS, 1, 0}
	};

// Moving     Yooooo -1
ColorSequence cs10[] = {
	{{ 0x606000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000}, 6, TWO_LEDS,-1, 0}
	};

// Moving     oBoBoB -1
// Moving     RoRoRo x1
ColorSequence cs11[] = {
	{{ 0x000000, 0x000060, 0x000000, 0x000060, 0x000000, 0x000060}, 6, HALF_LEDS,-1, 0},
	{{ 0x600000, 0x000000, 0x600000, 0x000000, 0x600000, 0x000000}, 6, HALF_LEDS, 1, 0}
	};

// Moving     Yooooo x1
// Moving     Mooooo x2
// Moving     Cooooo x3
ColorSequence cs12[] = {
	{{ 0x606000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000}, 6, HALF_LEDS, 1, 0},
	{{ 0x600060, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000}, 6, HALF_LEDS, 2, 0},
	{{ 0x006060, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000}, 6, HALF_LEDS, 3, 0}
	};

//----------------------------------------------------------------------------------------
//	Setup
//----------------------------------------------------------------------------------------
void setup()
{
	hexring.begin();

	//Serial.begin(115200);
}


//----------------------------------------------------------------------------------------
//	Loop
//----------------------------------------------------------------------------------------
void loop()
{
	TestAnimation();
}

//----------------------------------------------------------------------------------------
//	Individual tests
//----------------------------------------------------------------------------------------
void TestAnimation(void)
{
	
	//RotateColors(seqRYGCBM, 6, (2 << FIXEDPT_SHIFT), 300);

	//uint32_t seq03[6] = {0x400000, 0x404000, 0x004000, 0x00404, 0x000040, 0x400040};
	//RotateColors(seqRYGCBM, 6, ONE_LED, 1500);
	//DualRotateColors(seqRYGCBM, 6, seqRRGGBB, 6, ONE_LED, 100);
	//
	//uint32_t greenColor = 0x004000;
	//BackAndForth(&greenColor, 1, ONE_LED, (6 * LEDS_PER_SIDE << FIXEDPT_SHIFT), 1500);
	//uint32_t redColor = 0x600000;
	//BackAndForth(&redColor, 1, ONE_LED, (12 * LEDS_PER_SIDE << FIXEDPT_SHIFT), 1500);
	//BackAndForth(seqRoGoBo, 6, ONE_LED, (12 * LEDS_PER_SIDE << FIXEDPT_SHIFT), 1500);
	//BackAndForth(seqRGBRGB, 6, (LEDS_PER_SIDE << FIXEDPT_SHIFT), (2 * LEDS_PER_SIDE << FIXEDPT_SHIFT), 500);
	//BackAndForth(seqRYGCBM, 6, (3 << FIXEDPT_SHIFT), (2 * LEDS_PER_SIDE << FIXEDPT_SHIFT), 300);
	
	//fullColorsTest();
	
	MultiRotateColors(cs04, sizeof(cs04)/sizeof(*cs04), 100);
	//MultiRotateColors(cs09, 2, 250);
	//MultiRotateColors(cs11, 2, 250);
	//MultiRotateColors(cs12, 3, 250);
	//MultiRotateColors(cs07, 2, 100);
	
	uint32_t* sequenceList1[] = {seqoooooo, seqRYGCBM, seqCMYCMY, seqRRGGBB, seqRGBRGB};
	//FadeTriangleSequences(sequenceList1, sizeof(sequenceList1)/sizeof(*sequenceList1), 10, 4000);

	//uint32_t* sequenceList2[] = {seqRooRoo, seqoGooGo, seqooBooB};
	//FadeTriangleSequences(sequenceList2, sizeof(sequenceList2)/sizeof(*sequenceList2), 10, 4000);
	//delay(4000);
	
	//FadeTriangleColors(seqoooooo, seqRYGCBM, 10);
	//delay(4000);
	//FadeTriangleColors(seqRYGCBM, seqRRGGBB, 10);
	//delay(4000);
}

//----------------------------------------------------------------------------------------
//	Log Config
//----------------------------------------------------------------------------------------
void LogConfig(void)
{
	Serial.println("hexframe configuration");
	Serial.print("  LED_COUNT: ");
	Serial.println(LED_COUNT);
	Serial.print("  FIXEDPT_SHIFT: ");
	Serial.println(FIXEDPT_SHIFT);
	Serial.print("  FIXEDPT_MASK: ");
	Serial.println(FIXEDPT_MASK);
}



//========================================================================================
//	Color Animations
//========================================================================================

//----------------------------------------------------------------------------------------
//	Fade Triangle Sequences
//		Sets the colors of each side for a few seconds, blanks, 
//		continues with the next color sequence
//----------------------------------------------------------------------------------------
void FadeTriangleSequences(uint32_t** sequencesList, uint8_t sequencesCount, uint16_t fadeDelay, uint16_t holdDelay)
{
	for (int i = 0; i < sequencesCount; i++)
	{
		uint8_t next = (i + 1) % sequencesCount;
		FadeTriangleColors(sequencesList[i], sequencesList[next], fadeDelay);
		
		if (i < sequencesCount - 1)
			delay(holdDelay);
	}
}

//----------------------------------------------------------------------------------------
//	Rotate Colors
//----------------------------------------------------------------------------------------
void RotateColors(uint32_t* colorList, uint8_t colorCount, uint8_t colorWidth, uint16_t delayTime)
{
	uint8_t spacing = (LED_COUNT << FIXEDPT_SHIFT)/colorCount;
	
	for (uint16_t step = 0; step < (LED_COUNT << FIXEDPT_SHIFT); step++)
	{
		hexring.clear();
		
		for (uint8_t colorIdx = 0; colorIdx < colorCount; colorIdx++)
		{
			uint16_t offset = (step + colorIdx * spacing) % (LED_COUNT << FIXEDPT_SHIFT);
			SetFractPixel(hexring, offset, colorWidth, colorList[colorIdx], OP_SET);
		}
		
		hexring.show();
		delay(delayTime >> FIXEDPT_SHIFT);
	}
}

//----------------------------------------------------------------------------------------
//	Dual Rotate Colors
//----------------------------------------------------------------------------------------
void DualRotateColors(uint32_t* colorListA, uint8_t colorCountA, uint32_t* colorListB, uint8_t colorCountB, uint8_t colorWidth, uint16_t delayTime)
{
	uint8_t spacingA = (LED_COUNT << FIXEDPT_SHIFT)/colorCountA;
	uint8_t spacingB = (LED_COUNT << FIXEDPT_SHIFT)/colorCountB;
	
	for (uint16_t step = 0; step < (LED_COUNT << FIXEDPT_SHIFT); step++)
	{
		hexring.clear();
		
		for (uint8_t colorIdx = 0; colorIdx < colorCountA; colorIdx++)
		{
			uint16_t offset = (step + colorIdx * spacingA) % (LED_COUNT << FIXEDPT_SHIFT);
			SetFractPixel(hexring, offset, colorWidth, colorListA[colorIdx], OP_SET);
		}
		
		for (uint8_t colorIdx = 0; colorIdx < colorCountB; colorIdx++)
		{
			uint16_t offset = (((LED_COUNT - 1) << FIXEDPT_SHIFT) - step + colorIdx * spacingB) % (LED_COUNT << FIXEDPT_SHIFT);
			SetFractPixel(hexring, offset, colorWidth, colorListB[colorIdx], OP_MAX);
		}
		
		hexring.show();
		delay(delayTime >> FIXEDPT_SHIFT);
	}
}

//----------------------------------------------------------------------------------------
//	Multi-Rotate Colors
//		Rotate at various speeds and overlay multiple lists of colors
//----------------------------------------------------------------------------------------
void MultiRotateColors(ColorSequence* sequenceList, uint8_t sequenceCount, uint16_t delayTime)
{
	for (uint16_t step = 0; step < (LED_COUNT << FIXEDPT_SHIFT); step++)
	{
		hexring.clear();
		
		for (uint8_t seqIdx = 0; seqIdx < sequenceCount; seqIdx++)
		{
			ColorSequence* seq = sequenceList + seqIdx;
			uint8_t spacing = (LED_COUNT << FIXEDPT_SHIFT)/seq->colorCount;
			
			uint8_t speed = abs(seq->miscA);			
			uint16_t base = step * speed;
			
			if (seq->miscA < 0)
				base =  (LED_COUNT << FIXEDPT_SHIFT) * speed - base - 1;
				
			for (uint8_t colorIdx = 0; colorIdx < seq->colorCount; colorIdx++)
			{
				uint16_t offset = (base + colorIdx * spacing) % (LED_COUNT << FIXEDPT_SHIFT);
				SetFractPixel(hexring, offset, seq->colorWidth, seq->colorList[colorIdx], OP_MAX);
			}
		}
		hexring.show();
		delay(delayTime >> FIXEDPT_SHIFT);
	}
}


//----------------------------------------------------------------------------------------
//	BackAndForth
//		Use a sine function to move the LEDs back and forth
//----------------------------------------------------------------------------------------
void BackAndForth(uint32_t* colorList, uint8_t colorCount, uint8_t colorWidth, uint16_t colorSweep, uint16_t delayTime)
{
	uint8_t spacing = (LED_COUNT << FIXEDPT_SHIFT)/colorCount;
	
	// maxSteps is the number of steps to complete the full back and forth. The value is
	// somewhat arbitrary. 
	uint16_t maxSteps = (LEDS_PER_SIDE << FIXEDPT_SHIFT);
	
	// maxBase is how far along the color will be at the maximum "displacement"
	uint16_t maxBase = colorSweep - colorWidth;
	
	for (uint16_t step = 0; step < maxSteps; step++)
	{
		hexring.clear();
		
		uint16_t base = (maxBase - (float)maxBase * cos((step * 3.141592 * 2) / (maxSteps - 1))) / 2;
		
		for (uint8_t colorIdx = 0; colorIdx < colorCount; colorIdx++)
		{
			uint16_t offset = (base + colorIdx * spacing) % (LED_COUNT << FIXEDPT_SHIFT);
			SetFractPixel(hexring, offset, colorWidth, colorList[colorIdx], OP_MAX);
		}
		
		hexring.show();
		delay(delayTime >> FIXEDPT_SHIFT);
	}
}


//----------------------------------------------------------------------------------------
//	Fade Triangle Colors
//		Sets each triangle to a color
//		colors: list of 6 colors
//----------------------------------------------------------------------------------------
void FadeTriangleColors(uint32_t* colorsA, uint32_t* colorsB, uint16_t delayTime)
{
	for (uint16_t alpha = 0; alpha <= 255; alpha++)
	{
		hexring.clear();
	
		for (uint8_t side = 0; side < 6; side++)
		{
			uint32_t color = CalcAlphaBlendColor(colorsB[side], colorsA[side], alpha);
			UpdateSideColor(side, color, OP_SET);
		}
		
		hexring.show();
		delay(delayTime);
	}
}


//========================================================================================
//	HexRing Utilities
//		These routines are aware of the geometry of the LEDs
//========================================================================================

//----------------------------------------------------------------------------------------
//	All Off
//----------------------------------------------------------------------------------------
void AllOff(void)
{
	hexring.clear();
	hexring.show();
}

//----------------------------------------------------------------------------------------
//	Set Triangle Colors
//		Sets each triangle to a color
//		colors: list of 6 colors
//----------------------------------------------------------------------------------------
void SetTriangleColors(uint32_t* colors)
{
	hexring.clear();
	
	for (uint8_t side = 0; side < 6; side++)
		UpdateSideColor(side, colors[side], OP_SET);
		
	hexring.show();
}

//----------------------------------------------------------------------------------------
//	Side Colors
//		Sets the color of one side of the hexagon
//----------------------------------------------------------------------------------------
void UpdateSideColor(uint8_t sideIdx, uint32_t color, uint8_t op)
{
	for (uint16_t i = sideIdx * LEDS_PER_SIDE; i < (sideIdx + 1) * LEDS_PER_SIDE; i++)
		UpdateColor(hexring, i, color, op);
}

//========================================================================================
//	Utility Routines -- Update NeoPixels
//========================================================================================

//----------------------------------------------------------------------------------------
//	Set Fractional Pixel
//
//	        ..............fpOffset
//          v             
//	+----+----+----+----+
//	|    |  ==|====|=== | LEDs
//	+----+----+----+----+
//	        ^^^^^^^^^^^   fpWidth
//----------------------------------------------------------------------------------------
void SetFractPixel(Adafruit_NeoPixel& afnp, uint16_t fpOffset, uint8_t fpWidth, uint32_t fpColor, uint8_t op)
{
	int16_t width;
	uint32_t color;

	// Index of first LED 
	uint16_t idx = (fpOffset >> FIXEDPT_SHIFT);
	
	// Fractional offset into first LED. 
	uint8_t fract = (fpOffset & FIXEDPT_MASK);
	
	// Remainder of first LED available to be lit
	uint8_t remainder = (1 << FIXEDPT_SHIFT) - fract;
	
	// Determine if the fpWidth is enough to fill the remainder
	width = (remainder < fpWidth) ? remainder : fpWidth;
	
	// Calculate a color brightness for the first LED
	color = ScaleColor(fpColor, width, (1 << FIXEDPT_SHIFT));

	// Update the NeoPixel structure
	UpdateColor(afnp, idx, color, op);

	// Calc the remaining width to be used over subsequent pixels
	width = fpWidth - width;
	
	// Any width left?
	while (width > 0)
	{
		// Are we filling the entire LED or part of it?
		if (width >= (1 << FIXEDPT_SHIFT))
			color = fpColor;
		else
			color = ScaleColor(fpColor, width, (1 << FIXEDPT_SHIFT));
		
		// Move to the next pixel and set the color
		// NOTE: USING MODULO ASSUMES A LOOP!
		idx = (idx + 1) % LED_COUNT;
		
		// Update the NeoPixel 
		UpdateColor(afnp, idx, color, op);
	
		// Remove the width of an entire LED.
		width -= (1 << FIXEDPT_SHIFT);
	} 
}


//----------------------------------------------------------------------------------------
//	Update Color
//		Update the NeoPixel color according to one of the color operations
//
//	op:
//		OP_SET
//		OP_BLEND
//		OP_MAX
//----------------------------------------------------------------------------------------
void UpdateColor(Adafruit_NeoPixel& afnp, uint16_t index, uint32_t color, uint8_t op, uint32_t value = 0)
{
	if (op == OP_SET)
	{
		// Pass color through
	}
	else if (op == OP_BLEND)
	{
		uint32_t colorB = afnp.getPixelColor(index);
		color = CalcAlphaBlendColor(color, colorB, value);
	}
	else if (op == OP_MAX)
	{
		uint32_t colorB = afnp.getPixelColor(index);
		color = CalcMaxColor(color, colorB);
	}

	afnp.setPixelColor(index, color);
}

//========================================================================================
//	Utility Routines -- Color Calculations
//========================================================================================

//----------------------------------------------------------------------------------------
//	Scale Color
//		Scale each color channel by num/den
//----------------------------------------------------------------------------------------
uint32_t ScaleColor(uint32_t color, uint32_t num, uint32_t den)
{
	if (num != den)
	{
		uint32_t r = (color >> 16) & 0xff;
		uint32_t g = (color >>  8) & 0xff;
		uint32_t b = (color      ) & 0xff;
	
		r = (r * num) / den;
		g = (g * num) / den;
		b = (b * num) / den;
	
		color = ((r << 16) | (g << 8) | b);
	}
	
	return color;
}

//----------------------------------------------------------------------------------------
//	Calc Max Color
//		Take the maximum value from each channel
//----------------------------------------------------------------------------------------
uint32_t CalcMaxColor(uint32_t colorA, uint32_t colorB)
{
	uint32_t color;
	uint32_t ca, cb;
	uint32_t r, g, b;
	
	ca = (colorA & 0xff0000);
	cb = (colorB & 0xff0000);
	r  = (ca > cb) ? ca : cb;
	
	ca = (colorA & 0x00ff00);
	cb = (colorB & 0x00ff00);
	g  = (ca > cb) ? ca : cb;

	ca = (colorA & 0x0000ff);
	cb = (colorB & 0x0000ff);
	b  = (ca > cb) ? ca : cb;
	
	color = (r | g | b);
	
	return color;
}

//----------------------------------------------------------------------------------------
//	Calc Blend Color
//		Take the maximum value from each channel
//
//		colorA: 
//		colorB:
//		alphaA: alpha value to apply to colorA (and 1-alpha to apply to color B)
//----------------------------------------------------------------------------------------
uint32_t CalcAlphaBlendColor(uint32_t colorA, uint32_t colorB, uint32_t alphaA)
{
	uint32_t color;
	uint32_t ca, cb;
	uint32_t r, g, b;
	
	ca = ((colorA >> 16) & 0xff);
	cb = ((colorB >> 16) & 0xff);
	r  = ((alphaA * ca) + ((0xff - alphaA) * cb))/0xff;
	
	ca = ((colorA >> 8) & 0xff);
	cb = ((colorB >> 8) & 0xff);
	g  = ((alphaA * ca) + ((0xff - alphaA) * cb))/0xff;

	ca = (colorA & 0xff);
	cb = (colorB & 0xff);
	b  = ((alphaA * ca) + ((0xff - alphaA) * cb))/0xff;
	
	color = ((r << 16) | (g << 8) | b);
	
	return color;
}


//========================================================================================
//	Early routines
//========================================================================================

//----------------------------------------------------------------------------------------
//	Full Colors
//		Sets the colors of each side for a few seconds, blanks, 
//		continues with the next color sequence
//----------------------------------------------------------------------------------------
void fullColorsTest(void)
{
	int bigDelay = 3000;
	
	uint32_t* sequences[] = {seqRRGGBB, seqRGRGRG, seqRYGCBM, seqRGBRGB};
	
	for (int i = 0; i < sizeof(sequences)/sizeof(*sequences); i++)
	{
		uint32_t* colors = sequences[i];
		
		SetTriangleColors(colors);
		delay(bigDelay);
	
		AllOff();
		delay(250);
	}

	delay(250);
}

//----------------------------------------------------------------------------------------
//	Loop Sequence
//
//		colorsA: array with 6 colors
//		colorsB: array with 6 colors
//		delayTime: delay between each update
//----------------------------------------------------------------------------------------
void DualLoopSequence(uint32_t* colorsA, uint32_t* colorsB, uint32_t delayTime)
{
	uint16_t ledCount = hexring.numPixels();
	uint16_t spacing = ledCount/6;

	for (uint16_t step = 0; step < LED_COUNT; step++)
	{
		hexring.clear();
	
		for (uint8_t c = 0; c < 6; c++)
			hexring.setPixelColor((step + c * spacing) % ledCount, colorsA[c]);

		// Calculate the reverse the direction for the second set of colors
		int s2 = ledCount - 1 - step;
		
		for (uint8_t c = 0; c < 6; c++)
			hexring.setPixelColor((s2 + c * spacing) % ledCount, colorsB[c]);

		hexring.show();
		delay(delayTime);
	}

}


//----------------------------------------------------------------------------------------
//	Loop Sequence
//
//		colors: array with 6 colors
//		delayTime: delay between each update
//----------------------------------------------------------------------------------------
void loopSequence(uint32_t* colors, uint32_t delayTime)
{
	uint16_t ledCount = hexring.numPixels();
	uint16_t spacing = ledCount/6;
	
	for (uint16_t step = 0; step < LED_COUNT; step++)
	{
		hexring.clear();
	
		for (uint8_t c = 0; c < 6; c++)
			hexring.setPixelColor((step + c * spacing) % ledCount, colors[c]);

		hexring.show();
		delay(delayTime);
	}

}


//========================================================================================
//	Adafruit code
//========================================================================================

//----------------------------------------------------------------------------------------
//	chase
//----------------------------------------------------------------------------------------
static void chase(uint32_t c)
{
	for(uint16_t i = 0; i < hexring.numPixels() + 4; i++) 
	{
		hexring.setPixelColor(i  , c); // Draw new pixel
		hexring.setPixelColor(i-4, 0); // Erase pixel a few steps back
		hexring.show();
		delay(50);
	}
}

