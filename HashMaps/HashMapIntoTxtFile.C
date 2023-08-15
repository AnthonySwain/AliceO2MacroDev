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
#include <cstdlib>


void HashMapIntoTxtFile(){
  /*This is in development. It currently does not work:( */
std::unique_ptr<vecgeom::FlatVoxelHashMap<bool,true>> VoxelMap;

int NumbBins[3] = {200,200,600}; 
int Nx = NumbBins[0];
int Ny = NumbBins[1];
int Nz = NumbBins[2];

std::string HashMapFileName = "hashmap.root";

if (HashMapFileName != ""){
  if (std::filesystem::exists(HashMapFileName)){
    std::cout<< "File Exists" << std::endl;
    VoxelMap.reset(vecgeom::FlatVoxelHashMap<bool,true>::readFromTFile(HashMapFileName.c_str()));
  }

  else { std::cerr << "Hashmap file does not exist." << std::endl;
  std::exit(EXIT_FAILURE);
    }
  }

int length=0;
std::ofstream outputFile("HashMap.txt"); 
for (int i =0; i < Nx*Ny*Nz ; i++){
    if (VoxelMap->getPropertiesGivenKey(i,le  ngth)){
        outputFile << "1"; //Write 1
    }
    else{
        outputFile << "0"; //Write 0
    }
}

outputFile.close();

}