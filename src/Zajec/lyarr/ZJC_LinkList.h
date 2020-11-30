#ifndef __ZJC_LINKLIST_H__
#define __ZJC_LINKLIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../ZJC_CommonTypes.h"

//  - --- - --- - --- - --- -

struct ZJC_LNODE {

    uint              id;
    uint              data;
    struct ZJC_LNODE* next;

};

typedef struct ZJC_LNODE LND;

//  - --- - --- - --- - --- -

LND* ZJC_build_linkList         (uint* id, uint* data, uint numnodes);
void ZJC_del_linkList           (LND* head                          );

LND* ZJC_push_linkList          (LND* head, uint id, uint data      );
LND* ZJC_pop_linkList           (LND* head, uint id                 );
LND* ZJC_find_linkList          (LND* head, uint id                 );

//  - --- - --- - --- - --- -

#ifdef __cplusplus
}
#endif

#endif // __ZJC_LINKLIST_H__
