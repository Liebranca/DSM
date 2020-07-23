#ifndef __SIN_MESHBATCH_H__
#define __SIN_MESHBATCH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../Zajec/ZJC_CommonTypes.h"

enum { SIN_BATCH_VBO, SIN_BATCH_IBO, SIN_BATCH_NUM_BUFFERS };

//  - --- - --- - --- - --- -

typedef struct SIN_MESH_3D_BATCH {

    uint BUFFS[SIN_BATCH_NUM_BUFFERS];
    uint loc;
    uint vcursor;
    uint icursor;

} M3DB;

void  SIN_GVAO_init();
void  SIN_GVAO_end();

M3DB* SIN_meshbatches_get(ushort loc);
M3DB* SIN_genMeshBatch();
void  SIN_delMeshBatch(M3DB* batch);

uint  SIN_meshbatch_upload(ushort numVerts, ushort numIndices, uint* vertex_data, ushort* indices);

extern M3DB* SIN_active_meshbatch;

#ifdef __cplusplus
}
#endif

#endif // __SIN_MESHBATCH_H__
