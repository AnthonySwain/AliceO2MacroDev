/* 
The stepping function in the simulation creates voxel_'numbers'.root files of the desired particles
This file combines all the histograms inside each file together whilst maintaining that the particles have their
own seperate histograms 
*/

#include <filesystem>
#include "Globals.h"

TList* openhistlist2(std::string filepath)
{
    /* 
    Opens the voxel_'numbers'.root file and returns the TList called histlist  
    */
    TFile * file = new TFile(filepath.c_str(),"READ");
    TList* list;
    file->GetObject("histlist",list);
    return(list);
}

int numDigits2(int number)
{
    /* Computes the number of digits in an integer */
    int digits = 1;

    while (number>=10) {
        number /= 10;
        digits++;
    }
    return digits;
    }

template <std::size_t W>
TList* createhistlist2(int (&pdgs)[W]){ 
//naming is so it doesn't clash with other macros with the same function names
//when running 'AllMacros.C' (probably better to use a namespace but ah well)

/* Creates and returns a TList of histograms for each particle that is wanted to be investigated
  Input is a int array of PDGid of particles to be investigated */
  
    TList *list = new TList();
    
    for (int i : pdgs){
        std::string name1 = std::to_string(i);
        std::string name2 = "Histogram"+std::to_string(i);
        TH3I *h1 = new TH3I(name1.c_str(), name2.c_str(),
        numb_bins[0],min_values[0],max_values[0],
        numb_bins[1],min_values[1],max_values[1],
        numb_bins[2],min_values[2],max_values[2]);
        list -> Add(h1);
    }
   return (list);
}


void savehistlist2(TList* list, std::string filepath)
{ 
  /* 
  Saves a TList to a desired location (filepath)
  Creates new histlist if the .root file already exists and contains the same named TList. 
  */

  filepath += ".root";
  TFile *f = new TFile(filepath.c_str(),"RECREATE");
  list->Write("histlist", TObject::kSingleKey);
  delete f;
}

bool file_exists3(const std::string &filename) {
  /*Checks whether a file exists*/
  return std::filesystem::exists(filename);
}


void analyzeSteps(){
    /* Main function that is called for the macro */

    //Creates a TList to store the histograms
    TList* empty = createhistlist2(pdgs);

    //Finds all the voxels_'number'.root files and gets the histograms together
    //I know the pid has at most 7 digits so just go through them all (there's a better way for sure but this works)
    for (int i =0; i < 10000000; i++){
        if (file_exists3("voxel_"+std::to_string(i)+".root")){
            
            //std::cout<<j<<std::endl;
            TFile *f = new TFile(("voxel_"+std::to_string(i)+".root").c_str(),"READ");
            for (int pdg : pdgs)
            {      
                std::cout<<("voxel_"+std::to_string(i)+".root").c_str()<<std::endl;
                 ((TH3I*)empty->FindObject((std::to_string(pdg)).c_str()))->Add(((TH3I*)f->Get((std::to_string(pdg)).c_str())));
         
            }
            f->Close();
        }
        else{continue;}
}
//Save the TList to file 
TFile *f = new TFile("AllSteps.root","RECREATE");
empty->Write("Allsteps",TObject::kSingleKey);
f->Close();
}