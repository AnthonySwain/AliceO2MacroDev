/* 
Runs all the macros for the step and hit analysis
The final result is a folder created of 2D & 3D projections of the steps and hits.
*/

#include "Globals.h"

void AllMacros(){
    gROOT->ProcessLine(".L Globals.C+");
    gROOT->ProcessLine(".x analyzeHits.C");
    gROOT->ProcessLine(".x analyzeSteps.C");
    gROOT->ProcessLine(".x CombineDetectorHits.C");
    gROOT->ProcessLine(".x CombineSteps.C");
    gROOT->ProcessLine(".x AllProjections.C");
}