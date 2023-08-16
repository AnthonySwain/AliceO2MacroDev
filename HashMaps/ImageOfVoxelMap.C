/* Creates projections/images of a voxel map from its root file */

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


void ProjectionHistogram(string projectionaxis, TH3I* hist, string savename){

  /* Creates desired 2D projection from a 3D histogram and saves the output as a .pdf file
    projection axis is the axis you want to take the projection of, eg xy
    hist is the 3D histogram where projections are taken from
    SaveName - desired name of the saved .pdf file*/

    TCanvas *c3 = new TCanvas("c3");
    TH1* projection = nullptr;
    projection = hist->Project3D(projectionaxis.c_str());
    projection -> SetStats(0);
    projection->SetContour(1000);

    char xtitle;
    char ytitle;

    //Not the best work-around but ah well - it works;) 
    if (projectionaxis == "yx"){
        projection ->GetXaxis()->SetTitle("x");
        projection ->GetYaxis()->SetTitle("y");
    }

    if (projectionaxis == "zx"){
        projection ->GetXaxis()->SetTitle("x");
        projection ->GetYaxis()->SetTitle("z");
    }

    if (projectionaxis == "zy"){
        projection ->GetXaxis()->SetTitle("y");
        projection ->GetYaxis()->SetTitle("z");
    }
    projection->GetXaxis()->SetTitleOffset(0.9);
    projection->GetYaxis()->SetTitleOffset(1.2);
    //projection->GetZaxis()->SetTitleOffset(1.4);
    //projection ->GetZaxis()->SetTitle("count");
    projection ->Draw("");

    c3->SetRightMargin(0.10);
    c3->SetLeftMargin(0.10);
    c3->SetBottomMargin(0.10);
    

    //Create directory to save and saving the plots
    std::filesystem::create_directories("./HashMapVisualisation");
    //string name = "./HashMapVisualisation/" + projectionaxis + savename +".pdf";
    //c3->Print(name.c_str());
    string name2 = "./HashMapVisualisation/" + projectionaxis + savename +".png";
    c3->Print(name2.c_str());
    c3->Close();
}



std::array<float, 3> CellToPoint(vecgeom::Vector3D<float> MinValues, vecgeom::Vector3D<float> Lengths, int NumbBins[3],
int Kx, int Ky, int Kz){
    /* Takes a cell position and reutrns the position of the center of the bin */

    float delta_x = Lengths[0] / NumbBins[0];
    float delta_y = Lengths[1] / NumbBins[1];
    float delta_z = Lengths[2] / NumbBins[2];

    float x = MinValues[0] + delta_x*float(Kx) + delta_x*0.5;
    float y = MinValues[1] + delta_y*float(Ky) + delta_y*0.5;
    float z = MinValues[2] + delta_z*float(Kz) + delta_z*0.5;
    std::array<float,3> point = {x,y,z};

    return point;

}


void ImageOfVoxelMap(std::string filepath, int Nx, int Ny, int Nz){
    /* Takes a .root voxel map and turns it into 2D root histograms */

vecgeom::Vector3D<float> MinValues(-1000,-1000,-3000);
vecgeom::Vector3D<float> Lengths(2000,2000,6000);
int NumbBins[3] = {Nx,Ny,Nz}; 

//Create histogram
TH3I* HashMapVisual =  new TH3I("CombinedHistList", "AllParticleHits",
Nx, MinValues[0], MinValues[0]+Lengths[0],
Ny, MinValues[1], MinValues[1]+Lengths[1],
Nz, MinValues[2], MinValues[2]+Lengths[2]);

//Initialise the pointer so it exists
std::unique_ptr<vecgeom::FlatVoxelHashMap<bool,true>> VoxelMap;

//Read the map from file
if (filepath != ""){
  if (std::filesystem::exists(filepath)){
    std::cout<< "File Exists" << std::endl;
    VoxelMap.reset(vecgeom::FlatVoxelHashMap<bool,true>::readFromTFile(filepath.c_str()));
  }

  else { std::cerr << "Hashmap filepath given does not exist." << std::endl;
  std::exit(EXIT_FAILURE);
    }
  }

//Iterate over all cells, adding their value to the corresponding position in the histogram 
for (int i = 0; i < Nx; i++){
    for (int j = 0; j < Ny; j++){
        for (int k = 0; k < Nz; k++){
            //Cell to point
            std::array<float,3> point = CellToPoint(MinValues,Lengths,NumbBins,i,j,k);
            
            //Check if it is occupied, if it is, fill!
            if (VoxelMap->isOccupied(vecgeom::Vector3D<float> (point[0], point[1], point[2]))){
               
                HashMapVisual->Fill(point[0],point[1],point[2],1);
            }
        }
    }
}

//Setting up the histogram for plotting
HashMapVisual->GetXaxis()->SetTitle("x");
HashMapVisual->GetYaxis()->SetTitle("y");
HashMapVisual->GetZaxis()->SetTitle("z");
HashMapVisual->SetTitle("Hashmap Visualisation");
HashMapVisual->SetContour(100);

TCanvas *c1 = new TCanvas("c1");

HashMapVisual->Draw("BOX2");

//Creating direcory to save the maps 
std::filesystem::create_directories("./HashMapVisualisation");
string name = "./HashMapVisualisation/3DVisual.pdf";
HashMapVisual->SetStats(0);
HashMapVisual->GetXaxis()->SetTitleOffset(1.8);
HashMapVisual->GetYaxis()->SetTitleOffset(1.8);
HashMapVisual->GetZaxis()->SetTitleOffset(1.5);
c1->SetRightMargin(0.10);
c1->SetLeftMargin(0.10);
c1->SetBottomMargin(0.10);
//Print the 3D histogram here if you wish to
c1->Close();    

//2D Projections
ProjectionHistogram("yx",HashMapVisual,"ProjectionHashmap");
ProjectionHistogram("zx",HashMapVisual,"ProjectionHashmap");
ProjectionHistogram("zy",HashMapVisual,"ProjectionHashmap");

}