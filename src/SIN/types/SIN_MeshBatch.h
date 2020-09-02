#ifndef __SIN_MESHBATCH_H__
#define __SIN_MESHBATCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/ZJC_CommonTypes.h"

enum { SIN_BATCH_VBO, SIN_BATCH_IBO, SIN_BATCH_NUM_BUFFERS };

//  - --- - --- - --- - --- -

typedef struct SIN_MESH_3D_BATCH {

    uint VAO;
    uint BUFFS[SIN_BATCH_NUM_BUFFERS];

    uint loc;
    uint vcursor;
    uint icursor;

} M3DB;

void  SIN_Batcher_init();
int   SIN_Batcher_end();

M3DB* SIN_meshbatches_get(ushort loc);
M3DB* SIN_genMeshBatch();
void  SIN_bindMeshBatch(ushort loc);
void  SIN_delMeshBatch(M3DB* batch);

void  chkbatch        (ushort loc);

void  SIN_meshbatch_upload(ushort numVerts, ushort numIndices, float* vertex_data, ushort* indices);

extern M3DB* SIN_active_meshbatch;

#ifdef __cplusplus
}
#endif

#endif // __SIN_MESHBATCH_H__
