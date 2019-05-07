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
#define SIDE_COUNT 6
#define LEDS_PER_SIDE (LED_COUNT/SIDE_COUNT)

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

#define ALL_LEDS	(LED_COUNT << FIXEDPT_SHIFT)

//----------------------------------------------------------------------------------------
//	Color Operations
//----------------------------------------------------------------------------------------
#define OP_SET   0
#define OP_BLEND 1
#define OP_MAX   2

void UpdateColor(Adafruit_NeoPixel& afnp, uint16_t index, uint32_t color, uint8_t op, uint32_t value = 0);

//----------------------------------------------------------------------------------------
//	Colors
//		Convenient constants representing colors
//		Note that we are using a maximum value of 0x10 (16) in each channel. Later
//		we have the opportunity to multiple (scale) the colors. 
//----------------------------------------------------------------------------------------
#define RED		0x100000
#define GREEN	0x001000
#define BLUE	0x000010
#define CYAN	0x001010
#define MAGENTA	0x100010
#define YELLOW  0x101000

//----------------------------------------------------------------------------------------
//	Color Lists
//----------------------------------------------------------------------------------------
uint32_t seqoooooo[] = { 0,        0,        0,        0,        0,        0        }; // All off
uint32_t seqRRGGBB[] = { RED,      RED,      GREEN,    GREEN,    BLUE,     BLUE     }; // Pairs of red, green, blue
uint32_t seqoRoGoB[] = { 0,        RED,      0,        GREEN,    0,        BLUE     }; // Spaced red, green, blue
uint32_t seqRGRGRG[] = { RED,      GREEN,    RED,      GREEN,    RED,      GREEN    }; // Alternating red, green
uint32_t seqCMYCMY[] = { CYAN,     MAGENTA,  YELLOW,   CYAN,     MAGENTA,  YELLOW   }; // Triplets of cyan, magenta, yellow
uint32_t seqCoMoYo[] = { CYAN,     0,        MAGENTA,  0,        YELLOW,   0        }; // Spaced of cyan, magenta, yellow
uint32_t seqRYGCBM[] = { RED,      YELLOW,   GREEN,    CYAN,     BLUE,     MAGENTA  }; // All six colors
uint32_t seqRGBRGB[] = { RED,      GREEN,    BLUE,     RED,      GREEN,    BLUE,    };
uint32_t seqRoGoBo[] = { RED,      0,        GREEN,    0,        BLUE,     0        };

uint32_t seqRooRoo[] = { RED,      0,        0,        RED,      0,        0        };
uint32_t seqRoRoRo[] = { RED,      0,        RED,      0,        RED,      0        };
uint32_t seqRooooo[] = { RED,      0,        0,        0,        0,        0        };
uint32_t seqGooooo[] = { GREEN,    0,        0,        0,        0,        0        };
uint32_t seqYooooo[] = { YELLOW,   0,        0,        0,        0,        0        };
uint32_t seqMooooo[] = { MAGENTA,  0,        0,        0,        0,        0        };
uint32_t seqCooooo[] = { CYAN,     0,        0,        0,        0,        0        };
uint32_t seqoGoooo[] = { 0,        GREEN,    0,        0,        0,        0        };
uint32_t seqoGoGoG[] = { 0,        GREEN,    0,        GREEN,    0,        GREEN    };
uint32_t seqoGooGo[] = { 0,        GREEN,    0,        0,        GREEN,    0        };
uint32_t seqooBooB[] = { 0,        0,        BLUE,     0,        0,        BLUE     };
uint32_t seqoBoBoB[] = { 0,        BLUE,     0,        BLUE,     0,        BLUE     };
uint32_t seqBooooo[] = { BLUE,     0,        0,        0,        0,        0        };
uint32_t seqooooBo[] = { 0,        0,        0,        0,        BLUE,     0        };
uint32_t seqGoooBo[] = { GREEN,    0,        0,        0,        BLUE,     0        };

//----------------------------------------------------------------------------------------
//	Color sequence typedef
//
//		A structure to describe a color sequence, which includes a color list,
//		a "width", in pixels or LEDs, of each color (as a fixed point value), and a
//		brightness multiplier. Also included are misc values to be used as needed by
//		the animation functions.
//----------------------------------------------------------------------------------------
typedef struct {
	uint32_t* colorList;
	uint8_t   colorCount;
	uint8_t   colorWidth;
	uint8_t   colorBrightness;
	
	int8_t    miscA;
	uint16_t   miscB;
} ColorSequence_t;

