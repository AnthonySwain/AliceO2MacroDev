/* Macro to create a voxelmap from a csv of cylinders */
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


template <typename P, bool ScalarProperties>
void CreateCircularLayersXYplane(vecgeom::FlatVoxelHashMap<P, ScalarProperties>* VoxelMap, int Nx, int Ny, int Nz,float minRadius, 
float Xmin, float Ymin, float Zmin, float Xmax, float Ymax, float Zmax){
/*
Creates blackholes beyond a certain radius in the xy plane. E.g. if r = 5, everything beyond x^2 + y^2 = 25, would be blackholes for the particles. 
*/

//Width of the cells
float delta_X = (Xmax -Xmin)/float(Nx);
float delta_Y = (Ymax -Ymin)/float(Ny);
float delta_Z = (Zmax -Zmin)/float(Nz);

//Initialising for later
float yplus;
float yminus;
float xplus;
float xminus;

//Iterate over all radii possible
for (float radius = minRadius; radius < sqrt(2)*Xmax ; radius += delta_X/2){
  //std::cout << radius << std::endl;
  for (float X = Xmin; X < Xmax; X += delta_X/2){ 
    //find y (remember 2 solns for each X position)
    yplus = sqrt( (radius * radius) - (X*X));
    yminus = -yplus; 

    //Assuming Ymax = -Ymin
    if (yplus > Ymax){
      continue; //Outside the range of the voxel map
    }

    for (float Z = Zmin; Z < Zmax; Z += delta_Z){ //Assign the voxels to true over all Z 
      AssignVoxelTrue(VoxelMap, X,yplus,Z);
      AssignVoxelTrue(VoxelMap, X,yminus,Z);
    }
  }

  //This is to counter-act whack sampling around y=0
  for (float Y = Ymin; Y < Ymax; Y += delta_Y/2){ 

    //find y (remember 2 solns for each X position)
    xplus = sqrt( (radius * radius) - (Y*Y));
    xminus = -xplus; 

    //Assuming Xmax = -Xmin
    if (xplus > Xmax){
      continue; //Outside the range of the voxel map
    }

    for (float Z = Zmin; Z < Zmax; Z += delta_Z){ //Assign the voxels to true over all Z 
      AssignVoxelTrue(VoxelMap, xplus,Y,Z);
      AssignVoxelTrue(VoxelMap, xminus,Y,Z);
    }
  }
}
}



struct Cylinder_Data {
      int Zmin;
      int Zmax;
      float radius;
      std::string To_check;
      std::vector<int> PDGs;
      };

std::vector<std::string> splitString(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

Cylinder_Data parseCSVLine(const std::string &line) {
    Cylinder_Data entry;
    std::vector<std::string> tokens = splitString(line, ',');
    
    if (tokens.size() < 4) {
        std::cerr << "Invalid CSV line: " << line << std::endl;
        return entry;
    }

    if (tokens[0] == "Zmin"){
      return entry;
    }
    
    entry.Zmin = std::stof(tokens[0]);
    entry.Zmax = std::stof(tokens[1]);
    entry.radius = std::stof(tokens[2]);
    entry.To_check = tokens[3];

    for (size_t i = 4; i < tokens.size(); ++i) {
        entry.PDGs.push_back(std::stoi(tokens[i]));
    }
    
    return entry;
}





std::vector<Cylinder_Data> readCSVFileCylinderCuts(const std::string &filename) {
    std::vector<Cylinder_Data> dataEntries;
    std::ifstream file(filename);
    std::string line;
    
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return dataEntries;
    }
    
    while (std::getline(file, line)) {
        dataEntries.push_back(parseCSVLine(line));
    }
    
    file.close();
    return dataEntries;
}






//Creates voxel map from a CSV of cylinders.
void CreateVoxelMapFromCylindersCSV(string SaveMapLoc, int Nx, int Ny, int Nz, float minRadius, string cylinder_csv_filepath){
    /* 
    Creates radial hashmaps in the XY direction
    SaveMapLoc = where to save the resulting map (include .root)
    Nx, Ny, Nz = Number of bins in the X,Y,Z directions respectively
    minRadius = minimum radius to start creating black holes (i.e. everything outside of this radius will be a blackhole)
    Zmin/Zmax - extent in the Z direction (beam axis)
    */
    
    //Open up the CSV and retrieve the data. 
    std::vector<Cylinder_Data> cylinder_data = readCSVFileCylinderCuts(cylinder_csv_filepath);

    vecgeom::Vector3D<float> MinValues(-1000,-1000,-3000);
    vecgeom::Vector3D<float> Lengths(2000,2000,6000);
    int NumbBins[3] = {Nx,Ny,Nz};

    std::unique_ptr<vecgeom::FlatVoxelHashMap<bool,true>>VoxelMap = std::make_unique<vecgeom::FlatVoxelHashMap<bool,true>>(MinValues, Lengths, NumbBins[0],NumbBins[1],NumbBins[2]); 
    
    //Loop through all the cylinders, creating the voxelmap.
    for (auto& cylinder : cylinder_data){
      CreateCircularLayersXYplane(VoxelMap.get(), Nx, Ny, Nz, cylinder.radius,
    MinValues[0],MinValues[1],cylinder.Zmin, MinValues[0]+Lengths[0],MinValues[1]+Lengths[1],cylinder.Zmax);

    }
    

    VoxelMap->dumpToTFile(SaveMapLoc.c_str());

}