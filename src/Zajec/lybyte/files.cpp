#include <iostream>
#include <cstdint>

#include "files.h"

namespace files {

//  - --- - --- - --- - --- -

    void openbin(const char filename[]) {
        iFile file; file.open(filename, std::ios::binary);
        if (!file) { std::cerr << "Unable to open file " << filename << std::endl; exit(1); }
        curfile = &file;
    }

//  - --- - --- - --- - --- -

    void writecrk(const char filename[]){
        openbin(filename);
        std::vector<float> fv, gv; std::vector<std::uint16_t> iv;
        std::uint16_t numverts, numinds;
    
        curfile->read((char *)&numverts, sizeof(std::uint16_t));
        fv.resize(numverts);

        curfile->read((char *)&numinds, sizeof(std::uint16_t));
        iv.resize(numinds); gv.resize(24);

        curfile->read((char*)gv.data(), 24 * sizeof(float));
        curfile->read((char*)fv.data(), numverts * (sizeof(float) * 8));
        curfile->read((char*)iv.data(), numinds * (sizeof(std::uint16_t) * 3));

        curfile->close();
        
        /*CrkFile crk;        
        crk.numVerts = numverts;
        crk.numIndices = numinds;
        crk.verts.reserve(numverts);
        crk.inds.reserve(numinds);

        for(uint i = 0; i < fv.size(); i++) {
            VertexPacked v = {fv[i], fv[i+1]};
            crk.verts.emplace_back();
        }*/
    }

}