#include <algorithm>
#include <iostream>
#include <filesystem>
#include <math.h>  
#include "Globals.h" //Get global variables for all macros


std::vector<std::array<float,3>> FindMaxRadiusInSlices(TH3F* hist3D, int numSlices){
    //The array to be returned
    std::vector<std::array<float,3>> cylinder_info;
    std::array<float,3> slice_info;
    // Check if the histogram is valid
    if (!hist3D) {
        std::cerr << "Invalid histogram!" << std::endl;
        return;
    }

    int xBins = hist3D->GetXaxis()->GetNbins();
    int yBins = hist3D->GetYaxis()->GetNbins();
    int zBins = hist3D->GetZaxis()->GetNbins();


    numSlices = zBins; //just doing this now, for simplicity. 
    if (numSlices > zBins) {
        std::cerr << "Number of slices exceeds the number of bins in the Z-axis!" << std::endl;
        return;
    }

    // Loop over the specified number of slices
    for (int slice = 1; slice <= numSlices; ++slice) {
        double maxRadius = -1.0;

        

        // Calculate the Z range for this slice
        double zLow = hist3D->GetZaxis()->GetBinLowEdge(slice);
        double zHigh = hist3D->GetZaxis()->GetBinUpEdge(slice);

        // Loop over the bins in X and Y
        for (int xBin = 1; xBin <= xBins; ++xBin) {
            for (int yBin = 1; yBin <= yBins; ++yBin) {
                if (hist3D->GetBinContent(xBin, yBin, zBin) > 0) {

                    double x = hist3D->GetXaxis()->GetBinCenter(xBin);
                    double y = hist3D->GetYaxis()->GetBinCenter(yBin);
                    double z = (zLow + zHigh) / 2.0; // Use the middle of the slice

                    // Calculate the radius squared
                    double radiusSquared = x * x + y * y;

                    // Check if this point has a greater radius
                    if (radiusSquared > maxRadius) {
                        maxRadius = radiusSquared;
                    }
                }
            }        
        }

        slice_info = {zLow, zHigh, maxRadius};
        cylinder_info.push_back(slice_info);

    }

    return cylinder_info;
}


void WriteToCSV(std::vector<std::array<float,3>> Cylinder_Data){

     // Open the CSV file for writing
    std::ofstream csv_file("output.csv");

    // Check if the file is open
    if (!csv_file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 1; // Return an error code
    }

    // Write headers to the CSV file
    csv_file << "Zmin,Zmax,radius,To_check" << std::endl;

    // Write data to the CSV file
    for (const auto& slice_info : cylinder_info) {
        csv_file << slice_info[0] << "," << slice_info[1] << "," << slice_info[2] << "," << "All" << std::endl;
    }

    // Close the file
    csv_file.close();
}





void FindOptimalCylindersFromHits(){
    //Opening HitsHistogram
    string filepathHits = "AllHits.root";
    TFile* fileHits = new TFile(filepathHits.c_str(),"READ");
    TH3I* histHits = nullptr;
    fileHits->GetObject("AllHits",histHits);

    std::vector<std::array<float,3>> Cylinder_Data = FindMaximumRadiusInSlices(histHits, 100);
    WriteToCSV(Cylinder_Data);
}