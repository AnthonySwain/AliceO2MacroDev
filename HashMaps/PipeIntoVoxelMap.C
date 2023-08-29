/*
Accepts information from a pipe in python and creates a voxel map from the data. 
*/

#if defined(__linux__)
R__ADD_INCLUDE_PATH($VECGEOM_ROOT/../../Vc/latest/include)
#endif

R__ADD_INCLUDE_PATH($VECGEOM_ROOT/include)

#if defined(__linux__)
#include <VecCore/Types.h>
#include <VecCore/Common.h>
#include <VecCore/Backend/Implementation.h>
#include <VecCore/Backend/Interface.h>
#include <VecCore/Backend/VcVector.h>
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <VecGeom/base/FlatVoxelHashMap.h>
#include <cmath>
#include <cstdint>



template <typename P, bool ScalarProperties>
void PipeToMapping(vecgeom::FlatVoxelHashMap<P, ScalarProperties>* VoxelMap){
  //Reads a binary list and converts it to a voxelmap for the optimisation

  std::vector<bool> data;
  char digit;
  bool datum;


// Read data bytes from stdin
while (std::cin.read(reinterpret_cast<char*>(&datum), sizeof(bool))) {
    data.push_back(datum);
}

  
  for (size_t i = 0; i < data.size(); ++i) {

        if (data[i]==false){ 
          /*DO NOT write the key as false, it is this by default and writing it makes 
          a) Occupied which messes up how particles are deleted (the voxel is checked to see if it is occupied, not set to true)
          b) Takes up space that isn't needed to be taken as it's default is false / not occupied */
          continue;
        }

        //Set the voxel to true
        VoxelMap->addPropertyForKey(i,data[i]);
    }
} 



//from a random txt files with 0s and 1s in. 
void PipeIntoVoxelMap(string SaveMapLoc, int Nx, int Ny, int Nz, float MinX, float MinY, float MinZ, float LengthX, float LengthY, float LengthZ){
    /* Creates a boolean hashmap from a single line .txt file containing 0's & 1s (no spacing, e.g. 0101000010) 
    HashInfoFile = filepath to the .txt file 
    SaveMapLoc = where to save the created map (include .root)
    Nx,Ny,Nz = number of bins in X,Y,Z directions respectively (be careful that Nx,Ny,Nz = total number of 0's/1's)*/

    vecgeom::Vector3D<float> MinValues(MinX,MinY,MinZ);
    vecgeom::Vector3D<float> Lengths(LengthX,LengthY,LengthZ);
    int NumbBins[3] = {Nx,Ny,Nz};
    std::unique_ptr<vecgeom::FlatVoxelHashMap<bool,true>>VoxelMap = std::make_unique<vecgeom::FlatVoxelHashMap<bool,true>>(MinValues, Lengths, NumbBins[0],NumbBins[1],NumbBins[2]); 
    
    //Does what it says on the tin
    PipeToMapping(VoxelMap.get(),HashInfoFile);
    
    //Saves the mapping
    VoxelMap->dumpToTFile(SaveMapLoc.c_str());

}

