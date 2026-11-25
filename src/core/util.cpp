#include "util.h"
#include<iostream>
using namespace std;
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
    bool stringEquals(const char* a, const char* b)
    {
    int i =0;
    while(a[i]!='\0' && b[i]!= '\0')
    {
        if (a[i]!=b[i])
        return false;
        i++;
    }
    if(a[i]=='\0' && b[i]=='\0')
    {
        return true;
    }
    return false;
    }
    void printHeader(const char* title)
    {
        for (int i=0 ; i<40 ; i++)
        {
            cout << "=" ;
        }
        cout << endl;
        for(int i =0 ; i <15 ; i++)
        {
            cout << ' ';
        }
        cout << title<< endl;
        for (int i=0 ; i<40 ; i++)
        {
            cout << "=" ;
        }
        cout << endl;
    }
    void clearScreen()
    {
cout << "\033[2J\033[H";
// \033 Escape character
// [H Move cursor to the top-left corner
// [2j clear  the terminal clearScreen
    }




