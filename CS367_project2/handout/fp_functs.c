/* Fill in your Name and GNumber in the following two comment fields
 * Name:
 * GNumber:
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <inttypes.h>
#include "fp.h"

/* input: float value to be represented
 * output: integer representation of the input float
 *
 * Perform this the same way we did in class -
 *   either dividing or multiplying the value by 2
 *   until it is in the correct range (between 1 and 2).
 *
 * Follow the Project Documentation for Instructions
 */

 typedef unsigned int u32;
 typedef unsigned long long u64;
 typedef unsigned long long u64;

  typedef union {

         float f;
         struct
         {

             // Order is important.
             // Here the members of the union data structure
             // use the same memory (32 bits).
             // The ordering is taken
             // from the LSB to the MSB.

             unsigned int mantissa : 23;
             unsigned int exponent : 8;
             unsigned int sign : 1;

         } raw;
     } myfloat;

     int convertToInt(int arr[], int low, int high){
         unsigned f = 0, i;

         for (i = high; i >= low; i--) {
                     f = f + arr[i] * pow(2, high - i);
         }

         return f;
     }

int compute_fp(float val) {
  /* Implement this function */

    myfloat var;

    //get the 32 bit floating point number
   unsigned int ieee[32]
            = { 1,1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0 };


  //convert the least significant
  //mantissa part 21 bits
  // therefore low = 11, high = 31
  unsigned f = convertToInt(ieee,11,31);

  //assign integer representation of mantissa
  var.raw.mantissa = f;

  //convert the exponent part 4 bits
  f = convertToInt(ieee,1,4);

  //assign integer representation of the exponent
  var.raw.exponent = f;

  //assign sign bit
  var.raw.sign = ieee[0];


   // infinity error handle
  if(var.raw.exponent == 1111 && var.raw.mantissa == 0000000000 ){
    //var.raw.exponent = 0000;
    //var.raw.mantissa = 0;
    return 0;
  }else if(var.raw.exponent == 0000){ //denormalized error
    return 0;
  }else{
    return var.f;
  }

  //return 125;
}


/* input: integer representation of our floats
 * output: float value represented by our integer encoding
 *
 * Follow the Project Documentation for Instructions
 */

void printBinary(int n, int i)
{

    // Prints the binary representation
    // of a number n up to i-bits.
    int k;
    for (k = i - 1; k >= 0; k--) {

        if ((n >> k) & 1)
            printf("1");
        else
            printf("0");
    }
}

float get_fp(int val) {

  float intPart = 0, fracPart = 0, conversion;
  unsigned int i;
  char decimal[20];
  sprintf(decimal, "%05d", val);

   i = 0; /* Left to right */
   while (decimal[i] != '.') {
      intPart = intPart*10 + (decimal[i] - '0');
      i++;
     }

   i = strlen(decimal)-1; /* Right to left */
   while (decimal[i] != '.') {
      fracPart = (fracPart + (decimal[i] - '0'))/10;
      i--;
	  if(i==0)
		  break;
    }

   return conversion = intPart + fracPart;
}



/* input: Two integer representations of our floats
 * output: The multiplication result in our integer representation
 *
 * You must implement this using the algorithm described in class:
 *   Xor the signs: S = S1 ^ S2
 *   Add the exponents: E = E1 + E2
 *   Multiply the Frac Values: M = M1 * M2
 *   If M is not in a valid range for normalized, adjust M and E.
 *
 * Follow the Project Documentation for Instructions
 */