#define CNT(__a__) (sizeof(__a__)/sizeof(*(__a__)))

#define SEQ_LEN(__seq__) __seq__, CNT(__seq__)
#define _list_and_len_(__array__) __array__, CNT(__array__)

//----------------------------------------------------------------------------------------
//	Color sequences
//
//----------------------------------------------------------------------------------------
ColorSequence_t csRGBPairs[] = {
	{ seqRRGGBB, 6, FOUR_LEDS, 4,  1, 0}  // RRGGBB >
};

ColorSequence_t csAllColors[] = {
	{ seqRYGCBM, 6, HALF_LEDS, 4,  1, 0}  // RYGCBM >
};

ColorSequence_t csSpinningRRGB[] = {
	{ seqRooRoo, 6, TWO_LEDS, 6,  1, 0}, // RooRoo >
	{ seqoGoooo, 6, TWO_LEDS, 6,  2, 0}, // ooGooo >>
	{ seqooooBo, 6, TWO_LEDS, 6,  3, 0}, // ooooBo >>>
	};

ColorSequence_t csSpinningRandGB[] = {
	{ seqRooooo, 6, FOUR_LEDS, 3,  1, 0}, // Rooooo >
	{ seqGoooBo, 6, FOUR_LEDS, 3,  2, 0}  // GoooBo >>
	};


ColorSequence_t csSpinningRG[] = {
	{ seqRooooo, 6, FOUR_LEDS, 3,  1, 0}, // Rooooo >
	{ seqGooooo, 6, FOUR_LEDS, 3,  2, 0}  // Gooooo >>
	};

ColorSequence_t csSpinningRGB[] = {
	{ seqRooooo, 6, SIDE_LEDS, 6,  1, 0}, // Rooooo >
	{ seqGooooo, 6, SIDE_LEDS, 6,  2, 0}, // Gooooo >>
	{ seqBooooo, 6, SIDE_LEDS, 6,  3, 0}  // Booooo >>>
	};

ColorSequence_t csGreenSpinningRed[] = {
	{ seqoGoGoG, 6, SIDE_LEDS, 6,  0, 0}, // oGoGoG '
	{ seqRooooo, 1, SIDE_LEDS, 6,  1, 0}  // Rooooo >
	};

ColorSequence_t cs06[] = {
	{ seqoRoGoB, 6, SIDE_LEDS, 4,  0, 0}, // oRoGoB '
	{ seqCoMoYo, 6, TWO_LEDS,  6,  1, 0}  // CoMoYo >
	};


ColorSequence_t cs07[] = {
	{ seqCoMoYo, 6, SIDE_LEDS, 3,  0, 0}, // CoMoYo ' 
	{ seqCoMoYo, 6, TWO_LEDS,  6,  1, 0}  // CoMoYo >
	};

ColorSequence_t cs08[] = {
	{ seqoGoGoG, 6, SIDE_LEDS, 6,  0, 0}, // oGoGoG '
	{ seqRoRoRo, 6, SIDE_LEDS, 6,  1, 0}  // RoRoRo >
	};

ColorSequence_t cs09[] = {
	{ seqoGoGoG, 6, HALF_LEDS, 6, -1, 0}, // oGoGoG <
	{ seqRoRoRo, 6, HALF_LEDS, 6,  1, 0}  // RoRoRo >
	};

ColorSequence_t cs10[] = {
	{ seqYooooo, 6, TWO_LEDS,  6, -1, 0}  // Yooooo < 
	};

ColorSequence_t csBluesReds[] = {
	{ seqoBoBoB, 6, HALF_LEDS, 6, -1, 3*SIDE_LEDS}, // oBoBoB <
	{ seqRoRoRo, 6, HALF_LEDS, 6,  1, 3*SIDE_LEDS}  // RoRoRo >
	};

ColorSequence_t cs12[] = {
	{ seqYooooo, 6, HALF_LEDS, 6,  1, 0}, // Yooooo >
	{ seqMooooo, 6, HALF_LEDS, 6,  2, 0}, // Mooooo >>
	{ seqCooooo, 6, HALF_LEDS, 6,  3, 0}  // Cooooo >>>
	};

