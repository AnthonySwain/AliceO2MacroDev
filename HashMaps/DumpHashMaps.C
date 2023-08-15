/* Dumping ground for hashmapcreation things, it's outdated now but some of the code could be usable somewhere else... (e.g. build walls)*/

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
vecgeom::Vector3D<float> FindVoxelCenter(vecgeom::FlatVoxelHashMap<P, ScalarProperties>* VoxelMap, float x,float y, float z)
{
  vecgeom::Vector3D<float> pos(x, y, z);
  auto key = VoxelMap->getVoxelKey(pos);
  return VoxelMap->keyToPos(key);
}

template <typename P, bool ScalarProperties>
bool VoxelCheck(vecgeom::FlatVoxelHashMap<P, ScalarProperties>* VoxelMap, float x,float y, float z)
{
  vecgeom::Vector3D<float> pos(x, y, z);
  //auto key = VoxelMap->getVoxelKey(pos);
  if (VoxelMap->isOccupied(pos)){
    //std::cout << "IS BLACKHOLE Particle Deleted, POSITION: " << x << ", " << y << ", " << z << "\n";
    return true;
  }
  return false;
}

template <typename P, bool ScalarProperties>
void AssignVoxelTrue(vecgeom::FlatVoxelHashMap<P, ScalarProperties>* VoxelMap, float x, float y, float z)
{
  vecgeom::Vector3D<float> pos(x, y, z);
  //auto key = (VoxelMap)->getVoxelKey(pos);

  //If its already been set to true, don't touch:)
  if (VoxelCheck(VoxelMap, x,y,z)){ 
    return;
  }
  VoxelMap->addProperty(pos, true);
  //std::cout << "BLACKHOLE Set, POSITION: " << x << ", " << y << ", " << z << "\n";
}

float UniformRandom(float upper_bound, float lower_bound){
  std::random_device rd;

  // Use the Mersenne Twister algorithm for random number generation
  std::mt19937 mt(rd());

  // Create a uniform distribution for integers within the range
  std::uniform_real_distribution<float> dist(lower_bound, upper_bound);

  float random_number = dist(mt);

  return(random_number);
}

template <typename P, bool ScalarProperties>
void RandomAllocation(vecgeom::FlatVoxelHashMap<P, ScalarProperties>* VoxelMap, int N, float Min, float Max){
  //Assign N random voxels near the center as true (i.e blachholes )
  for (int i =0; i < N; i++){
  float x = UniformRandom(Min,Max);
  float y = UniformRandom(Min,Max);
  float z = UniformRandom(Min,Max);

  AssignVoxelTrue(VoxelMap, x, y, z);
  }
}

template <typename P, bool ScalarProperties>
void BuildWallZYplane(vecgeom::FlatVoxelHashMap<P, ScalarProperties>* VoxelMap, float Xval, int thickness, int Zmin, int Zmax, int Ymin, int Ymax, int ZBins, int YBins){
  float deltaY = (Ymax - Ymin)/YBins;
  float deltaZ = (Zmax - Zmin)/ZBins;

  for (float k = 0; k < thickness ;k++){
  for (float j = (float(Ymin) + deltaY/2); j < float(Ymax); j += deltaY ){
    for (float i = (float(Zmin) + deltaZ/2); i < float(Zmax); i += deltaZ ){
      AssignVoxelTrue(VoxelMap, Xval+k, j, i);
  }}
  }
}

template <typename P, bool ScalarProperties>
void BuildWallXYplane(vecgeom::FlatVoxelHashMap<P, ScalarProperties>* VoxelMap, float Zval, int thickness, int Xmin, int Xmax, int Ymin, int Ymax, int XBins, int YBins){
  float deltaY = (Ymax - Ymin)/YBins;
  float deltaX = (Xmax - Xmin)/XBins;

  for (float k = 0; k<thickness; k++){
  for (float j = (float(Ymin) + deltaY/2); j < float(Ymax); j += deltaY ){
    for (float i = (float(Xmin) + deltaX/2); i < float(Xmax); i += deltaX ){
      AssignVoxelTrue(VoxelMap, i, j, Zval+k);
  }}
  }
}

template <typename P, bool ScalarProperties>
void BuildWallXZplane(vecgeom::FlatVoxelHashMap<P, ScalarProperties>* VoxelMap, float Yval, int thickness, int Xmin, int Xmax, int Zmin, int Zmax, int XBins, int ZBins){
  float deltaZ = (Zmax - Zmin)/ZBins;
  float deltaX = (Xmax - Xmin)/XBins;

  for (float k = 0; k<thickness; k++){
    for (float j = (float(Zmin) + deltaZ/2); j < float(Zmax); j += deltaZ ){
      for (float i = (float(Xmin) + deltaX/2); i < float(Xmax); i += deltaX ){
        AssignVoxelTrue(VoxelMap, i, Yval +k, j);
      }
    }
  }
}




