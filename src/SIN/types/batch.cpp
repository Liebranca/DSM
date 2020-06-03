#include "batch.h"

const int drawflags[3] = { GL_STATIC_DRAW, GL_STREAM_DRAW, GL_DYNAMIC_DRAW };

//  - --- - --- - --- - --- -


BatchBuffer::BatchBuffer(uint usageflag, bool genUbo) {
    glGenVertexArrays(1, &vao_location);
    glBindVertexArray(vao_location);

    glGenBuffers(NUM_BUFFERS - (!genUbo - 1), &buffs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, buffs[VBO]);

    glBufferData(GL_ARRAY_BUFFER, SIN::batch_vboSize, NULL, drawflags[usageflag]);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, co));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, n));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void*)offsetof(Vertex, uv));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffs[IBO]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, SIN::batch_iboSize, NULL, drawflags[usageflag]);

    /*if (genUbo) {
        glBindBuffer(GL_UNIFORM_BUFFER, buffs[UBO]);
    }*/

    glBindVertexArray(0);

}

//  - --- - --- - --- - --- -

BatchBuffer::~BatchBuffer() {
    for (uint i = 0; i < NUM_BUFFERS; i++) {
        if (buffs[i]) { glDeleteBuffers(1, &buffs[i]); }
    }

    glDeleteVertexArrays(1, &vao_location);
}
