//Macro to create various projections from a 3D histograms of data. 
/* 
Currently this code has a load of functions, 
loads in the 3D Histograms of hits and steps from CombinedDetectorHits.C and CombinedSteps.C
and makes various plots of them from the functions + whatever I have added. All the output files get dumped into a folder in the cwd
*/


#include <algorithm>
#include <iostream>
#include <filesystem>
#include <math.h>  
#include "Globals.h" //Get global variables for all macros

void HitStepGraphRadial(TH3I* HitHist, TH3I* StepHist, TH3F* HitOverStepHist){
    /*
    Quite clunky function that I have not really used any of the graphs from. It creates a 3D graph of 
    hits/steps against hits/hits_total against radius over all voxels. 
    */

    int TotalHits = HitHist->GetEntries();

    int BinsX = HitHist->GetNbinsX();
    int BinsY = HitHist->GetNbinsY();
    int BinsZ = HitHist->GetNbinsZ();

    int BinNumb;
    int NumbHits;
    int NumbSteps;

    int Radius;
    int x;
    int y;

    //Setting up scatter graph
    auto graph = new TGraph2D();

    //Iterate over all voxels
    for (int i = 0; i <BinsX;i++)
    {
        for (int j=0; j <BinsY; j++)
        {
            for (int k=0; k<BinsZ; k++)
            {   
                //I will assume the bins will be numbered the same for all 3 histograms as the only difference is their entries inside
                int BinNumb = HitHist->GetBin(i,j,k);
                NumbHits = HitHist->GetBinContent(BinNumb);
                NumbSteps = StepHist->GetBinContent(BinNumb);

                if (NumbSteps ==0) //Done so no divide by 0 errors.
                {continue;}

                else{
                x = StepHist->GetXaxis()->GetBinCenter(i);
                y = StepHist->GetYaxis()->GetBinCenter(j);

                Radius = sqrt((x*x) + (y*y));
                graph->AddPoint(/*BinNumb,*/(float(NumbHits)/float(NumbSteps)),(float(NumbHits)/float(TotalHits)),Radius);
                //std::cout<< (float(NumbHits)/float(NumbSteps))<<std::endl;}
                }
            }
        }
    }

    //Titles
    graph->SetTitle("Ratio for Hits/Steps to Hits/Total Hits for each voxel against radius");
    graph->GetXaxis()->SetTitle("Hits/Steps");
    graph->GetYaxis()->SetTitle("Hits/Total Hits");
    graph->GetZaxis()->SetTitle("Radius");
    

    TCanvas* c1 = new TCanvas();

    //Print
    graph->SetMarkerStyle(20);
    graph->Draw("pcol");
    string name = "./AllParticlesListed/HitStepRatioGraphRadial.png";
    c1->Print(name.c_str());
    //c1->Close();
}

void HitStepGraph(TH3I* HitHist, TH3I* StepHist, TH3F* HitOverStepHist){
    /*
    Quite clunky function that I have not really used any of the graphs from. It creates a 2D graph of 
    hits/steps against hits/hits_total 
    */
    int TotalHits = HitHist->GetEntries();

    int BinsX = HitHist->GetNbinsX();
    int BinsY = HitHist->GetNbinsY();
    int BinsZ = HitHist->GetNbinsZ();

    int BinNumb;
    int NumbHits;
    int NumbSteps;



    //Setting up scatter graph
    auto graph = new TGraph();

    //Iterate over all voxels
    for (int i = 0; i <BinsX;i++)
    {
        for (int j=0; j <BinsY; j++)
        {
            for (int k=0; k<BinsZ; k++)
            {   
                //I assume the bins will be numbered the same for all 3 histograms as the only difference is their entries 
                int BinNumb = HitHist->GetBin(i,j,k);
                NumbHits = HitHist->GetBinContent(BinNumb);
                NumbSteps = StepHist->GetBinContent(BinNumb);
                if (NumbSteps ==0)
                {continue;}
                else{
                graph->AddPoint((float(NumbHits)/float(NumbSteps)),(float(NumbHits)/float(TotalHits)));
                //std::cout<< (float(NumbHits)/float(NumbSteps))<<std::endl;}
                }
            }
        }
    }

    graph->SetTitle("Ratio for Hits/Steps to Hits/Total Hits for each voxel");
    graph->GetXaxis()->SetTitle("Hits/Steps");
    graph->GetYaxis()->SetTitle("Hits/Total Hits");

    TCanvas* c1 = new TCanvas();

    graph->Draw("A * P");
    string name = "./AllParticlesListed/HitStepRatioGraph.png";
    c1->Print(name.c_str());
    c1->Close();
}

