#if !defined(__CLING__) || defined(__ROOTCLING__)
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "ITSMFTSimulation/Hit.h"
#include "TOFSimulation/Detector.h"
#include "EMCALBase/Hit.h"
#include "DataFormatsTRD/Hit.h"
#include "FT0Simulation/Detector.h" // for Fit Hit
#include "DataFormatsFV0/Hit.h"
#include "HMPIDBase/Hit.h"
#include "TPCSimulation/Point.h"
#include "PHOSBase/Hit.h"
#include "DataFormatsFDD/Hit.h"
#include "MCHSimulation/Hit.h"
#include "MIDSimulation/Hit.h"
#include "DataFormatsCPV/Hit.h"
#include "DataFormatsZDC/Hit.h"
#include "DetectorsCommonDataFormats/DetectorNameConf.h"
#include "DataFormatsParameters/GRPObject.h"
#endif

#include <typeinfo>

//Waa waa you can't statically link in the optimisation waa wa 
//#include "/home/answain/alice/AliceO2MacroDev/StepandHit_Analysis/Globals.h" //Get global variables for all macros

bool file_exists(const std::string &filename) {
  /*Check whether a file exists*/
  return std::filesystem::exists(filename);
}

int numDigits(int number)
{
  /*Returns the number of digits of an integer*/
    int digits = 1;

    while (number>=10) {
        number /= 10;
        digits++;
    }
    return digits;
    }

void savehistlist(TList* list, std::string filepath)
{ 

  /* 
  Saves a TList to a desired location (filepath)
  Creates new histlist if the .root file already exists and contains the same named TList. 
  */

  int i = 0; 
  while (file_exists(filepath+".root")){
    int digits = numDigits(i);
    std::cout << digits << std::endl;
    for (int j=0; j<digits; j++){
    filepath.pop_back();
    }
    i += 1; 
    filepath += std::to_string(i);
    

  }
  filepath += ".root";
  TFile *f = new TFile(filepath.c_str(),"RECREATE");
  list->Write("histlist", TObject::kSingleKey);
}

