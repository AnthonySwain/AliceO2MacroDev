//Macro to create various projections from a 3D histogram of data. 
//Code is quite dirty currently, needs a clean-up

#include <algorithm>
#include <iostream>
#include <filesystem>


void ProjectionHistogram(string projectionaxis, TH3I* hist){

  /*projection axis is the axis you want to take the projection of, eg xy
    hist is the 3D histogram*/

    TCanvas *c3 = new TCanvas("c3");
    TH1* projection = hist->Project3D(projectionaxis.c_str());
    projection -> SetStats(0);


   //cout << ytitle << endl;
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

    projection ->GetZaxis()->SetTitle("count");
    projection ->Draw("colz");

    c3->SetRightMargin(0.15);
    c3->SetLeftMargin(0.15);
    c3->SetBottomMargin(0.10);

    //Create directory to save and saving the plots
    std::filesystem::create_directories("./AllParticlesListed");
    string name = "./AllParticlesListed/" + projectionaxis + "Projection.pdf";
    c3 ->Print(name.c_str());
}

TList* openhistlist(std::string filepath)
{
    TFile* file = new TFile(filepath.c_str(),"READ");
    TList* list;
    file->GetObject("Allsteps",list);
    return(list);
}


bool file_exists(const std::string &filename) {
  return std::filesystem::exists(filename);
}

void ProjectionsAllParticles(){
    
    string filepath = "/home/answain/alice/O2Sim/AllSteps.root";
    TFile* file = new TFile(filepath.c_str(),"READ");
    TH3I* allsteps;
    file->GetObject("Allsteps",allsteps);
   
    
    TH3I* hist = (TH3I*)allsteps->FindObject("Allsteps");
    //Titles n stuff 3DHist
    hist->GetXaxis()->SetTitle("x");
    hist->GetYaxis()->SetTitle("y");
    hist->GetZaxis()->SetTitle("z");
    hist->SetTitle("Steps in ALICE particle detector");
    hist->SetContour(1000);

    TCanvas *c1 = new TCanvas("c1");
    hist->Draw();

    std::filesystem::create_directories("./AllParticlesListed");
    string name = "./AllParticlesListed/3D.pdf";
    c1 ->Print(name.c_str());


    ProjectionHistogram("yx",hist);
    ProjectionHistogram("zx",hist);
    ProjectionHistogram("zy",hist);
    
    }