void CountStepsZYplane(TH3I* histogram, int Xval){
    /*
    Counts the number of steps either side of a wall, made for debugging / checks 
    If Xval = 170, it looks at steps above X=170 and steps below X=-170 and prints those values respectively 
    (to check if a wall in a voxel map is working)
    */
    std::cout << "Before Cuts: " << histogram->GetEntries() << std::endl;
    //perform some kind of cut

    //Left side of the wall
    int entries = histogram->GetEntries();
    //std::cout << histogram->GetEntries() << std::endl;

    unsigned long long int entriesAboveThreshold = 0;
    unsigned long long int entriesBelowThreshold = 0;


    //Iterates over all bins
    for (int xBin = 1; xBin <= histogram->GetNbinsX(); ++xBin) {
        for (int yBin = 1; yBin <= histogram->GetNbinsY(); ++yBin) {
            for (int zBin = 1; zBin <= histogram->GetNbinsZ(); ++zBin) {
                double xValue = histogram->GetXaxis()->GetBinCenter(xBin);
                double yValue = histogram->GetYaxis()->GetBinCenter(yBin);
                double zValue = histogram->GetZaxis()->GetBinCenter(zBin);

                if (xValue > Xval) {
                    entriesAboveThreshold += histogram->GetBinContent(xBin, yBin, zBin);
                }
                if (xValue < -Xval) {
                    entriesBelowThreshold += histogram->GetBinContent(xBin, yBin, zBin);
                }
            }
        }
    }
  
    std::cout << "Entries to the right of X = " << Xval << " = " << entriesAboveThreshold << std::endl;

    std::cout << "Entries to the left of X = -" << Xval << " = " << entriesBelowThreshold << std::endl;
}

void CountStepsXZplane(TH3I* histogram, int Yval){
    /*
    Counts the number of steps either side of a wall, made for debugging / checks 
    If Yval = 170, it looks at steps above Y=170 and steps below Y=-170 and prints those values respectively 
    (to check if a wall in a voxel map is working)
    */
   
    std::cout << "Before Cuts: " << histogram->GetEntries() << std::endl;

    int entries = histogram->GetEntries();

    unsigned long long int entriesAboveThreshold = 0;
    unsigned long long int entriesBelowThreshold = 0;

    //Iterate over all bins
    for (int xBin = 1; xBin <= histogram->GetNbinsX(); ++xBin) {
        for (int yBin = 1; yBin <= histogram->GetNbinsY(); ++yBin) {
            for (int zBin = 1; zBin <= histogram->GetNbinsZ(); ++zBin) {
                double xValue = histogram->GetXaxis()->GetBinCenter(xBin);
                double yValue = histogram->GetYaxis()->GetBinCenter(yBin);
                double zValue = histogram->GetZaxis()->GetBinCenter(zBin);

                if (yValue > Yval) {
                    entriesAboveThreshold += histogram->GetBinContent(xBin, yBin, zBin);
                }
                if (yValue < -Yval) {
                    entriesBelowThreshold += histogram->GetBinContent(xBin, yBin, zBin);
                }
            }
        }
    }
  
    std::cout << "Entries above Y = " << Yval << " = " << entriesAboveThreshold << std::endl;

    std::cout << "Entries below y = -" << Yval << " = " << entriesBelowThreshold << std::endl;
}

