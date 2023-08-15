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
bool VoxelCheck(vecgeom::FlatVoxelHashMap<P, ScalarProperties>* VoxelMap, float x,float y, float z)
{
  /* Checks if a voxel is set to true. Returns true if it is, returns false if not. */
  vecgeom::Vector3D<float> pos(x, y, z);

  if (VoxelMap->isOccupied(pos)){
      return true;
  }

  return false;
}

template <typename P, bool ScalarProperties>
void AssignVoxelTrue(vecgeom::FlatVoxelHashMap<P, ScalarProperties>* VoxelMap, float x, float y, float z)
{
  /* Sets a voxel at a certain position to true, performs a check to make sure it is not already true before setting it. */
  vecgeom::Vector3D<float> pos(x, y, z);
 
  //If its already been set to true, don't touch:)
  if (VoxelCheck(VoxelMap, x,y,z)){ 
    return;
  }
  VoxelMap->addProperty(pos, true);

}



std::array<float, 3> CellToPoint(vecgeom::Vector3D<float> MinValues, vecgeom::Vector3D<float> Lengths, int NumbBins[3],
int Kx, int Ky, int Kz){
    /* Takes a cell position (Kx,Ky,Kz) and reutrns the position of the center of the bin */

    float delta_x = Lengths[0] / NumbBins [0];
    float delta_y = Lengths[1] / NumbBins [1];
    float delta_z = Lengths[2] / NumbBins [2];

    float x = MinValues[0] + delta_x*float(Kx) + delta_x*0.5;
    float y = MinValues[1] + delta_y*float(Ky) + delta_y*0.5;
    float z = MinValues[2] + delta_z*float(Kz) + delta_z*0.5;
    std::array<float,3> point = {x,y,z};
    return point;
}


void AddHashMaps(string HashMapFileName1, string HashMapFileName2, int Nx, int Ny, int Nz, string SaveMapLoc){
    /*
    HashMapFileName1 = First .root voxelmap 
    HashMapFileName2 = Second .root voxelMap 
    Nx,Ny,Nz = number of bins in X,Y,Z directions (assumed the same for all maps)
    SaveMapLoc = SavePath for the final map (include .root!)
    
    Function that adds 2 boolean hashmaps of the same(!!) dimension.
    True + True = True
    True + False = True
    False + False = False 
    */


std::unique_ptr<vecgeom::FlatVoxelHashMap<bool,true>> VoxelMap;
std::unique_ptr<vecgeom::FlatVoxelHashMap<bool,true>> VoxelMap1;
std::unique_ptr<vecgeom::FlatVoxelHashMap<bool,true>> VoxelMap2;

vecgeom::Vector3D<float> MinValues(-1000,-1000,-3000);
vecgeom::Vector3D<float> Lengths(2000,2000,6000); 
int NumbBins[3] = {Nx,Ny,Nz}; 

//The resultant map (well, it will be). 
VoxelMap = std::make_unique<vecgeom::FlatVoxelHashMap<bool,true>>(MinValues, Lengths, NumbBins[0],NumbBins[1],NumbBins[2]);

//Read the first map to add
if (HashMapFileName1 != ""){
  if (std::filesystem::exists(HashMapFileName1)){
    std::cout<< "File Exists" << std::endl;
    VoxelMap1.reset(vecgeom::FlatVoxelHashMap<bool,true>::readFromTFile(HashMapFileName1.c_str()));
  }

  else { std::cerr << "Hashmap file 1 does not exist." << std::endl;
  std::exit(EXIT_FAILURE);
    }
  }

//Read the second map to add
if (HashMapFileName2 != ""){
  if (std::filesystem::exists(HashMapFileName2)){
    std::cout<< "File Exists" << std::endl;
    VoxelMap2.reset(vecgeom::FlatVoxelHashMap<bool,true>::readFromTFile(HashMapFileName2.c_str()));
  }

  else { std::cerr << "Hashmap file 2 does not exist." << std::endl;
  std::exit(EXIT_FAILURE);
    }
  }


// Some check should be here for the dimensions of the hashmap, for now we will assume they are the same dimensions.


//Iterates over all voxels, if for a voxel, either hashmap is true, the final voxel will be true in the resultant hashmap
for (int i=0; i<Nx ; i++){

    for (int j=0; j<Ny ; j++){
        
        for (int k=0; k<Nz ; k++){
            std::array<float,3> pos  = CellToPoint(MinValues,Lengths,NumbBins,i,j,k);
            vecgeom::Vector3D<float> point(pos[0], pos[1], pos[2]);

            if (VoxelMap1->isOccupied(point) or VoxelMap2->isOccupied(point)){
                AssignVoxelTrue(VoxelMap.get(),pos[0],pos[1],pos[2]);
                
            }
        }
    }
}

//Saves the mapping to designated filepath
VoxelMap->dumpToTFile(SaveMapLoc.c_str());

}