ColorSequence_t csYellowSwing[] = {
	{ seqYooooo, 1, FOUR_LEDS,  6,  1, (2*ALL_LEDS)}  // Yooooo  
	};

ColorSequence_t csRedBlueSwing[] = {
	{ seqRooooo, 1, SIDE_LEDS,  6,  1, ALL_LEDS},
	{ seqBooooo, 1, SIDE_LEDS,  6, -1, ALL_LEDS}
	};

ColorSequence_t csRGBandCMYSwing[] = {
	{ seqoRoGoB, 6, TWO_LEDS,  6,  1, 3*SIDE_LEDS},
	{ seqCoMoYo, 6, TWO_LEDS,  6,  2, 3*SIDE_LEDS}
	};

ColorSequence_t csLotsOfFade[] = {
	{ seqRRGGBB, 6, TWO_LEDS,  6,  1, 0},
	{ seqCMYCMY, 6, TWO_LEDS,  6,  1, 0},
	{ seqRYGCBM, 6, TWO_LEDS,  6,  1, 0},
	{ seqRYGCBM, 6, TWO_LEDS,  6,  1, 0}
	};

//----------------------------------------------------------------------------------------
//	Color Animations typedefs
//----------------------------------------------------------------------------------------

typedef uint16_t (*ColorAnimationFunction_t)(struct ColorAnimation_tag* animation, uint16_t* step);

typedef struct ColorAnimation_tag
{
	ColorAnimationFunction_t	animationFunction;
	ColorSequence_t*			colorSequence;
	uint8_t						colorSequenceCount;
	uint8_t						repeatCount;
	uint16_t					delayTime;
	uint16_t					miscA;
} ColorAnimation_t;

//----------------------------------------------------------------------------------------
//	Animation Functions
//----------------------------------------------------------------------------------------
uint16_t FadeTrianglesAnimation	(ColorAnimation_t* animation, uint16_t* pStep);
uint16_t RotateColorsAnimation	(ColorAnimation_t* animation, uint16_t* pStep);
uint16_t SwingColorsAnimation	(ColorAnimation_t* animation, uint16_t* pStep);

//----------------------------------------------------------------------------------------
//	Color Animations 
//----------------------------------------------------------------------------------------
ColorAnimation_t anFadeTriangle_BlueRed 	= { FadeTrianglesAnimation, _list_and_len_(csBluesReds), 1, 10, 4000 };
ColorAnimation_t anFadeTriangle_RGBandCMY 	= { FadeTrianglesAnimation, _list_and_len_(csRGBandCMYSwing), 1, 10, 4000 };
ColorAnimation_t anFadeTriangle_Lots	 	= { FadeTrianglesAnimation, _list_and_len_(csLotsOfFade), 1, 10, 4000 };
ColorAnimation_t anRotate_RGBPairs 			= { RotateColorsAnimation, _list_and_len_(csRGBPairs), 1, 10, 0 };
ColorAnimation_t anRotate_AllColors 		= { RotateColorsAnimation, _list_and_len_(csAllColors), 1, 100, 0 };
ColorAnimation_t anRotate_RRGB 				= { RotateColorsAnimation, _list_and_len_(csSpinningRRGB), 1, 100, 0 };
ColorAnimation_t anSwing_Yellow				= { SwingColorsAnimation, _list_and_len_(csYellowSwing), 1, 400, 0 };
ColorAnimation_t anSwing_RedBlue			= { SwingColorsAnimation, _list_and_len_(csRedBlueSwing), 1, 200, 0 };
ColorAnimation_t anSwing_BluesReds			= { SwingColorsAnimation, _list_and_len_(csBluesReds), 1, 300, 0 };
ColorAnimation_t anSwing_RGBandCMY			= { SwingColorsAnimation, _list_and_len_(csRGBandCMYSwing), 1, 500, 0 };

//----------------------------------------------------------------------------------------
//	Animation List
//----------------------------------------------------------------------------------------
ColorAnimation_t* AnimationList[] = 
	{
		&anFadeTriangle_BlueRed,
		&anSwing_Yellow,
		&anRotate_RGBPairs,
		&anFadeTriangle_Lots,
		&anSwing_RGBandCMY,
		&anRotate_AllColors,
		&anSwing_RedBlue,
		&anRotate_RRGB,
		&anSwing_BluesReds,
		&anFadeTriangle_RGBandCMY
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
	//TestAnimation();
	
	//RunAnimation(&anFadeTriangle_Lots);
	
	RunAnimationList(_list_and_len_(AnimationList), 30 * 1000);
}


