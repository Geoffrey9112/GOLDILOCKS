#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int gPrimeNumber[] =
{
/*     127, */
/*     563, */
/*     1031, */
/*     3181, */
/*     7673, */
/*     10979, */
/*     20789, */
/*     43591, */
/*     87119, */
/*     199999, */
/*     399989, */
/*     1099997, */
/*     3000017, */
/*     5000011, */
    10000019,
    0   /*  */
};


int main( int argc, char *argv[] )
{
    long sPrimeValue;
    long sBitLen;
    long i, j;
    unsigned long sTmpValue;
    long sConstValue;
    long sWantBitCount = 32;
    long sM;
    char sSuccess;
    int  sAdjustConst;

    for( j = 0; ; j++ )
    {
        if( gPrimeNumber[j] == 0 )
            break;

        if( sPrimeValue < 0 )
        {
            printf("Need Positive Value\n");
            return 0;
        }

        sPrimeValue = (unsigned long)gPrimeNumber[j];

        sTmpValue = (unsigned long)0x00000000000000001UL << (sWantBitCount * 2 - 1);
        sTmpValue /= (unsigned long)sPrimeValue;

        sBitLen = 0;
        while( !(sTmpValue & (unsigned long)0x8000000000000000UL) )
        {
            sBitLen++;
            sTmpValue <<= 1;
        }

        sTmpValue >>= (sWantBitCount - 1);
        sTmpValue += 1;
        sTmpValue >>= 1;
        sConstValue = (long) (sTmpValue & (unsigned long)0x00000000FFFFFFFFUL);

        sSuccess = 1;
        
        for( i = 0; i < 100000000; i++ )
        {
            sTmpValue = i - (((((((unsigned long)i * (unsigned long)sConstValue) >> (sWantBitCount - 1)))) >> sBitLen) * sPrimeValue);
            
            if( sTmpValue != (((unsigned long)i) % ((unsigned long)sPrimeValue)) )
            {
                sSuccess = 0;
                break;
            }
        }

        if( sSuccess == 0 )
        {
            sSuccess = 1;
        
            for( i = 0; i < 0xffffffffU; i++ )
            {
                sTmpValue = i - (((((((unsigned long)i * (unsigned long)sConstValue) >> (sWantBitCount - 1)) + 1)) >> sBitLen) * sPrimeValue);
            
                if( sTmpValue != (((unsigned long)i) % ((unsigned long)sPrimeValue)) )
                {
                    sSuccess = 0;
                    break;
                }
            }

            if( sSuccess == 1 )
            {
                sAdjustConst = 1;
            }
        }
        else
        {
            sAdjustConst = 0;
        }
        
        if( sSuccess == 1 )
        {
            printf("VALUE: %ld [OK]\n", sPrimeValue);
            printf("  Const  = %ld ( 0x%08X )\n", (unsigned long)sConstValue, (unsigned int)sConstValue);
            printf("  BitLen = %ld\n", sBitLen - 1);
            printf("  AdjCon = %d\n", sAdjustConst);
        }
        else
        {
            printf("VALUE: %ld [FAILED]\n", sPrimeValue);
            printf("  Value  = %ld\n", i);
            printf("  Result = %ld\n", sTmpValue);
            printf("  Answer = %ld\n", ((unsigned long)i % (unsigned long)sPrimeValue) );
        }
        printf("\n");
    }

    return 0;
}

