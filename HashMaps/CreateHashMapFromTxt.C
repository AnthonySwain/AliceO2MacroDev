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




template <typename P, bool ScalarProperties>
void BinaryListToMapping(vecgeom::FlatVoxelHashMap<P, ScalarProperties>* VoxelMap, string filename/*, std::list<bool> listOfValues ,int NbinX, int NbinsY, int NbinsZ*/){
  //Reads a binary list and converts it to a voxelmap for the optimisation

  std::vector<bool> data;
  char digit;
  std::ifstream file;
  file.open(filename);
  
  // Annoyingly, there is no way to read boolean directly from .txt files, this shouldn't take much time compared to the length of the simulations though. 
  //Below reads the .txt file digit by digit and adds it onto the vector
  if (file.is_open()) {
    while (file >> digit) {
      if (digit == '0'){
        data.push_back(false);
      }

      if (digit == '1'){
        data.push_back(true);
      }
  }
  file.close();
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
void CreateHashMapFromTxt(string HashInfoFile, string SaveMapLoc, int Nx, int Ny, int Nz){
    /* Creates a boolean hashmap from a single line .txt file containing 0's & 1s (no spacing, e.g. 0101000010) 
    HashInfoFile = filepath to the .txt file 
    SaveMapLoc = where to save the created map (include .root)
    Nx,Ny,Nz = number of bins in X,Y,Z directions respectively (be careful that Nx,Ny,Nz = total number of 0's/1's)*/

    vecgeom::Vector3D<float> MinValues(-1000,-1000,-3000);
    vecgeom::Vector3D<float> Lengths(2000,2000,6000);
    int NumbBins[3] = {Nx,Ny,Nz};
    std::unique_ptr<vecgeom::FlatVoxelHashMap<bool,true>>VoxelMap = std::make_unique<vecgeom::FlatVoxelHashMap<bool,true>>(MinValues, Lengths, NumbBins[0],NumbBins[1],NumbBins[2]); 
    
    //Does what it says on the tin
    BinaryListToMapping(VoxelMap.get(),HashInfoFile);
    
    //Saves the mapping
    VoxelMap->dumpToTFile(SaveMapLoc.c_str());

}

