#include <iostream>
#include <cstdint>

#include "files.h"
#include "../lymath/gops.h"

namespace zjc {

//  - --- - --- - --- - --- -

    void openbin(const char filename[]) { ; }

//  - --- - --- - --- - --- -

    void writecrk(const char filename[]){
        //openbin(filename);
        //std::uint16_t numverts, numinds;
        
        FILE* pfile;
        int * b1;
        
        pfile = fopen(filename, "r");
        if(!pfile) { std::cerr << "Cannot open file " << filename << std::endl; std::exit(1); }

        b1 = (int*) malloc(2 * sizeof(int));
        if(b1 == NULL) { std::cerr << "Insufficient memory" << std::endl; std::exit(2); }

        fread(b1, 2 * sizeof(int), 2, pfile);
                

        fclose(pfile);
        free(b1);

        /*curfile->read(retchar(&numverts), sizeof(std::uint16_t));
        fv.resize(numverts*8);
        
        curfile->read(retchar(&numinds), sizeof(std::uint16_t));
        iv.resize(numinds*3); gv.resize(24);
        
        curfile->read(retchar(gv.data()), 24 * sizeof(float));
        curfile->read(retchar(fv.data()), numverts * (sizeof(float) * 8));
        curfile->read(retchar(iv.data()), numinds * (sizeof(std::uint16_t) * 3));
        
        curfile->close();
        
        CrkFile crk; uint i;
        crk.numVerts = numverts;
        crk.numInds = numinds;
        crk.bounds.reserve(24);
        crk.verts.reserve(numverts*8);
        crk.inds.reserve(numinds*3);
        
        for(i = 0; i < gv.size(); i++) {
            crk.bounds.emplace_back(gv[i]);
        }

        for(i = 0; i < fv.size(); i++) {
            crk.verts.emplace_back(fv[i]);
        }

        std::cout << fv.size();

        for(i = 0; i < iv.size(); i++) {
            crk.inds.emplace_back(iv[i]);
        }
        
        if( remove( filename ) != 0 )
        { std::cerr << "File " << filename << " couldn't be overwritten." << std::endl; }
        
        oFile out(filename, std::ios::binary);
        if (!out) { std::cerr << "Can't create file " << filename << std::endl; }

        out.write(retcchar(&crk.magik), 16);
        
        out.write(retchar(&crk.numVerts), 16);
        out.write(retchar(&crk.numInds), 16);

        out.write(retchar(&crk.bounds[0]), crk.bounds.size() * sizeof(frac16));
        out.write(retchar(&crk.verts[0]), crk.verts.size() * sizeof(frac16));
        out.write(retchar(&crk.inds[0]), crk.inds.size() * sizeof(uint16_t));

        out.close(); std::cout << "Succes writting to " << filename << std::endl;*/

    }

}