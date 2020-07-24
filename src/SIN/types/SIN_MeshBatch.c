#include "SIN_MeshBatch.h"

#include "lyarr/ZJC_Stack.h"
#include "lyutils/ZJC_Evil.h"
#include "GL/glew.h"

#include <stdio.h>

#define SIN_MAX_BATCHES       64

M3DB*  SIN_active_meshbatch = NULL;

M3DB*  SIN_meshbatches      = NULL;
M3DB   SIN_emptybatch       = {0};
ushort SIN_ACTIVE_BATCHES   = 0;
sStack SIN_BATCH_SLOTSTACK  = {0};

uint   SIN_GVAO             = 0;

#define SIN_BATCH_VERTSIZE    32

#define SIN_BATCH_MAXOBJECTS  256
#define SIN_BATCH_MAXVERTS    65536

#define SIN_BATCH_VBOSIZE SIN_BATCH_MAXVERTS * SIN_BATCH_VERTSIZE
#define SIN_BATCH_IBOSIZE SIN_BATCH_MAXVERTS * sizeof(uint)

//  - --- - --- - --- - --- -

void  SIN_GVAO_init()                           { SIN_BATCH_SLOTSTACK = build_sStack(SIN_MAX_BATCHES);

                                                  glGenVertexArrays(1, &SIN_GVAO);
                                                  glBindVertexArray(SIN_GVAO);

                                                  glVertexAttribFormat(0, 3, GL_FLOAT, 0, 0);
                                                  glVertexAttribBinding(0, 0);
                                                  glEnableVertexAttribArray(0);

                                                  glVertexAttribFormat(1, 3, GL_FLOAT, 0, 3 * sizeof(float));
                                                  glVertexAttribBinding(1, 0);
                                                  glEnableVertexAttribArray(1);

                                                  glVertexAttribFormat(2, 2, GL_FLOAT, 0, 6 * sizeof(float));
                                                  glVertexAttribBinding(2, 0);
                                                  glEnableVertexAttribArray(2);                                         }

void  SIN_GVAO_end()                            {

    for(uint i = 0;
        i < SIN_ACTIVE_BATCHES; i ++)           { M3DB* batch = SIN_meshbatches + i;

                                                  if(batch != NULL)
                                                { SIN_delMeshBatch(&SIN_meshbatches[i]); }                              }

    del_sStack(&SIN_BATCH_SLOTSTACK);

    glBindVertexArray(0);
    glDeleteVertexArrays(1, &SIN_GVAO);                                   }

M3DB* SIN_meshbatches_get  (ushort loc)         {

    if(loc < SIN_MAX_BATCHES)
    { 
        if(SIN_meshbatches+loc)                 { return SIN_meshbatches+loc;                                           }

        printf("Location %u points to an empty meshbatch slot\n", loc);
        return NULL;
    }

    printf("No meshbatch at location %u\n", loc);
    return NULL;                                                                                                        }

//  - --- - --- - --- - --- -

M3DB* SIN_genMeshBatch()                        {

    M3DB batch;

    uint loc = sStack_pop(&SIN_BATCH_SLOTSTACK);
    WARD_EVIL_UNSIG(loc, 1);

    batch.loc     = loc;
    batch.vcursor = 0;
    batch.icursor = 0;

    glGenBuffers(SIN_BATCH_NUM_BUFFERS, &batch.BUFFS[0]);
    glBindBuffer(GL_ARRAY_BUFFER, batch.BUFFS[SIN_BATCH_VBO]);
    glBufferData(GL_ARRAY_BUFFER, SIN_BATCH_VBOSIZE, NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, batch.BUFFS[SIN_BATCH_IBO]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, SIN_BATCH_IBOSIZE, NULL, GL_STATIC_DRAW);

    SIN_meshbatches[loc] = batch;
    SIN_active_meshbatch = &batch;

    return &batch;                                                                                                      }

//  - --- - --- - --- - --- -

void SIN_delMeshBatch(M3DB* batch)              {

    for (uint i = 0;
         i < SIN_BATCH_NUM_BUFFERS; i++)        { if (batch->BUFFS[i])
                                                { glDeleteBuffers(1, &batch->BUFFS[i]); }                               }

    int memward = sStack_push(&SIN_BATCH_SLOTSTACK, batch->loc);
    WARD_EVIL_UNSIG(memward, 1);

    SIN_meshbatches[batch->loc] = SIN_emptybatch;                                                                       }

//  - --- - --- - --- - --- -

uint  SIN_meshbatch_upload(ushort  numVerts,
                           ushort  numIndices,
                           float*   vertex_data,
                           ushort* indices)     {

    M3DB* batch = SIN_active_meshbatch;
    uint  vsize = numVerts   * sizeof(uint) * 2;
    uint  isize = numIndices * sizeof(uint);

    glBindBuffer   (GL_ARRAY_BUFFER, batch->BUFFS[SIN_BATCH_VBO]);
    glBufferSubData(GL_ARRAY_BUFFER, batch->vcursor, vsize, vertex_data);

    glBindBuffer   (GL_ELEMENT_ARRAY_BUFFER, batch->BUFFS[SIN_BATCH_IBO]);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, batch->icursor, isize, indices);

    glBindBuffer   (GL_ELEMENT_ARRAY_BUFFER, 0);

    batch->vcursor += vsize;
    batch->icursor += isize;

    return batch->icursor;                                                                                              }