//----------------------------------------------------------------------------------------
//	Run Animation List
//		
//----------------------------------------------------------------------------------------
void RunAnimationList(ColorAnimation_t** animations, uint16_t animationCount, uint32_t runTimeEach)
{
	uint16_t animationIdx = 0;

	do {
		// Run the current animation for at least 'runTimeEach' milliseconds
		RunAnimationFor(animations[animationIdx], runTimeEach);
		
		// Move to next animation or back to the beginning
		animationIdx = (animationIdx + 1) % animationCount;
		
	} while (true); // Repeat forever 
}

//----------------------------------------------------------------------------------------
//	Run Animation
//		Run the animation
//----------------------------------------------------------------------------------------
void RunAnimation(ColorAnimation_t* animation)
{
	uint16_t delayTime;
	uint16_t step = 0;
	
	do {
		delayTime = animation->animationFunction(animation, &step);
		delay(delayTime);
	} while (step != 0);
}

//----------------------------------------------------------------------------------------
//	Run Animation For
//		Run the animation for at least the given number of milliseconds
//----------------------------------------------------------------------------------------
void RunAnimationFor(ColorAnimation_t* animation, uint32_t runForTime)
{
	uint32_t runTime = 0;
	uint16_t delayTime;
	uint16_t step = 0;
	
	do {
		delayTime = animation->animationFunction(animation, &step);
		delay(delayTime);
		// Accumulate the run time. Some functions return a delay of zero ms.
		// For these functions add 5 ms to the run time (arbitrary, but close enough)
		runTime += (delayTime > 5) ? delayTime : 5;
		
	} while (step != 0 || runTime < runForTime);
}

