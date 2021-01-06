#include "ZJC_BitPacker.h"
#include "ZJC_BinTypes.h"

#include "../lyutils/ZJC_Evil.h"
#include "../lymath/ZJC_GOPS.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

//      - --- - --- - --- - --- -

static uint* protodict    = NULL;
static uint  protosize    = 0;

void zh8_buildProto ()                          { WARD_EVIL_MALLOC(protodict, uint, sizeof(uint), 256); protosize = 0;  }
void zh8_cleanProto ()                          { memset          (protodict, 0, sizeof(*protodict)  ); protosize = 0;  }
void zh8_delProto   ()                          { WARD_EVIL_MFREE (protodict                         );                 }

void zh8_insertProto(uint value)                { uint freq        = protodict[value]; if(!freq) { protosize++;         }
                                                  protodict[value]++;                                                   }

H8PACK* zh8_dictFromProto()                     {

    H8PACK* hpck   = NULL; WARD_EVIL_MALLOC     (hpck,       H8PACK, sizeof(H8PACK), 1                                  );
    WARD_EVIL_MALLOC                            (hpck->dict, uchar,  sizeof(uchar ), protosize                          );

    hpck->dictsize = protosize; uint k = 0;

    for(uint i = 0; i < 256; i++)               {

        uint value  = i;
        uint hifreq = protodict[i];

        for(uint j = 0; j < 256; j++) { uint freq = protodict[j]; if(freq > hifreq) { hifreq = freq; value = j; }       }
        if(hifreq)                    { hpck->dict[k] = value; k++; protodict[value] = 0;                       }       }

    return hpck;

    }

//      - --- - --- - --- - --- -

uchar  sizebyprob   (uint prob, uint size)      { float fpercent  = roundf( ((float) prob / size) * 100 );
                                                  ushort percent  = (ushort) fpercent; return usbitsize(percent);       }

void   zh8_delPacker(H8PACK* hpck)              { WARD_EVIL_MFREE(hpck->data); WARD_EVIL_MFREE(hpck->dict);
                                                  WARD_EVIL_MFREE(hpck);                                                }

//      - --- - --- - --- - --- -

void zh8_pack(H8PACK* hpck,
              uchar*  data)                     {

    uchar  hilength = 3 + usbitsize(hpck->dictsize-1);

    uint   numbytes = 0;
    uint   curbit   = 0; uint fuck = 0;

    for(uint i = 0; i < hpck->usize; i++)
    {

        for(uchar j = 0; j < hpck->dictsize; j++)
        {   if(data[i] == hpck->dict[j])        { data[i] = j; break; }                                                 }

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

    WARD_EVIL_MALLOC                            (hpck->data, uchar, sizeof(uchar), numbytes                             );

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
    hpck->datasize = numbytes;                                                                                          }

//      - --- - --- - --- - --- -

uchar* zh8_unpack(H8PACK* hpck)                 {

    uint   numbytes = 0;
    uint   curbit   = 0;

    uchar* unpacked = NULL; WARD_EVIL_MALLOC    (unpacked, uchar, sizeof(uchar), hpck->usize                            );

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

