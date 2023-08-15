/*
    Macro that combines all the detectors hits together into one 3D histogram, also combining the different particle types
*/

#include <iostream>
#include <filesystem>
#include "Globals.h" //Get global variables for all macros

void CombineDetectorHits(){
    /*
    Macro that combines all the detectors hits together into one 3D histogram, also combining the different particle types
    */

    TFile *file = new TFile("HitsInDetectorsHistograms.root","READ");
    TList* list;
    TH3I* hist;

    TH3I* combinedHits = new TH3I("CombinedHistList", "AllParticleHits",
    numb_bins[0],min_values[0],max_values[0],
    numb_bins[1],min_values[1],max_values[1],
    numb_bins[2],min_values[2],max_values[2]);
    
    //Iterates through all the detectors (see global file) and also through the particles saved (also see global file)
    //Adding all the histograms together      
    for (int i =0; i<sizeof(detectors)/sizeof(const char*); i++)
    {
        file->GetObject(detectors[i],list);
        std::cout<<i<<std::endl;
        for (int j=0; j< sizeof(pdgs)/sizeof(int);j++)
        {   
            //std::cout << pdgs[j] << std::endl;
            hist = (TH3I*)list->FindObject((std::to_string(pdgs[j])).c_str());
            combinedHits->Add(hist);
            
        }
    }

    //Include TPC Detector
    TH3I* TPCHist;
    file->GetObject("TPC", TPCHist);
    combinedHits->Add(TPCHist);


    //Save to file
    TFile *f = new TFile("AllHits.root","RECREATE");
    combinedHits->Write("AllHits", TObject::kSingleKey);
    //f->Close();
    
}