template <std::size_t W>
TList* createhistlist(int (&pdgs)[W])
{   
  //naming is so it doesn't clash with other macros with the same function names
//when running 'AllMacros.C' (probably better to use a namespace but ah well)

/* Creates and returns a TList of histograms for each particle that is wanted to be investigated
  Input is a int array of PDGid of particles to be investigated */



    //Number of bins in the histograms
  int numb_bins[3] = {100,100,100};

//Min values of the histograms
  float min_values[3] = {-1000,-1000,-3000};

//Max values of the histograms
  float max_values[3] = {1000,1000,3000};
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

TString gPrefix("");

template <typename Hit, typename Accumulator>
Accumulator analyse(TTree* tr, const char* brname)
{
  /*
  Analyses the hits. Custom addition gets all the hits for each detector as a TList in a root file
  i.e TList FT0 -> Histogram for each PDG put in the array in the global variables
  TList TRD -> Histogram for each PDG Put in the array in the global variables
  for all the detectors, all saved to the same root file.
  */

  Accumulator prop;
  auto br = tr->GetBranch(brname);
  if (!br) {
    return prop;
  }

  auto entries = br->GetEntries();
  
  std::vector<Hit>* hitvector = nullptr;
  br->SetAddress(&hitvector);
  
  //Open o2sim_Kine.root
  //Get the O2Sim tree -> MCTracks branch, get the trackID and find the PDGid from this. 
  TFile* o2sim_Kine = new TFile("o2sim_Kine.root");
  TTree *tree = (TTree*)o2sim_Kine->Get("o2sim");
  std::vector<o2::MCTrack>* MCTrack = nullptr;
  tree->SetBranchAddress("MCTrack",&MCTrack);
  int pdgs[8] = {11, 13, -11, -13, 22, 111, 211, -211};
  TList* particleHistograms = createhistlist(pdgs);

  //Iterate through all entries to the hit file adding the corresponding PDG to its own histogram
  std::cout << entries << std::endl;
  for (int i = 0; i < entries; ++i) {

    br->GetEntry(i);

    tree->GetEntry(i);

    for (auto& hit : *hitvector) {

      prop.addHit(hit);
      
      //Get PDG number of the particle 
      Int_t trackID = hit.GetTrackID();

      o2::MCTrack thisTrack = (*MCTrack).at(trackID);
      int PDGnumb = thisTrack.GetPdgCode();

      //Fill the corresponding histogram to the PDG number with the data 
      if (std::find(std::begin(pdgs), std::end(pdgs), PDGnumb) != std::end(pdgs))
        {
        TH3I* hist = (TH3I*)particleHistograms->FindObject((std::to_string(PDGnumb)).c_str());
        hist->Fill(hit.GetX(), hit.GetY(), hit.GetZ(), 1.0);
        
        }
    }
  }
  
  //Save the histograms
  TFile *f = new TFile("HitsInDetectorsHistograms.root","UPDATE");
  particleHistograms->Write(brname, TObject::kSingleKey);

  delete f;
  delete o2sim_Kine;

  prop.normalize();
 
  return prop;
};

struct HitStatsBase {
  int NHits = 0;
  double XAvg = 0.; // avg 1st moment
  double YAvg = 0.;
  double ZAvg = 0.;
  double X2Avg = 0.; // avg 2nd moment
  double Y2Avg = 0.;
  double Z2Avg = 0.;
  double EAvg = 0.; // average total energy
  double E2Avg = 0.;
  double TAvg = 0.;  // average T
  double T2Avg = 0.; // average T^2

  void print() const
  {
    std::cout << NHits << " "
              << XAvg << " "
              << YAvg << " "
              << ZAvg << " "
              << X2Avg << " "
              << Y2Avg << " "
              << Z2Avg << " "
              << EAvg << " "
              << E2Avg << " "
              << TAvg << " "
              << T2Avg << "\n";
  }

  void normalize()
  {
    XAvg /= NHits;
    YAvg /= NHits;
    ZAvg /= NHits;
    X2Avg /= NHits;
    Y2Avg /= NHits;
    Z2Avg /= NHits;
    EAvg /= NHits;
    E2Avg /= NHits;
    TAvg /= NHits;
    T2Avg /= NHits;
  }
};

template <typename T>
struct HitStats : public HitStatsBase {
  // adds a hit to the statistics
  void addHit(T const& hit)
  {
    NHits++;
    auto x = hit.GetX();
    XAvg += x;
    X2Avg += x * x;
    auto y = hit.GetY();
    YAvg += y;
    Y2Avg += y * y;
    auto z = hit.GetZ();
    ZAvg += z;
    Z2Avg += z * z;
    auto e = hit.GetEnergyLoss();
    EAvg += e;
    E2Avg += e * e;
    auto t = hit.GetTime();
    TAvg += t;
    T2Avg += t * t;
  }
};

/* DOESN'T WORK
struct TRDHitStats : public HitStatsBase {
  // adds a hit to the statistics
  void addHit(o2::trd::Hit const& hit)
  {
    NHits++;
    auto x = hit.GetX();
    XAvg += x;
    X2Avg += x * x;
    auto y = hit.GetY();
    YAvg += y;
    Y2Avg += y * y;
    auto z = hit.GetZ();
    ZAvg += z;
    Z2Avg += z * z;
    auto e = hit.GetCharge();
    EAvg += e;
    E2Avg += e * e;
    auto t = hit.GetTime();
    TAvg += t;
    T2Avg += t * t;
  }
};
*/
struct ITSHitStats : public HitStatsBase {
  // adds a hit to the statistics
  void addHit(o2::itsmft::Hit const& hit)
  {
    NHits++;
    auto x = hit.GetStartX();
    XAvg += x;
    X2Avg += x * x;
    auto y = hit.GetStartY();
    YAvg += y;
    Y2Avg += y * y;
    auto z = hit.GetStartZ();
    ZAvg += z;
    Z2Avg += z * z;
    auto e = hit.GetTotalEnergy();
    EAvg += e;
    E2Avg += e * e;
    auto t = hit.GetTime();
    TAvg += t;
    T2Avg += t * t;
  }
}; // end struct

struct TPCHitStats : public HitStatsBase {
  // adds a hit to the statistics
  void addHit(o2::tpc::HitGroup const& hitgroup)
  {
    for (int i = 0; i < hitgroup.getSize(); ++i) {
      auto hit = hitgroup.getHit(i);
      NHits++;
      auto x = hit.GetX();
      XAvg += x;
      X2Avg += x * x;
      auto y = hit.GetY();
      YAvg += y;
      Y2Avg += y * y;
      auto z = hit.GetZ();
      ZAvg += z;
      Z2Avg += z * z;
      auto e = hit.GetEnergyLoss();
      EAvg += e;
      E2Avg += e * e;
      auto t = hit.GetTime();
      TAvg += t;
      T2Avg += t * t;
    }
  }
}; // end struct

// need a special version for TPC since loop over sectors
TPCHitStats analyseTPC(TTree* tr)
{
  TPCHitStats prop;
  /*
  Analyses the hits. Custom addition gets all the hits for each detector as a TList in a root file
  i.e TList TPC -> Histogram for each PDG put in the array in the global variables
  This is the TPC part of the above as it is split into sectors. Same methodology as in analyse()
   */


  //Unsure how to get PDGid from this, so I don't... (Commented out)
  /*
  //Open o2sim_Kine.root
  //Get the O2Sim tree -> MCTracks branch, get the trackID and find the PDGid from this. 
  TFile* o2sim_Kine = new TFile("o2sim_Kine.root");
  TTree *tree = (TTree*)o2sim_Kine->Get("o2sim");
  std::vector<o2::MCTrack>* MCTrack = nullptr;
  tree->SetBranchAddress("MCTrack",&MCTrack);

  TList* particleHistograms = createhistlist(pdgs);
  Method for PDGid*/
  //Number of bins in the histograms
int numb_bins[3] = {100,100,100};

//Min values of the histograms
float min_values[3] = {-1000,-1000,-3000};

//Max values of the histograms
float max_values[3] = {1000,1000,3000};

  TH3I* hist = new TH3I("TPCHits","TPCHits",
  numb_bins[0],min_values[0],max_values[0],
  numb_bins[1],min_values[1],max_values[1],
  numb_bins[2],min_values[2],max_values[2]);

  string brname = "TPC";

  for (int sector = 0; sector < 35; ++sector) {
    std::stringstream brnamestr;
    brnamestr << "TPCHitsShiftedSector" << sector;
    auto br = tr->GetBranch(brnamestr.str().c_str());
    if (!br) {
      return prop;
    }
    auto entries = br->GetEntries();
    std::vector<o2::tpc::HitGroup>* hitvector = nullptr;
    br->SetAddress(&hitvector);

   

    for (int i = 0; i < entries; ++i) {
      br->GetEntry(i);

      /*
      tree->GetEntry(i);
      Method for PDGid*/

      for (auto& hit : *hitvector) {
        prop.addHit(hit);
        for (int i=0; i < hit.getSize(); ++i){

          auto ActualHit = hit.getHit(i);

          /*
          //Get PDG number of the particle 
          Int_t trackID = ActualHit.GetTrackID();

          o2::MCTrack thisTrack = (*MCTrack).at(trackID);
          int PDGnumb = thisTrack.GetPdgCode();
          

          //Write to corresponding histogram of 
          if (std::find(std::begin(pdgs), std::end(pdgs), PDGnumb) != std::end(pdgs))
            {
            TH3I* hist = (TH3I*)particleHistograms->FindObject((std::to_string(PDGnumb)).c_str());
            hist->Fill(ActualHit.GetX(), ActualHit.GetY(), ActualHit.GetZ(), 1.0);
            
            }Method for PDGid*/
          hist->Fill(ActualHit.GetX(), ActualHit.GetY(), ActualHit.GetZ(), 1.0);
          
        }
      }
    }
  }

  //Saves the 3D histogram
 
  TFile *f = new TFile("HitsInDetectorsHistograms.root","UPDATE");
  hist->Write(brname.c_str(), TObject::kSingleKey);
  delete f;
  
  /*
  delete o2sim_Kine;
  Method for PDGid*/

  prop.normalize();
  return prop;
};

// do comparison for ITS
void analyzeITS(TTree* reftree)
{
  if (!reftree)
    return;
  auto refresult = analyse<o2::itsmft::Hit, ITSHitStats>(reftree, "ITSHit");
  std::cout << gPrefix << " ITS ";
  refresult.print();
}

// do comparison for TOF
void analyzeTOF(TTree* reftree)
{
  if (!reftree)
    return;
  auto refresult = analyse<o2::tof::HitType, HitStats<o2::tof::HitType>>(reftree, "TOFHit");
  std::cout << gPrefix << " TOF ";
  refresult.print();
}

// do comparison for EMC
void analyzeEMC(TTree* reftree)
{
  if (!reftree)
    return;
  auto refresult = analyse<o2::emcal::Hit, HitStats<o2::emcal::Hit>>(reftree, "EMCHit");
  std::cout << gPrefix << " EMC ";
  refresult.print();
}

// do comparison for TRD
// need a different version of TRDHitStats to retrieve the hit value
/*
void analyzeTRD(TTree* reftree)
{
  if (!reftree)
    return;
  auto refresult = analyse<o2::trd::Hit, TRDHitStats>(reftree, "TRDHit");
  std::cout << gPrefix << " TRD ";
  refresult.print();
}
*/

// do comparison for PHS
void analyzePHS(TTree* reftree)
{
  if (!reftree)
    return;
  auto refresult = analyse<o2::phos::Hit, HitStats<o2::phos::Hit>>(reftree, "PHSHit");
  std::cout << gPrefix << " PHS ";
  refresult.print();
}

void analyzeFT0(TTree* reftree)
{
  std::cout <<"we get here!"<< std::endl;
  if (!reftree)
    return;
  auto refresult = analyse<o2::ft0::HitType, HitStats<o2::ft0::HitType>>(reftree, "FT0Hit");
  std::cout << gPrefix << " FT0 ";
  refresult.print();
}

void analyzeHMP(TTree* reftree)
{
  if (!reftree)
    return;
  auto refresult = analyse<o2::hmpid::HitType, HitStats<o2::hmpid::HitType>>(reftree, "HMPHit");
  std::cout << gPrefix << " HMP ";
  refresult.print();
}

void analyzeMFT(TTree* reftree)
{
  if (!reftree)
    return;
  auto refresult = analyse<o2::itsmft::Hit, ITSHitStats>(reftree, "MFTHit");
  std::cout << gPrefix << " MFT ";
  refresult.print();
}

void analyzeFDD(TTree* reftree)
{
  if (!reftree)
    return;
  auto refresult = analyse<o2::fdd::Hit, HitStats<o2::fdd::Hit>>(reftree, "FDDHit");
  std::cout << gPrefix << " FDD ";
  refresult.print();
}

void analyzeFV0(TTree* reftree)
{
  if (!reftree)
    return;
  auto refresult = analyse<o2::fv0::Hit, HitStats<o2::fv0::Hit>>(reftree, "FV0Hit");
  std::cout << gPrefix << " FV0 ";
  refresult.print();
}

void analyzeMCH(TTree* reftree)
{
  if (!reftree)
    return;
  auto refresult = analyse<o2::mch::Hit, HitStats<o2::mch::Hit>>(reftree, "MCHHit");
  std::cout << gPrefix << " MCH ";
  refresult.print();
}

void analyzeMID(TTree* reftree)
{
  if (!reftree)
    return;
  auto refresult = analyse<o2::mid::Hit, HitStats<o2::mid::Hit>>(reftree, "MIDHit");
  std::cout << gPrefix << " MID ";
  refresult.print();
}

void analyzeCPV(TTree* reftree)
{
  if (!reftree)
    return;
  auto refresult = analyse<o2::cpv::Hit, HitStats<o2::cpv::Hit>>(reftree, "CPVHit");
  std::cout << gPrefix << " CPV ";
  refresult.print();
}

void analyzeZDC(TTree* reftree)
{
  if (!reftree)
    return;
  auto refresult = analyse<o2::zdc::Hit, HitStats<o2::zdc::Hit>>(reftree, "ZDCHit");
  std::cout << gPrefix << " ZDC ";
  refresult.print();
}

void analyzeTPC(TTree* reftree)
{
  if (!reftree)
    return;
  // need to loop over sectors
  auto refresult = analyseTPC(reftree); //what?
  std::cout << gPrefix << " TPC ";
  refresult.print();
}

TTree* getHitTree(o2::parameters::GRPObject const* grp, const char* filebase, o2::detectors::DetID detid)
{
  if (!grp->isDetReadOut(detid)) {
    return nullptr;
  }

  std::string filename(o2::base::DetectorNameConf::getHitsFileName(detid, filebase).c_str());

  // shamefully leaking memory as the TTree cannot live without the file...
  TFile* file = new TFile(filename.c_str(), "OPEN");
  auto t = (TTree*)file->Get("o2sim");
  return t;
}

// Simple macro to get basic mean properties of simulated hits
// Used for instance to validate MC optimizations or to study
// the effect on the physics at the lowest level.
//
// A prefix (such as a parameter) can be given which will be  prepended before each line of printout.
// This could be useful for plotting.
//
void analyzeHitsForOptimisation(const char* filebase = "o2sim", const char* prefix = "")
{
  gPrefix = prefix;

  //Clear the existing file so that it doesn't get written on-top of over again and again giving false data:(
  TFile *f = new TFile("HitsInDetectorsHistograms.root","RECREATE");
  delete f;

  // READ GRP AND ITERATE OVER DETECTED PARTS
  auto grp = o2::parameters::GRPObject::loadFrom(filebase);

  // should correspond to the same number as defined in DetID
  analyzeITS(getHitTree(grp, filebase, o2::detectors::DetID::ITS));
  analyzeTPC(getHitTree(grp, filebase, o2::detectors::DetID::TPC)); ////////This dude plays by different rules.
  analyzeMFT(getHitTree(grp, filebase, o2::detectors::DetID::MFT));
  analyzeTOF(getHitTree(grp, filebase, o2::detectors::DetID::TOF));
  analyzeEMC(getHitTree(grp, filebase, o2::detectors::DetID::EMC));
  //analyzeTRD(getHitTree(grp, filebase, o2::detectors::DetID::TRD)); //Why doesn't this dude work:(
  analyzePHS(getHitTree(grp, filebase, o2::detectors::DetID::PHS));
  analyzeCPV(getHitTree(grp, filebase, o2::detectors::DetID::CPV));
  //analyzeFT0(getHitTree(grp, filebase, o2::detectors::DetID::FT0)); //Doesn't work either
  analyzeFV0(getHitTree(grp, filebase, o2::detectors::DetID::FV0));
  analyzeFDD(getHitTree(grp, filebase, o2::detectors::DetID::FDD));
  analyzeHMP(getHitTree(grp, filebase, o2::detectors::DetID::HMP));
  analyzeMCH(getHitTree(grp, filebase, o2::detectors::DetID::MCH));
  analyzeMID(getHitTree(grp, filebase, o2::detectors::DetID::MID));
  analyzeZDC(getHitTree(grp, filebase, o2::detectors::DetID::ZDC));
  // analyzeACO(getHitTree(grp, filebase, o2::detectors::DetID::ACO));
}
