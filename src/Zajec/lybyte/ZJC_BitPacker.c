#include "ZJC_BitPacker.h"
#include "ZJC_BinTypes.h"

#include "../lyutils/ZJC_Evil.h"
#include "../lymath/ZJC_GOPS.h"

#include <stdio.h>
#include <math.h>

//      - --- - --- - --- - --- -

uchar  sizebyprob   (uint prob, uint size)      { float fpercent  = roundf( ((float) prob / size) * 100 );
                                                  ushort percent  = (ushort) fpercent; return usbitsize(percent);       }

void   zh8_delPacker(H8PACK* hpck)              { WARD_EVIL_MFREE(hpck->data); WARD_EVIL_MFREE(hpck->dict);             }

//      - --- - --- - --- - --- -

void zh8_pack(H8PACK* hpck,
              uint*   protodict,
              uchar*  data       )              {

    hpck->dict        = (uchar*) evil_malloc(hpck->dictsize, sizeof(uchar));

    for(ushort i = 0; i < hpck->dictsize; i++)
    {

        uchar  mostfrequent = 0;
        ushort hifrequency  = 0;
        ushort popi         = 0;

        for(ushort j = 0; j < hpck->dictsize; j++)
        {
            if(protodict[j])                    { uchar  value     =  protodict[j] & 0x000000FF;
                                                  ushort frequency = (protodict[j] & 0xFFFFFF00) >> 8;

                                                  if(frequency > hifrequency)
                                                { mostfrequent = value; hifrequency = frequency; popi = j; }            }
        }

        hpck->dict[i]        = mostfrequent; protodict[popi] = 0;
    }

//      - --- - --- - --- - --- -

    uchar  hilength = 3 + usbitsize(hpck->dictsize-1);

    uint   numbytes = 0;
    uint   curbit   = 0;

    for(uint i = 0; i < hpck->usize; i++)
    {

        for(uchar j = 0; j < hpck->dictsize; j++)
        {
            if(data[i] == hpck->dict[j])
            {
                data[i] = j;
                break;
            }
        }

        uchar value    = data[i];

        if     (value == 0)                     { curbit += 1;                                                          }

        else if(value == 1)                     { curbit += 3;                                                          }
        else if(value == 2)                     { curbit += 3;                                                          }

        else if(value == 3)                     { curbit += 5;                                                          }
        else if(value == 4)                     { curbit += 5;                                                          }
        else if(value == 5)                     { curbit += 5;                                                          }
        else if(value == 6)                     { curbit += 5;                                                          }

        else                                    { curbit += hilength;                                                   }

        while  (curbit  > 7)                    { numbytes++; curbit -= clampi(8, 0, curbit);                           }

    }

//      - --- - --- - --- - --- -

    printf("Final bytecount: %u/", numbytes);

    hpck->data     = (uchar*) evil_malloc(numbytes+1, sizeof(uchar));

    numbytes       = 0;
    curbit         = 0;

    for(uint i = 0; i < hpck->usize; i++)
    {
        uchar value    = 0;
        uchar length   = 1;

        if     (data[i] == 0)                   { value =  1; length =  1;                                              }

        else if(data[i] == 1)                   { value =  4; length =  3;                                              }
        else if(data[i] == 2)                   { value =  6; length =  3;                                              }

        else if(data[i] == 3)                   { value =  0; length =  5;                                              }
        else if(data[i] == 4)                   { value = 16; length =  5;                                              }
        else if(data[i] == 5)                   { value =  8; length =  5;                                              }
        else if(data[i] == 6)                   { value = 24; length =  5;                                              }

        else                                    { value = 2 + (data[i] << 3); length = hilength;                        }

        for(uchar b = 0; b < length; b++)       { hpck->data[numbytes] |= nthbit(value, b) << curbit; curbit++;
                                                  if(curbit  > 7) { numbytes++; curbit = 0; }                           }

    }

    printf("%u\n", hpck->usize);
    hpck->datasize = numbytes+1;                                                                                        }

//      - --- - --- - --- - --- -

uchar* zh8_unpack(H8PACK* hpck)                 {

    uint   numbytes = 0;
    uint   curbit   = 0;

    uchar* unpacked = (uchar*) evil_malloc(hpck->usize, sizeof(uchar));

    uchar  hilength = 3 + usbitsize(hpck->dictsize-1);
    uint   index    = 0;

    for(uint i = 0; i < hpck->usize; i++)
    {
        uchar length  = 1;
        uint  value   = 0;

        for(uchar b = 0; b < 1; b++)            { value |= nthbit(hpck->data[numbytes], curbit) << b; curbit++;
                                                  if(curbit  > 7) { numbytes++; curbit = 0; }                           }

        if          (!value                      )
        {
            for(uchar b = 1; b < 3; b++)        { value |= nthbit(hpck->data[numbytes], curbit) << b; curbit++;
                                                  if(curbit  > 7) { numbytes++; curbit = 0; }                           }

            if      (value == 2                  )
            {
                for(uchar b = 3;
                    b < hilength; b++)          { value |= nthbit(hpck->data[numbytes], curbit) << b; curbit++;
                                                  if(curbit  > 7) { numbytes++; curbit = 0; }                           }
            }

            else if ((value != 4) && (value != 6))
            {
                for(uchar b = 3; b < 5; b++)    { value |= nthbit(hpck->data[numbytes], curbit) << b; curbit++;
                                                  if(curbit  > 7) { numbytes++; curbit = 0; }                           }
            }
        }

        if     (value ==  1)                    { index = 0;                                                            }

        else if(value ==  4)                    { index = 1; length =  3;                                               }
        else if(value ==  6)                    { index = 2; length =  3;                                               }

        else if(value ==  0)                    { index = 3; length =  5;                                               }
        else if(value == 16)                    { index = 4; length =  5;                                               }
        else if(value ==  8)                    { index = 5; length =  5;                                               }
        else if(value == 24)                    { index = 6; length =  5;                                               }

        else                                    { index = value >> 3; length = hilength;                                }

        unpacked[i] = hpck->dict[index];

    }

    return unpacked;                                                                                                    }

//      - --- - --- - --- - --- -

