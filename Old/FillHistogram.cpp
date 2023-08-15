
namespace TonysDevelopmentArea{

bool file_exists(const std::string &filename) {
  return std::filesystem::exists(filename);
}

TList* createhistlist(int (&pdgs)[8])
{   
    TList *list = new TList();
    
    for (int i : pdgs){
        std::string name1 = std::to_string(i);
        std::string name2 = "Histogram"+std::to_string(i);
        TH3I *h1 = new TH3I(name1.c_str(), name2.c_str(),100,-1000,1000,100,-1000,1000,100,-3000,3000);
        list -> Add(h1);
    }
   return (list);
}

void savehistlist(TList* list, std::string filepath)
{ 

  //Create new histlist if one already exists 
  int i = 0; 
  while (file_exists(filepath+".root")){

    filepath.pop_back();
    filepath += std::to_string(i);
    i += 1; 

  }
  filepath += ".root";
  TFile *f = new TFile(filepath.c_str(),"RECREATE");
  list->Write("histlist", TObject::kSingleKey);
}

TList* openhistlist(std::string filepath)
{
    TFile * file = new TFile(filepath.c_str(),"READ");
    TList* list;
    file->GetObject("histlist",list);
    return(list);
}


void AddToHistogram(std::string histogramlistfilepath, std::vector<std::array<float,4>> data)
{
/*  histogramlistfilepath - where to save or find the TList root file of the histograms 
    std::vector<std::array<float,4>>  data - {x,y,z,PDGid}
*/

int pdgs[8] = {11,13,-11,-13,22,111,211,-211}; //The particles we want to model.

/*
// Open root file (create if it doens't exist but output something in terminal incase the filename has been miss-typed)
//Given worker nodes, I am going to get rid of this for now and make a seperate root file for each worker node for ease of use

TList* list;
if (file_exists(histogramlistfilepath)){
    std::cout << "File exists, adding to existing histograms" << std::endl;
    TList* list = openhistlist(histogramlistfilepath);
    
}
//Else -> Initialise TList of histograms 
else 
  {
    std::cout << "File doesn't exist, creating new set of histograms" << std::endl;
    TList *list = createhistlist(pdgs);
  }
  */

TList *list = createhistlist(pdgs);
  
//Filling the histograms with data
for (auto &element : data)
{
auto pdg = element[3];
int pdgnumb = static_cast<int>(pdg);
//std::cout << pdg << ", PDG, " << pdgnumb << std::endl;

//If PDG number isn't in the pdg list we just continue 
if (std::find(std::begin(pdgs), std::end(pdgs), pdgnumb) != std::end(pdgs)){
  TH3I* hist = (TH3I*)list->FindObject((std::to_string(pdgnumb)).c_str());
  hist->Fill(element[0],element[1],element[2],1.0);
}

else{
  continue;
}

//std::cout << element[0] << ", " << element[1] << ", " <<element[2] << ", " << element[3] << ", " << std::endl;
}
savehistlist(list, histogramlistfilepath);
}




} //end namespace TonysDevelopmentArea