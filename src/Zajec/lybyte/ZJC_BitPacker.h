#ifndef __ZAJEC_ENTROPY_H__
#define __ZAJEC_ENTROPY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../ZJC_CommonTypes.h"

//      - --- - --- - --- - --- -

typedef struct ZAJEC_BIT_PACKER {

    ushort dictsize;

    uint   datasize;
    uint   usize;

    uchar* dict;
    uchar* data;

} H8PACK;

//      - --- - --- - --- - --- -

void zh8_buildProto ();
void zh8_cleanProto ();
void zh8_delProto   ();

void zh8_insertProto(uint value);
H8PACK* zh8_dictFromProto();

//      - --- - --- - --- - --- -

void   zh8_pack     (H8PACK* hpck, uchar*  data);
uchar* zh8_unpack   (H8PACK* hpck);

void   zh8_delPacker(H8PACK* hpck);

//      - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZAJEC_ENTROPY_H__
