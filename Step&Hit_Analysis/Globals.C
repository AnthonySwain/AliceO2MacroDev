/* For variables to be used across all the macro files for analysis*/

#include "Globals.h"

//Particles to be saved to histograms
int pdgs[8] = {11, 13, -11, -13, 22, 111, 211, -211};

//The detectors to be used in histogram analysis
const char* detectors[12] = {"ITSHit","MFTHit","TOFHit","EMCHit","PHSHit","CPVHit",/*"FT0Hit",*/
    "FV0Hit","FDDHit","HMPHit", "MCHHit", "MIDHit", "ZDCHit"};

//Number of bins in the histograms
int numb_bins[3] = {100,100,100};

//Min values of the histograms
float min_values[3] = {-1000,-1000,-3000};

//Max values of the histograms
float max_values[3] = {1000,1000,3000};