template <typename P, bool ScalarProperties>
void BinaryListToMapping(vecgeom::FlatVoxelHashMap<P, ScalarProperties>* VoxelMap, string filename/*, std::list<bool> listOfValues ,int NbinX, int NbinsY, int NbinsZ*/){
  //Reads a binary list and converts it to a voxelmap for the optimisation

  std::vector<bool> data;
  char digit;
  std::ifstream file;
  file.open(filename);
  
  // Annoyingly, there is no way to read boolean directly from .txt files, this shouldn't take much time compared to the length of the simulations though. 
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
        VoxelMap->addPropertyForKey(i,data[i]);
        //std::cout << data[i];// << std::endl;   
    }
} 


template <typename P, bool ScalarProperties>
void CreateCircularLayersXYplane(vecgeom::FlatVoxelHashMap<P, ScalarProperties>* VoxelMap, int Nx, int Ny, int Nz,float minRadius,
float Xmin, float Ymin, float Zmin, float Xmax, float Ymax, float Zmax){
/*
Creates blackholes beyond a certain radius in the xy plane. E.g. if r = 5, everything beyond x^2 + y^2 = 25, would be blackholes for the particles. 
*/

float delta_X = (Xmax -Xmin)/float(Nx);
float delta_Z = (Zmax -Zmin)/float(Nz);

float yplus;
float yminus; 

vecgeom::Vector3D<float> PosYplus;

vecgeom::Vector3D<float> PosYminus;

//this is for one radius, need to do it for more
for (float radius = minRadius; radius < Xmax ; radius += delta_X/2){
  std::cout << radius << std::endl;
  for (float X = Xmin; X < Xmax; X += delta_X/2){

    //find y (remember 2 solns)
    yplus = sqrt( (radius * radius) - (X*X));
    yminus = -yplus; 

    //Assume Ymax = -Ymin
    if (yplus > Ymax){
      continue; //Outside the range of the voxel map, we don't care
    }

    for (float Z = Zmin; Z < Zmax; Z += delta_Z){ //over all Z 

      AssignVoxelTrue(VoxelMap, X,yplus,Z);
      AssignVoxelTrue(VoxelMap, X,yminus,Z);
      //VoxelMap->addProperty(PosYminus,true);
    }


}
}
}

/*
void DumpHashMaps(string HashInfoFile, string SaveMapLoc, int Nx, int Ny, int Nz){

    vecgeom::Vector3D<float> MinValues(-1000,-1000,-3000);
    vecgeom::Vector3D<float> Lengths(2000,2000,6000);
    int NumbBins[3] = {Nx,Ny,Nz};
    std::unique_ptr<vecgeom::FlatVoxelHashMap<bool,true>>VoxelMap = std::make_unique<vecgeom::FlatVoxelHashMap<bool,true>>(MinValues, Lengths, NumbBins[0],NumbBins[1],NumbBins[2]); 
    BinaryListToMapping(VoxelMap.get(),HashInfoFile);
    VoxelMap->dumpToTFile(SaveMapLoc.c_str());

}
*/


void DumpHashMaps(){

    string SaveMapLoc = "HashMap.root" ;
    int Nx = 200;
    int Ny = 200;
    int Nz = 600; 
    float minRadius = 250;

    //Hopefully pass the values here from config:) (or infer from the lenngth of the list of binary digits?)
    vecgeom::Vector3D<float> MinValues(-1000,-1000,-3000);
    vecgeom::Vector3D<float> Lengths(2000,2000,6000);
    int NumbBins[3] = {Nx,Ny,Nz};
    std::unique_ptr<vecgeom::FlatVoxelHashMap<bool,true>>VoxelMap = std::make_unique<vecgeom::FlatVoxelHashMap<bool,true>>(MinValues, Lengths, NumbBins[0],NumbBins[1],NumbBins[2]); 
    //CreateCircularLayersXYplane(VoxelMap.get(), Nx, Ny, Nz, minRadius,
    //MinValues[0],MinValues[1],MinValues[2], MinValues[0]+Lengths[0],MinValues[1]+Lengths[1],MinValues[2]+Lengths[2]);
    
    BuildWallXZplane(VoxelMap.get(), 150, 30, MinValues[0], MinValues[0]+Lengths[0], MinValues[2], MinValues[2]+Lengths[2], Nx,Nz);
    
    VoxelMap->dumpToTFile(SaveMapLoc.c_str());

}