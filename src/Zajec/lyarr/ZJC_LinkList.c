#include "ZJC_LinkList.h"
#include "../lyutils/ZJC_Evil.h"

#include <stdio.h>

//  - --- - --- - --- - --- -

static LND* curlnd   = NULL;
static LND* prevlnd  = NULL;

//  - --- - --- - --- - --- -

LND* ZJC_build_linkList(uint* id,
                        uint* data,
                        uint  numnodes)         {

    LND* head  = NULL;

    WARD_EVIL_MALLOC (head, LND, sizeof(LND), 1)
    MEAN_NZID_WARNING("ZJC_LinkList", id[0]    );

    head->id   = id  [0];
    head->data = data[0];

    curlnd     = head;

    for(uint i = 1; i < numnodes; i++)
    {
        curlnd->next = NULL;

        WARD_EVIL_MALLOC (curlnd->next, LND, sizeof(LND), 1)
        MEAN_NZID_WARNING("ZJC_LinkList", id[i]            );

        curlnd       = curlnd->next;
        curlnd->id   = id  [i];
        curlnd->data = data[i];
    }

    return head;                                                                                                        }

void ZJC_del_linkList(LND* head)                { curlnd = head;

    while(curlnd != NULL)                       { LND* tmp = curlnd->next; WARD_EVIL_MFREE(curlnd); curlnd = tmp;       }
                                                                                                                        }

//  - --- - --- - --- - --- -

LND* ZJC_push_linkList(LND* head,
                       uint id,
                       uint data  )             { curlnd = head;

    while(curlnd->next != NULL)                 { prevlnd = curlnd; curlnd = curlnd->next;                              }

    WARD_EVIL_MALLOC (curlnd->next, LND, sizeof(LND), 1)
    MEAN_NZID_WARNING("ZJC_LinkList", id               );

    curlnd       = curlnd->next;
    curlnd->id   = id;
    curlnd->data = data;

    return curlnd;                                                                                                      }

LND* ZJC_find_linkList(LND* head, uint id)      { curlnd = head;

    LND* found = NULL;
    prevlnd    = NULL;

    while(curlnd != NULL      )                 {
    if   (id     == curlnd->id)                 { found   = curlnd; break;                                              }
                                                  prevlnd = curlnd; curlnd = curlnd->next;                              }

    if(!found)                                  { printf("Couldn't find node with ID %u\n", id);                        }
    return found;                                                                                                       }

LND* ZJC_pop_linkList(LND* head,
                      uint id   )               { if(id) { ZJC_find_linkList(head, id); }

    if(curlnd)
    {
        LND* tmp      = curlnd->next;
        WARD_EVIL_MFREE(curlnd);

        if(prevlnd)                             { prevlnd->next = tmp; return head;                                     }

        return tmp;
    }

    return head;                                                                                                        }

//  - --- - --- - --- - --- -
