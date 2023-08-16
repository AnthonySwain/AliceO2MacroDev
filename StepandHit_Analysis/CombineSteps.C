/*
    Combines all the different particles' steps together into one single 3D histogram
*/

#include <iostream>
#include <filesystem>
#include "Globals.h" //Get global variables for all macros



void CombineSteps(){
    /*
    Combines all the different particles' steps together into one single 3D histogram
    */

    TFile *file = new TFile("AllSteps.root","READ");
    TList* list;

    //Getting the file where all the different particle steps are saved (from analyzeSteps.C)
    file->GetObject("Allsteps",list);
    TH3I* hist;

    TH3I* combinedSteps = new TH3I("CombinedHistList", "AllParticleSteps",
    numb_bins[0],min_values[0],max_values[0],
    numb_bins[1],min_values[1],max_values[1],
    numb_bins[2],min_values[2],max_values[2]);
    
    //Iterates through all pdgs (see global file) to combine all of the histograms into one
    for (int j=0; j< sizeof(pdgs)/sizeof(int);j++)
    {   

        //std::cout << pdgs[j] << std::endl;
        //hist = ((TH3I*)list->FindObject((std::to_string(pdgs[j])).c_str()));
        combinedSteps->Add(((TH3I*)list->FindObject((std::to_string(pdgs[j])).c_str())));
        
    }

    //Save
    TFile *f = new TFile("AllStepsCombined.root","RECREATE");
    combinedSteps->Write("AllStepsCombined", TObject::kSingleKey);
    f->Close();

    
}