void ProjectionHistogramExcludeBeamPipe(string projectionaxis, TH3I* hist, string savename,int xMin, int xMax, int yMin, int yMax, int zMin, int zMax){

    /* 
    Creates projections off a 3D histogram where the beampipe is excluded (mainly to see more fine details in the steps/hits as the 
    beam pipe contains orders of magnitude more steps (and a bit more hits))
    
    projection axis is the axis you want to take the projection of, eg xy
    hist is the 3D histogram that the projections come from
    x/y/z Min | x/y/z Max are the respective min and max values to cut out. (It's not really the beam pipe, its a bit more generalsied)

    So xMin=0 xMax=20, yMin=0, yMax=20 gets rid of a radius 20 circle in the x,y plane down the entire z axis
    */

    TCanvas *c3 = new TCanvas("c3");
    TH1* projection = nullptr;
    projection = hist->Project3D(projectionaxis.c_str());
    projection -> SetStats(0);
    projection->SetContour(1000);
    
    char xtitle;
    char ytitle;


    //Not the best work-around but ah well - it works;) 

    /*
    There are only 3 projections allowed, there should be an error drop out if one isn't selected. 
    Eachprojection sets the beam axis to 0 and plots it. 
    */
    if (projectionaxis == "yx"){
        projection ->GetXaxis()->SetTitle("x");
        projection ->GetYaxis()->SetTitle("y");

        int binXMin = hist->GetXaxis()->FindFixBin(xMin);
        int binXMax = hist->GetXaxis()->FindFixBin(xMax);
        int binYMin = hist->GetYaxis()->FindFixBin(yMin);
        int binYMax = hist->GetYaxis()->FindFixBin(yMax);

        for (int i = binXMin; i <= binXMax; i++) {
            for (int j = binYMin; j <= binYMax; j++) {
                 projection->SetBinContent(i, j, 0); // Set the bin content to zero
            
        }
        }
    }

    if (projectionaxis == "zx"){
        projection ->GetXaxis()->SetTitle("x");
        projection ->GetYaxis()->SetTitle("z");
        int binXMin = hist->GetXaxis()->FindFixBin(xMin);
        int binXMax = hist->GetXaxis()->FindFixBin(xMax);
        int binYMin = hist->GetYaxis()->FindFixBin(zMin);
        int binYMax = hist->GetYaxis()->FindFixBin(zMax);

        for (int i = binXMin; i <= binXMax; i++) {
            for (int j = binYMin; j <= binYMax; j++) {
                 projection->SetBinContent(i, j, 0); // Set the bin content to zero
           
        }
        }

    }

    if (projectionaxis == "zy"){
        projection ->GetXaxis()->SetTitle("y");
        projection ->GetYaxis()->SetTitle("z");

        int binXMin = hist->GetXaxis()->FindFixBin(xMin);
        int binXMax = hist->GetXaxis()->FindFixBin(xMax);
        int binYMin = hist->GetYaxis()->FindFixBin(zMin);
        int binYMax = hist->GetYaxis()->FindFixBin(zMax);

        for (int i = binXMin; i <= binXMax; i++) {
            for (int j = binYMin; j <= binYMax; j++) {
                 projection->SetBinContent(i, j, 0); // Set the bin content to zero
           
        }
        }
    }
    projection->GetXaxis()->SetTitleOffset(0.9);
    projection->GetYaxis()->SetTitleOffset(1.2);
    projection->GetZaxis()->SetTitleOffset(1.4);
    projection ->GetZaxis()->SetTitle("count");
    projection ->Draw("colz");

    c3->SetRightMargin(0.20);
    c3->SetLeftMargin(0.10);
    c3->SetBottomMargin(0.10);
    

    //Create directory to save and saving the plots
    std::filesystem::create_directories("./AllParticlesListed");
    string name = "./AllParticlesListed/" + projectionaxis + savename +"ExcludedCenter.pdf";
    c3->Print(name.c_str());
    c3->Close();
}

void ProjectionHistogram(string projectionaxis, TH3I* hist, string savename){

    /*
    projection axis is the axis you want to take the projection of, eg xy     
    There are only 3 projections allowed, there should be an error drop out if one isn't selected (NOT IMPLEMENTED)
    hist is the 3D histogram
    savename = savename of the projection... 
    */

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
    projection->GetZaxis()->SetTitleOffset(1.4);
    projection ->GetZaxis()->SetTitle("count");
    projection ->Draw("colz");

    c3->SetRightMargin(0.20);
    c3->SetLeftMargin(0.10);
    c3->SetBottomMargin(0.10);
    

    //Create directory to save and saving the plots
    std::filesystem::create_directories("./AllParticlesListed");
    //string name = "./AllParticlesListed/" + projectionaxis + savename +".pdf";
    //c3->Print(name.c_str());
    string name2 = "./AllParticlesListed/" + projectionaxis + savename +".png";
    c3->Print(name2.c_str());
    c3->Close();
}