//----------------------------------------------------------------------------------------
//	Individual tests
//----------------------------------------------------------------------------------------
void TestAnimation(void)
{
	
	//RotateColors(seqRYGCBM, 6, TWO_LEDS, 300);

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
	
	//MultiRotateColors(csSpinningRGB, CNT(csSpinningRGB), 100);
	//MultiRotateColors(cs09, 2, 250);
	//MultiRotateColors(cs11, 2, 250);
	//MultiRotateColors(cs12, 3, 250);
	//MultiRotateColors(cs07, 2, 100);
	
	//uint32_t* sequenceList1[] = {seqoooooo, seqRYGCBM, seqCMYCMY, seqRRGGBB, seqRGBRGB};
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
//	Color Animation Functionc
//========================================================================================

//----------------------------------------------------------------------------------------
//	Fade Triangles
//		Light each triangle (side of the hexagon) with a color from the color list,
//		and transition from one color list to the next
//----------------------------------------------------------------------------------------

uint16_t FadeTrianglesAnimation(ColorAnimation_t* animation, uint16_t* pStep)
{
	#define MAX_ALPHA 256
	uint16_t maxStep = MAX_ALPHA * animation->colorSequenceCount;

	uint16_t step = *pStep;

	uint8_t seqIdx = step / MAX_ALPHA;
	uint8_t alpha  = step % MAX_ALPHA;
	
	uint16_t delayTime;
		
	uint8_t next = (seqIdx + 1) % animation->colorSequenceCount;
		
	hexring.clear();

	for (uint8_t side = 0; side < SIDE_COUNT; side++)
	{
		ColorSequence_t* seqA = animation->colorSequence + seqIdx;
		ColorSequence_t* seqB = animation->colorSequence + next;
		uint32_t colorA = seqA->colorList[side] * seqA->colorBrightness;
		uint32_t colorB = seqB->colorList[side] * seqB->colorBrightness;
		uint32_t color = CalcAlphaBlendColor(colorB, colorA, alpha);
		UpdateSideColor(side, color, OP_SET);
	}

	hexring.show();

	delayTime = animation->delayTime;
	if (alpha == MAX_ALPHA - 1)
		delayTime += animation->miscA;
		
		
	*pStep = (step + 1) % maxStep;
	
	return delayTime;
}

//----------------------------------------------------------------------------------------
//	Rotate Colors
//		Rotate at various speeds and overlay multiple lists of colors
//----------------------------------------------------------------------------------------
uint16_t RotateColorsAnimation(ColorAnimation_t* animation, uint16_t* pStep)
{
	uint16_t maxStep = (LED_COUNT << FIXEDPT_SHIFT);
	uint16_t step = *pStep;
	
	hexring.clear();
	
	for (uint8_t seqIdx = 0; seqIdx < animation->colorSequenceCount; seqIdx++)
	{
		ColorSequence_t* seq = animation->colorSequence + seqIdx;
		uint8_t spacing = (LED_COUNT << FIXEDPT_SHIFT)/seq->colorCount;
		
		uint8_t speed = abs(seq->miscA);			
		uint16_t base = step * speed;
		
		if (seq->miscA < 0)
			base =  (LED_COUNT << FIXEDPT_SHIFT) * speed - base - 1;
			
		for (uint8_t colorIdx = 0; colorIdx < seq->colorCount; colorIdx++)
		{
			uint16_t offset = (base + colorIdx * spacing) % (LED_COUNT << FIXEDPT_SHIFT);
			uint32_t color = seq->colorList[colorIdx] * seq->colorBrightness;
			SetFractPixel(hexring, offset, seq->colorWidth, color, OP_MAX);
		}
	}
	
	hexring.show();
	
	*pStep = (step + 1) % maxStep;
	
	return (animation->delayTime >> FIXEDPT_SHIFT);
}

//----------------------------------------------------------------------------------------
//	BackAndForth
//		Use a sine function to move the LEDs back and forth
//----------------------------------------------------------------------------------------
uint16_t SwingColorsAnimation(ColorAnimation_t* animation, uint16_t* pStep)
{
	// maxSteps is the number of steps to complete the full back and forth. The value is
	// somewhat arbitrary. 
	uint16_t maxSteps = (LEDS_PER_SIDE << FIXEDPT_SHIFT);
	uint16_t step = *pStep;
	
	
	hexring.clear();
		
	for (uint8_t seqIdx = 0; seqIdx < animation->colorSequenceCount; seqIdx++)
	{
		ColorSequence_t* seq = animation->colorSequence + seqIdx;

		uint8_t spacing = (LED_COUNT << FIXEDPT_SHIFT)/seq->colorCount;
	
		// maxBase is how far along the color will be at the maximum "displacement"
		uint16_t colorSweep = seq->miscB;
		uint16_t maxBase = colorSweep - seq->colorWidth;

		uint8_t speed = abs(seq->miscA);	

		uint16_t base;
		
		if (seq->miscA > 0)
			base = (maxBase - (float)maxBase * cos((speed * step * 3.141592 * 2) / (maxSteps - 1))) / 2;
		else if (seq->miscA < 0)
			base = (maxBase + (float)maxBase * cos((speed * step * 3.141592 * 2) / (maxSteps - 1))) / 2;
		else
			base = 0;
	
		for (uint8_t colorIdx = 0; colorIdx < seq->colorCount; colorIdx++)
		{
			uint16_t offset = (base + colorIdx * spacing) % (LED_COUNT << FIXEDPT_SHIFT);
			uint32_t color = seq->colorList[colorIdx] * seq->colorBrightness;
			SetFractPixel(hexring, offset, seq->colorWidth, color, OP_MAX);
		}
	}
	hexring.show();
	
	*pStep = (step + 1) % maxSteps;
	
	return (animation->delayTime >> FIXEDPT_SHIFT);
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
void MultiRotateColors(ColorSequence_t* sequenceList, uint8_t sequenceCount, uint16_t delayTime)
{
	for (uint16_t step = 0; step < (LED_COUNT << FIXEDPT_SHIFT); step++)
	{
		hexring.clear();
		
		for (uint8_t seqIdx = 0; seqIdx < sequenceCount; seqIdx++)
		{
			ColorSequence_t* seq = sequenceList + seqIdx;
			uint8_t spacing = (LED_COUNT << FIXEDPT_SHIFT)/seq->colorCount;
			
			uint8_t speed = abs(seq->miscA);			
			uint16_t base = step * speed;
			
			if (seq->miscA < 0)
				base =  (LED_COUNT << FIXEDPT_SHIFT) * speed - base - 1;
				
			for (uint8_t colorIdx = 0; colorIdx < seq->colorCount; colorIdx++)
			{
				uint16_t offset = (base + colorIdx * spacing) % (LED_COUNT << FIXEDPT_SHIFT);
				uint32_t color = seq->colorList[colorIdx] * seq->colorBrightness;
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