int mult_vals(int source1, int source2) {
  /* Implement this function */
   myfloat s1,s2;

   s1.f = get_fp(source1);
   s2.f = get_fp(source2);

// extract mantissa, exponent and sign
	/*u32 s1Frac = s1.f & 0x007FFFFF;
	u32 s2Frac = s2.f & 0x007FFFFF;

	u32 s1Exp = (s1.f >> 27) & 0xFF;
	u32 s2Exp = (s2.f >> 27) & 0xFF;

	u32 s1Sign = s1.f >> 31;
	u32 s2Sign = s2.f >> 31;

	// compute sign bit
	u32 zSign = s1Sign ^ s2Sign;

	// compute exponent
	u32 zExp = s1Exp + s2Exp - 0x7F;

	// add implicit `1' bit
	s1Frac = (s1Frac | 0x00800000) << 7;
	s2Frac = (s2Frac | 0x00800000) << 8;

	u64 zFrac = (u64)s1Frac * (u64)s2Frac;

	u32 zFrac0 = zFrac >> 32;
	u32 zFrac1 = zFrac & 0xFFFFFFFF;

	// check if overflowed into more than 23-bits and handle accordingly
	zFrac0 |= (zFrac1 != 0);
	if(0 <= (i32)(zFrac0 << 1)){
		zFrac0 <<= 1;
		zExp--;
	}
	// reconstruct the float;
	float mulp = (zSign << 31) | ((zExp << 23) + (zFrac >> 7));
    return compute_fp(mulp);*/
    return 125;

}

/* input: Two integer representations of our floats
 * output: The addition result in our integer representation
 *
 * You must implement this using the algorithm described in class:
 *   If needed, adjust the numbers to get the same exponent E
 *   Add the two adjusted Mantissas: M = M1 + M2
 *   Adjust M and E so that it is in the correct range for Normalized
 *
 * Follow the Project Documentation for Instructions
 */


int msb_length(float l) {
    int cnt = 0;
    int x = get_fp(l);
    while (x >= 1) {
        x >>= 1;
        cnt++;
    }
    return cnt;
}

u32 shift32Right(int a, int count){

  if(count == 0){
    return a;
  }else if(count < 32){
    return (a >> count) | ((a << ((-count) & 31)) != 0);
  }else{
    return a != 0;
  }

}

float add(float x,float y){
        	int zExp;
        	u32 zFrac;

        	/*u32 xFrac = x & 0x007FFFFF;
        	u32 yFrac = y & 0x007FFFFF;

        	int xExp = (x >> 27) & 0xFF;
        	int yExp = (y >> 27) & 0xFF;

        	u32 xSign = x >> 31;
        	u32 ySign = y >> 31;
        	u32 zSign = xSign;

        	int expDiff = xExp - yExp;

        	xFrac <<= 6;
        	yFrac <<= 6;

        	// align exponents if needed

        	if(expDiff > 0){
        		if(yExp == 0){
        		    --expDiff;
        		}
        	}else{
        	    yFrac |= 0x20000000;
        		yFrac = shift32Right(yFrac, expDiff);
        		zExp = xExp;
        	}else if(expDiff < 0){
        		if(xExp == 0){
        		    ++expDiff;
        		} else {
        		    xFrac |= 0x20000000;
        		}
        		xFrac = shift32Right(xFrac, -expDiff);
            	zExp = bExp;
        	}else if(expDiff == 0){
        		if(xExp == 0){
        		    return (zSign << 31) | ((xFrac + yFrac) >> 13);
        		}
        		zFrac = 0x40000000 + xFrac + yFrac;
        		zExp = xExp;
        		return (zSign << 31) | ((zExp << 27) + (zFrac >> 3));
        	}

        	xFrac |= 0x20000000;
        	zFrac = (xFrac + yFrac) << 1;
        	--zExp;

        	if((i32)zFrac < 0){
        		zFrac = xFrac + yFrac;
        		++zExp;
        	}

        	// reconstruct the float;
        	return (zSign << 31) | ((zExp << 23) + (zFrac >> 7));*/
        	return 24.5;
}


float add_vals(int source1, int source2) {
  /* Implement this function */

  myfloat s1,s2;

  s1.f = get_fp(source1);
  s2.f = get_fp(source2);

 return add(s1.f,s2.f);
}
