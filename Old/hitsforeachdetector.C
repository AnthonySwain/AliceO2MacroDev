//Stopped doing this, not really the most needed. //To be finished...
//Was going to show the number of hits by each particle in each detector 

#include <iostream>
#include <filesystem>

template <typename T, std::size_t W, std::size_t H>
void print_2d_array(T (&array)[W][H])
{
    for (auto &row : array)
    {
        for (auto &column : row)
        {
            std::cout << column << " ";
        }
        std::cout << std::endl;
    }
}

template <std::size_t W>
void create_bar_graph(const char* (&xnames)[W], int (&yvalues)[W], const char* name, const char* title){
    TCanvas *c1 = new TCanvas("c1");
    TH1F *h = new TH1F(name,title,3,0,3);
    h->SetStats(0);
    h->SetCanExtend(TH1::kAllAxes);
    c1->SetGrid();
    h->SetFillColor(38);
    for (int j=0 ; j < (sizeof(xnames)/sizeof(const char*)) ;j++){
        h->Fill(xnames[j],yvalues[j]);
    }
    h->LabelsDeflate();
    h->Draw("BAR");

    std::filesystem::create_directories("./DetectorHits");
   
    c1->Print(("./DetectorHits/" + std::string(title)+".pdf").c_str());


}


void hitsforeachdetector(){
    //Macro to create a 1D histogram / bar chart showing the number of hits per histogram 

    TFile *file = new TFile("HitsInDetectorsHistograms.root","READ");
    TList* list;
    

    const char* detectors[13] = {"ITSHit","MFTHit","TOFHit","EMCHit","PHSHit","CPVHit",
    "FV0Hit","FDDHit","HMPHit", "MCHHit", "MIDHit", "ZDCHit"};

    int pdgs[8] = {11,13,-11,-13,22,111,211,-211};
    int no_pdgs = sizeof(pdgs)/sizeof(int);
    const char* pdgnumbs[8] = {"11","13","-11","-13","22","111","211","-211"};

    int entries[13][8] = {0}; //initialise 2D array of zeroes    

    int no_detectors = sizeof(detectors)/sizeof(char*);
     //same number of entries as the number of detectors:) 
    for (int i=0;i<no_detectors;i++)
    {   
        file->GetObject(detectors[i],list);
        for (int j=0;j<no_pdgs;j++)
        {   
            TH3I* hist = (TH3I*)list->FindObject((std::to_string(pdgs[j])).c_str());
            int entry = hist->GetEntries();
            entries[i][j] = entry;
        }
    }
    print_2d_array(entries);

    //Different particles for each detector
    for (int i =0; i < sizeof(detectors)/sizeof(const char*);i++){
        create_bar_graph(pdgnumbs,entries[i],(detectors[i]),(detectors[i]));
    }

    //Different detectors for each particle

    //Total hits on each detector compared
    
}