TList* openhistlist(std::string filepath)
{
    /* Opens a file and a TList in the file, specifically for the step files here however. */
    TFile* file = new TFile(filepath.c_str(),"READ");
    TList* list;
    file->GetObject("Allsteps",list);
    return(list);
}

void ProjectionHistogram2(string projectionaxis, TH3F* hist, string savename){
    //I can't quite remember why there are 2 of this function, they look the same (and are the same?)

    /*
    projection axis is the axis you want to take the projection of, eg xy
    There are only 3 projections allowed, there should be an error drop out if one isn't selected (NOT IMPLEMENTED)
    hist is the 3D histogram
    */

    TCanvas *c3 = new TCanvas("c3");
    TH1* projection = nullptr;
    projection = hist->Project3D(projectionaxis.c_str());
    projection -> SetStats(0);

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
    projection->GetZaxis()->SetTitleOffset(1.4);
    projection->SetContour(1000);
    projection ->GetZaxis()->SetTitle("count");
    projection ->Draw("colz");


    c3->SetRightMargin(0.20);
    c3->SetLeftMargin(0.10);
    c3->SetBottomMargin(0.10);
    

    //Create directory to save and saving the plots
    std::filesystem::create_directories("./AllParticlesListed");
    string name = "./AllParticlesListed/" + projectionaxis + savename +".pdf";
    c3->Print(name.c_str());
    c3->Close();
}

bool file_exists2(const std::string &filename) {
    /*Checks whether a file exists*/
  return std::filesystem::exists(filename);
}

