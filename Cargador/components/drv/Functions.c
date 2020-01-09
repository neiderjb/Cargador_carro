
#include "Functions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char *substring(char *string, int position, int length)
{
	char *pointer;
	int c;

	pointer = malloc(length + 1);

	if (pointer == NULL)
	{
		printf("Unable to allocate memory.\n");
		exit(1);
	}

	for (c = 0; c < length; c++)
	{
		*(pointer + c) = *(string + position - 1);
		string++;
	}

	*(pointer + c) = '\0';

	return pointer;
}

// Converts a floating-point/double number to a string.
void ftoa(float n, char *res, int afterpoint)
{
    // Extract integer part
    int ipart = (int)n;

    // Extract floating part
    float fpart = n - (float)ipart;

    // convert integer part to string
    int i = intToStr(ipart, res, 0);

    // check for display option after point
    if (afterpoint != 0)
    {
        res[i] = '.'; // add dot

        // Get the value of fraction part upto given no.
        // of points after dot. The third parameter
        // is needed to handle cases like 233.007
        fpart = fpart * pow(10, afterpoint);

        intToStr((int)fpart, res + i + 1, afterpoint);
    }
}

// char* itoa(int num, char* str, int base) 
// { 
//     int i = 0; 
//     bool isNegative = false; 
  
//     /* Handle 0 explicitely, otherwise empty string is printed for 0 */
//     if (num == 0) 
//     { 
//         str[i++] = '0'; 
//         str[i] = '\0'; 
//         return str; 
//     } 
  
//     // In standard itoa(), negative numbers are handled only with  
//     // base 10. Otherwise numbers are considered unsigned. 
//     if (num < 0 && base == 10) 
//     { 
//         isNegative = true; 
//         num = -num; 
//     } 
  
//     // Process individual digits 
//     while (num != 0) 
//     { 
//         int rem = num % base; 
//         str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0'; 
//         num = num/base; 
//     } 
  
//     // If number is negative, append '-' 
//     if (isNegative) 
//         str[i++] = '-'; 
  
//     str[i] = '\0'; // Append string terminator 
  
//     // Reverse the string 
//     reverse(str, i); 
  
//     return str; 
// } 

// Reverses a string 'str' of length 'len'
void reverse(char *str, int len)
{
    int i = 0, j = len - 1, temp;
    while (i < j)
    {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
}

// Converts a given integer x to string str[].
// d is the number of digits required in the output.
// If d is more than the number of digits in x,
// then 0s are added at the beginning.
int intToStr(int x, char str[], int d)
{
    int i = 0;
    while (x)
    {
        str[i++] = (x % 10) + '0';
        x = x / 10;
    }

    // If number of digits required is more, then
    // add 0s at the beginning
    while (i < d)
        str[i++] = '0';

    reverse(str, i);
    str[i] = '\0';
    return i;
}


