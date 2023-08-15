/* PLAN!
Input: List of PDGs of steps of particles we want 
-> If no file exists already 
    -> Create vector of histograms for each PDG, as well as for all particles. 

-> If file exists, read all the histograms as a vector (might just be able to read/write as the vector)
-> Recieve PDG number, step-coordinates, dump data into histogram file. Do this over all steps.

-> Save all updated histograms into the same file
*/

#ifndef TONYS_DEVELOPMENT_AREA_H_
#define TONYS_DEVELOPMENT_AREA_H_


#include <TCanvas.h>
#include <TFile.h>
#include <TH3I.h>
#include <TNtuple.h>
#include <TString.h>
#include <TTree.h>
#include <TObject.h>
#include <TList.h>
#include <filesystem>



namespace TonysDevelopmentArea{

bool file_exists(const std::string &filename);

TList* createhistlist(int (&pdgs)[8]);
void savehistlist(TList* list, std::string filepath);

void AddToHistogram(std::string histogramlistfilepath, std::vector<std::array<float,4>> data);

} //end namespace TonysDevelopmentArea

#endif 