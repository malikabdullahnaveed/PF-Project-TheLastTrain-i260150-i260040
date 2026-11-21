#include "util.h"
    int clampInt(int val, int minVal, int maxVal)
    {
        if (val>=minVal && val<=maxVal)
        {
            return val;
        }
        
        if (val>maxVal)
        {
            return maxVal;
        }
        if (val<minVal)
        {
            return minVal;
        }
        return minVal;
    }

    void trimWhitespace(char* str)
    {
        int i = 0 ;
       while(str[i]== ' ')
       {
         i++;
       }
       int j = 0;
       while(str[i]!='\0')
       {
        str[j]= str[i];
        i++;
        j++;
       }
       str[j]= '\0';
        int k = j-1 ;
        while (k>=0&& str[k]==' ')
        {
            k--;
        }
        str[k+1]= '\0';
        
    }

// Primitive helper utilities, clamping, and screen control