void AllProjections(){
    //Main macro program


    //Opening HitsHistogram
    string filepathHits = "AllHits.root";
    TFile* fileHits = new TFile(filepathHits.c_str(),"READ");
    TH3I* histHits = nullptr;
    fileHits->GetObject("AllHits",histHits);
    //fileHits->Close();
  
    //Hits Histogram Projections
    histHits->GetXaxis()->SetTitle("x");
    histHits->GetYaxis()->SetTitle("y");
    histHits->GetZaxis()->SetTitle("z");
    histHits->SetTitle("Hits in ALICE particle detector");
    histHits->SetContour(1000);
    
    TCanvas *c1 = new TCanvas("c1");
    histHits->Draw("BOX2");

    //Creating directory to save all the files
    std::filesystem::create_directories("./AllParticlesListed");

    //Some root drawing stuff
    string name = "./AllParticlesListed/3DHits.pdf";
    histHits->SetStats(0);
    histHits->GetXaxis()->SetTitleOffset(1.8);
    histHits->GetYaxis()->SetTitleOffset(1.8);
    histHits->GetZaxis()->SetTitleOffset(1.5);
    c1->SetRightMargin(0.10);
    c1->SetLeftMargin(0.10);
    c1->SetBottomMargin(0.10);
    c1 ->Print(name.c_str());
    c1->Close();    

    
    //Various functions we want to run on the HitsHistogram
    ProjectionHistogram("yx",histHits,"ProjectionHits");
    ProjectionHistogram("zx",histHits,"ProjectionHits");
    ProjectionHistogram("zy",histHits,"ProjectionHits");

    ProjectionHistogramExcludeBeamPipe("yx",histHits,"ProjectionHits",-25,25,-25,25,-3000,3000);
    ProjectionHistogramExcludeBeamPipe("zx",histHits,"ProjectionHits",-25,25,-25,25,-3000,3000);
    ProjectionHistogramExcludeBeamPipe("zy",histHits,"ProjectionHits",-25,25,-25,25,-3000,3000);


    //Opening StepsHistogram
    string filepathSteps = "AllStepsCombined.root";
    TFile* fileSteps = new TFile(filepathSteps.c_str(),"READ");
    TH3I* histSteps = nullptr;
    fileSteps->GetObject("AllStepsCombined",histSteps);
    //fileSteps->Close();

    //Steps Histogram Projections
    histSteps->GetXaxis()->SetTitle("x");
    histSteps->GetYaxis()->SetTitle("y");
    histSteps->GetZaxis()->SetTitle("z");
    histSteps->SetTitle("Steps in ALICE particle detector");
    histSteps->SetContour(1000);
    
    TCanvas *c2 = new TCanvas("c2");
    string name2 = "./AllParticlesListed/3DSteps.pdf";
    histSteps->Draw("");
    //histSteps->SetStats(1);
    histSteps->GetXaxis()->SetTitleOffset(1.8);
    histSteps->GetYaxis()->SetTitleOffset(1.8);
    histSteps->GetZaxis()->SetTitleOffset(1.5);
    c2->SetRightMargin(0.10);
    c2->SetLeftMargin(0.10);
    c2->SetBottomMargin(0.10);
    c2->Print(name2.c_str());
    c2->Close();
    

    //Various functions we want to run on the StepsHistogram
    ProjectionHistogram("yx",histSteps,"ProjectionSteps");
    ProjectionHistogram("zx",histSteps,"ProjectionSteps");
    ProjectionHistogram("zy",histSteps,"ProjectionSteps");

    ProjectionHistogramExcludeBeamPipe("yx",histSteps,"ProjectionSteps",-25,25,-25,25,-3000,3000);
    ProjectionHistogramExcludeBeamPipe("zx",histSteps,"ProjectionSteps",-25,25,-25,25,-3000,3000);
    ProjectionHistogramExcludeBeamPipe("zy",histSteps,"ProjectionSteps",-25,25,-25,25,-3000,3000);



//////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // Hits Over Steps and their Projections
    TH3F* HitsDividedSteps = new TH3F("Hits/Steps", "Hits/Steps",
    numb_bins[0],min_values[0],max_values[0],
    numb_bins[1],min_values[1],max_values[1],
    numb_bins[2],min_values[2],max_values[2]);

    int HitBinsX = histHits->GetNbinsX();
    int HitBinsY = histHits->GetNbinsY();
    int HitBinsZ = histHits->GetNbinsZ();

    int NumbHits;
    int NumbSteps;


    //Iterate over all bins, dividing hits by steps (if steps !=0)
    for (int i = 0; i <HitBinsX;i++)
    {
        for (int j=0; j <HitBinsY; j++)
        {
            for (int k=0; k<HitBinsZ; k++)
            {   
                //I assume the bins will be numbered the same for all 3 histograms as the only difference is their entries? 
                int BinNumb = histHits->GetBin(i,j,k);
                NumbHits = histHits->GetBinContent(BinNumb);
                NumbSteps = histSteps->GetBinContent(BinNumb);

                if (NumbSteps ==0) //So there is no divide by 0 error
                {continue;}
                else{
                HitsDividedSteps->AddBinContent(BinNumb,(float(NumbHits)/float(NumbSteps)));
                
                }
            }
        }
    }

    //More root drawing stuff
    HitsDividedSteps->GetXaxis()->SetTitle("x");
    HitsDividedSteps->GetYaxis()->SetTitle("y");
    HitsDividedSteps->GetZaxis()->SetTitle("z");
    HitsDividedSteps->SetTitle("Hits/Steps in ALICE particle detector");
    HitsDividedSteps->SetContour(1000);

    TCanvas* c3 = new TCanvas("c3");
    HitsDividedSteps->SetStats(0);
    HitsDividedSteps->Draw("BOX2");
    HitsDividedSteps->GetXaxis()->SetTitleOffset(1.8);
    HitsDividedSteps->GetYaxis()->SetTitleOffset(1.8);
    HitsDividedSteps->GetZaxis()->SetTitleOffset(1.5);
    string name3 = "./AllParticlesListed/HitsOverSteps.pdf";
    c3->SetRightMargin(0.10);
    c3->SetLeftMargin(0.10);
    c3->SetBottomMargin(0.10);
    c3 ->Print(name3.c_str());
    c3->Close();

    //Functions we want to run on the hits/steps histogram
    ProjectionHistogram2("yx",HitsDividedSteps,"ProjectionHitsOverSteps");
    ProjectionHistogram2("zx",HitsDividedSteps,"ProjectionHitsOverSteps");
    ProjectionHistogram2("zy",HitsDividedSteps,"ProjectionHitsOverSteps");
    
    HitStepGraph(histHits,histSteps,HitsDividedSteps);
    HitStepGraphRadial(histHits,histSteps,HitsDividedSteps);

    //CountStepsZYplane(histSteps, 100);
    //CountStepsZYplane(histHits, 100);




    CountStepsXZplane(histSteps, 170);
    CountStepsXZplane(histHits, 170);
  
   
    }


