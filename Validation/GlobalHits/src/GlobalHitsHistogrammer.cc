/** \file GlobalHitsHistogrammer.cc
 *
 *  See header file for description of class
 *
 *  \author M. Strang SUNY-Buffalo
 */

#include "Validation/GlobalHits/interface/GlobalHitsHistogrammer.h"
// #include "DQMServices/Core/interface/DQMStore.h"

GlobalHitsHistogrammer::GlobalHitsHistogrammer(const edm::ParameterSet &iPSet)
    : fName(""), verbosity(0), frequency(0), vtxunit(0), label(""),
      getAllProvenances(false), printProvenanceInfo(false), count(0) {
  std::string MsgLoggerCat = "GlobalHitsHistogrammer_GlobalHitsHistogrammer";

  // get information from parameter set
  fName = iPSet.getUntrackedParameter<std::string>("Name");
  verbosity = iPSet.getUntrackedParameter<int>("Verbosity");
  frequency = iPSet.getUntrackedParameter<int>("Frequency");
  vtxunit = iPSet.getUntrackedParameter<int>("VtxUnit");
  outputfile = iPSet.getParameter<std::string>("OutputFile");
  doOutput = iPSet.getParameter<bool>("DoOutput");
  edm::ParameterSet m_Prov =
      iPSet.getParameter<edm::ParameterSet>("ProvenanceLookup");
  getAllProvenances = m_Prov.getUntrackedParameter<bool>("GetAllProvenances");
  printProvenanceInfo =
      m_Prov.getUntrackedParameter<bool>("PrintProvenanceInfo");

  // get Labels to use to extract information
  GlobalHitSrc_ = iPSet.getParameter<edm::InputTag>("GlobalHitSrc");
  // fix for consumes
  GlobalHitSrc_Token_ = consumes<PGlobalSimHit>(
      iPSet.getParameter<edm::InputTag>("GlobalHitSrc"));

  // use value of first digit to determine default output level (inclusive)
  // 0 is none, 1 is basic, 2 is fill output, 3 is gather output
  verbosity %= 10;

  // print out Parameter Set information being used
  if (verbosity >= 0) {
    edm::LogInfo(MsgLoggerCat)
        << "\n===============================\n"
        << "Initialized as EDAnalyzer with parameter values:\n"
        << "    Name          = " << fName << "\n"
        << "    Verbosity     = " << verbosity << "\n"
        << "    Frequency     = " << frequency << "\n"
        << "    VtxUnit       = " << vtxunit << "\n"
        << "    OutputFile    = " << outputfile << "\n"
        << "    DoOutput      = " << doOutput << "\n"
        << "    GetProv       = " << getAllProvenances << "\n"
        << "    PrintProv     = " << printProvenanceInfo << "\n"
        << "    GlobalHitSrc  = " << GlobalHitSrc_.label() << ":"
        << GlobalHitSrc_.instance() << "\n"
        << "===============================\n";
  }
}

void GlobalHitsHistogrammer::bookHistograms(DQMStore::IBooker &ibooker,
                                            edm::Run const &,
                                            edm::EventSetup const &) {

  // initialize monitor elements
  for (Int_t i = 0; i < 2; ++i) {
    meMCRGP[i] = nullptr;
    meMCG4Vtx[i] = nullptr;
    meGeantVtxX[i] = nullptr;
    meGeantVtxY[i] = nullptr;
    meGeantVtxZ[i] = nullptr;
    meMCG4Trk[i] = nullptr;
    meCaloEcal[i] = nullptr;
    meCaloEcalE[i] = nullptr;
    meCaloEcalToF[i] = nullptr;
    meCaloPreSh[i] = nullptr;
    meCaloPreShE[i] = nullptr;
    meCaloPreShToF[i] = nullptr;
    meCaloHcal[i] = nullptr;
    meCaloHcalE[i] = nullptr;
    meCaloHcalToF[i] = nullptr;
    meTrackerPx[i] = nullptr;
    meTrackerSi[i] = nullptr;
    meMuon[i] = nullptr;
    meMuonDtToF[i] = nullptr;
    meMuonCscToF[i] = nullptr;
    meMuonRpcFToF[i] = nullptr;
    meMuonRpcBToF[i] = nullptr;
  }
  meGeantTrkPt = nullptr;
  meGeantTrkE = nullptr;
  meCaloEcalPhi = nullptr;
  meCaloEcalEta = nullptr;
  meCaloPreShPhi = nullptr;
  meCaloPreShEta = nullptr;
  meCaloHcalPhi = nullptr;
  meCaloHcalEta = nullptr;
  meTrackerPxPhi = nullptr;
  meTrackerPxEta = nullptr;
  meTrackerPxBToF = nullptr;
  meTrackerPxBR = nullptr;
  meTrackerPxFToF = nullptr;
  meTrackerPxFZ = nullptr;
  meTrackerSiPhi = nullptr;
  meTrackerSiEta = nullptr;
  meTrackerSiBToF = nullptr;
  meTrackerSiBR = nullptr;
  meTrackerSiFToF = nullptr;
  meTrackerSiFZ = nullptr;
  meMuonPhi = nullptr;
  meMuonEta = nullptr;
  meMuonDtR = nullptr;
  meMuonCscZ = nullptr;
  meMuonRpcBR = nullptr;
  meMuonRpcFZ = nullptr;

  // create histograms
  Char_t hname[200];
  Char_t htitle[200];

  // MCGeant
  ibooker.setCurrentFolder("GlobalHitsV/MCGeant");
  sprintf(hname, "hMCRGP1");
  sprintf(htitle, "RawGenParticles");
  meMCRGP[0] = ibooker.book1D(hname, htitle, 100, 0., 5000.);
  sprintf(hname, "hMCRGP2");
  meMCRGP[1] = ibooker.book1D(hname, htitle, 100, 0., 500.);
  for (Int_t i = 0; i < 2; ++i) {
    meMCRGP[i]->setAxisTitle("Number of Raw Generated Particles", 1);
    meMCRGP[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hMCG4Vtx1");
  sprintf(htitle, "G4 Vertices");
  meMCG4Vtx[0] = ibooker.book1D(hname, htitle, 100, 0., 50000.);
  sprintf(hname, "hMCG4Vtx2");
  meMCG4Vtx[1] = ibooker.book1D(hname, htitle, 100, -0.5, 99.5);
  for (Int_t i = 0; i < 2; ++i) {
    meMCG4Vtx[i]->setAxisTitle("Number of Vertices", 1);
    meMCG4Vtx[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hMCG4Trk1");
  sprintf(htitle, "G4 Tracks");
  meMCG4Trk[0] = ibooker.book1D(hname, htitle, 150, 0., 15000.);
  sprintf(hname, "hMCG4Trk2");
  meMCG4Trk[1] = ibooker.book1D(hname, htitle, 150, -0.5, 99.5);
  for (Int_t i = 0; i < 2; ++i) {
    meMCG4Trk[i]->setAxisTitle("Number of Tracks", 1);
    meMCG4Trk[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hGeantVtxX1");
  sprintf(htitle, "Geant vertex x/micrometer");
  meGeantVtxX[0] = ibooker.book1D(hname, htitle, 100, -8000000., 8000000.);
  sprintf(hname, "hGeantVtxX2");
  meGeantVtxX[1] = ibooker.book1D(hname, htitle, 100, -50., 50.);
  for (Int_t i = 0; i < 2; ++i) {
    meGeantVtxX[i]->setAxisTitle("x of Vertex (um)", 1);
    meGeantVtxX[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hGeantVtxY1");
  sprintf(htitle, "Geant vertex y/micrometer");
  meGeantVtxY[0] = ibooker.book1D(hname, htitle, 100, -8000000, 8000000.);
  sprintf(hname, "hGeantVtxY2");
  meGeantVtxY[1] = ibooker.book1D(hname, htitle, 100, -50., 50.);
  for (Int_t i = 0; i < 2; ++i) {
    meGeantVtxY[i]->setAxisTitle("y of Vertex (um)", 1);
    meGeantVtxY[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hGeantVtxZ1");
  sprintf(htitle, "Geant vertex z/millimeter");
  meGeantVtxZ[0] = ibooker.book1D(hname, htitle, 100, -11000., 11000.);
  sprintf(hname, "hGeantVtxZ2");
  meGeantVtxZ[1] = ibooker.book1D(hname, htitle, 100, -250., 250.);
  for (Int_t i = 0; i < 2; ++i) {
    meGeantVtxZ[i]->setAxisTitle("z of Vertex (mm)", 1);
    meGeantVtxZ[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hGeantTrkPt");
  sprintf(htitle, "Geant track pt/GeV");
  meGeantTrkPt = ibooker.book1D(hname, htitle, 100, 0., 200.);
  meGeantTrkPt->setAxisTitle("pT of Track (GeV)", 1);
  meGeantTrkPt->setAxisTitle("Count", 2);

  sprintf(hname, "hGeantTrkE");
  sprintf(htitle, "Geant track E/GeV");
  meGeantTrkE = ibooker.book1D(hname, htitle, 100, 0., 5000.);
  meGeantTrkE->setAxisTitle("E of Track (GeV)", 1);
  meGeantTrkE->setAxisTitle("Count", 2);

  // ECal
  ibooker.setCurrentFolder("GlobalHitsV/ECals");

  sprintf(hname, "hCaloEcal1");
  sprintf(htitle, "Ecal hits");
  meCaloEcal[0] = ibooker.book1D(hname, htitle, 100, 0., 10000.);
  sprintf(hname, "hCaloEcal2");
  meCaloEcal[1] = ibooker.book1D(hname, htitle, 100, -0.5, 99.5);

  sprintf(hname, "hCaloEcalE1");
  sprintf(htitle, "Ecal hits, energy/GeV");
  meCaloEcalE[0] = ibooker.book1D(hname, htitle, 100, 0., 10.);
  sprintf(hname, "hCaloEcalE2");
  meCaloEcalE[1] = ibooker.book1D(hname, htitle, 100, 0., 0.1);

  sprintf(hname, "hCaloEcalToF1");
  sprintf(htitle, "Ecal hits, ToF/ns");
  meCaloEcalToF[0] = ibooker.book1D(hname, htitle, 100, 0., 1000.);
  sprintf(hname, "hCaloEcalToF2");
  meCaloEcalToF[1] = ibooker.book1D(hname, htitle, 100, 0., 100.);

  for (Int_t i = 0; i < 2; ++i) {
    meCaloEcal[i]->setAxisTitle("Number of Hits", 1);
    meCaloEcal[i]->setAxisTitle("Count", 2);
    meCaloEcalE[i]->setAxisTitle("Energy of Hits (GeV)", 1);
    meCaloEcalE[i]->setAxisTitle("Count", 2);
    meCaloEcalToF[i]->setAxisTitle("Time of Flight of Hits (ns)", 1);
    meCaloEcalToF[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hCaloEcalPhi");
  sprintf(htitle, "Ecal hits, phi/rad");
  meCaloEcalPhi = ibooker.book1D(hname, htitle, 100, -3.2, 3.2);
  meCaloEcalPhi->setAxisTitle("Phi of Hits (rad)", 1);
  meCaloEcalPhi->setAxisTitle("Count", 2);

  sprintf(hname, "hCaloEcalEta");
  sprintf(htitle, "Ecal hits, eta");
  meCaloEcalEta = ibooker.book1D(hname, htitle, 100, -5.5, 5.5);
  meCaloEcalEta->setAxisTitle("Eta of Hits", 1);
  meCaloEcalEta->setAxisTitle("Count", 2);

  sprintf(hname, "hCaloPreSh1");
  sprintf(htitle, "PreSh hits");
  meCaloPreSh[0] = ibooker.book1D(hname, htitle, 100, 0., 10000.);
  sprintf(hname, "hCaloPreSh2");
  meCaloPreSh[1] = ibooker.book1D(hname, htitle, 100, -0.5, 99.5);

  sprintf(hname, "hCaloPreShE1");
  sprintf(htitle, "PreSh hits, energy/GeV");
  meCaloPreShE[0] = ibooker.book1D(hname, htitle, 100, 0., 10.);
  sprintf(hname, "hCaloPreShE2");
  meCaloPreShE[1] = ibooker.book1D(hname, htitle, 100, 0., 0.1);

  sprintf(hname, "hCaloPreShToF1");
  sprintf(htitle, "PreSh hits, ToF/ns");
  meCaloPreShToF[0] = ibooker.book1D(hname, htitle, 100, 0., 1000.);
  sprintf(hname, "hCaloPreShToF2");
  meCaloPreShToF[1] = ibooker.book1D(hname, htitle, 100, 0., 100.);

  for (Int_t i = 0; i < 2; ++i) {
    meCaloPreSh[i]->setAxisTitle("Number of Hits", 1);
    meCaloPreSh[i]->setAxisTitle("Count", 2);
    meCaloPreShE[i]->setAxisTitle("Energy of Hits (GeV)", 1);
    meCaloPreShE[i]->setAxisTitle("Count", 2);
    meCaloPreShToF[i]->setAxisTitle("Time of Flight of Hits (ns)", 1);
    meCaloPreShToF[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hCaloPreShPhi");
  sprintf(htitle, "PreSh hits, phi/rad");
  meCaloPreShPhi = ibooker.book1D(hname, htitle, 100, -3.2, 3.2);
  meCaloPreShPhi->setAxisTitle("Phi of Hits (rad)", 1);
  meCaloPreShPhi->setAxisTitle("Count", 2);

  sprintf(hname, "hCaloPreShEta");
  sprintf(htitle, "PreSh hits, eta");
  meCaloPreShEta = ibooker.book1D(hname, htitle, 100, -5.5, 5.5);
  meCaloPreShEta->setAxisTitle("Eta of Hits", 1);
  meCaloPreShEta->setAxisTitle("Count", 2);

  // Hcal
  ibooker.setCurrentFolder("GlobalHitsV/HCals");
  sprintf(hname, "hCaloHcal1");
  sprintf(htitle, "Hcal hits");
  meCaloHcal[0] = ibooker.book1D(hname, htitle, 100, 0., 10000.);
  sprintf(hname, "hCaloHcal2");
  meCaloHcal[1] = ibooker.book1D(hname, htitle, 100, -0.5, 99.5);

  sprintf(hname, "hCaloHcalE1");
  sprintf(htitle, "Hcal hits, energy/GeV");
  meCaloHcalE[0] = ibooker.book1D(hname, htitle, 100, 0., 10.);
  sprintf(hname, "hCaloHcalE2");
  meCaloHcalE[1] = ibooker.book1D(hname, htitle, 100, 0., 0.1);

  sprintf(hname, "hCaloHcalToF1");
  sprintf(htitle, "Hcal hits, ToF/ns");
  meCaloHcalToF[0] = ibooker.book1D(hname, htitle, 100, 0., 1000.);
  sprintf(hname, "hCaloHcalToF2");
  meCaloHcalToF[1] = ibooker.book1D(hname, htitle, 100, 0., 100.);

  for (Int_t i = 0; i < 2; ++i) {
    meCaloHcal[i]->setAxisTitle("Number of Hits", 1);
    meCaloHcal[i]->setAxisTitle("Count", 2);
    meCaloHcalE[i]->setAxisTitle("Energy of Hits (GeV)", 1);
    meCaloHcalE[i]->setAxisTitle("Count", 2);
    meCaloHcalToF[i]->setAxisTitle("Time of Flight of Hits (ns)", 1);
    meCaloHcalToF[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hCaloHcalPhi");
  sprintf(htitle, "Hcal hits, phi/rad");
  meCaloHcalPhi = ibooker.book1D(hname, htitle, 100, -3.2, 3.2);
  meCaloHcalPhi->setAxisTitle("Phi of Hits (rad)", 1);
  meCaloHcalPhi->setAxisTitle("Count", 2);

  sprintf(hname, "hCaloHcalEta");
  sprintf(htitle, "Hcal hits, eta");
  meCaloHcalEta = ibooker.book1D(hname, htitle, 100, -5.5, 5.5);
  meCaloHcalEta->setAxisTitle("Eta of Hits", 1);
  meCaloHcalEta->setAxisTitle("Count", 2);

  // SiPixels
  ibooker.setCurrentFolder("GlobalHitsV/SiPixels");
  sprintf(hname, "hTrackerPx1");
  sprintf(htitle, "Pixel hits");
  meTrackerPx[0] = ibooker.book1D(hname, htitle, 100, 0., 10000.);
  sprintf(hname, "hTrackerPx2");
  meTrackerPx[1] = ibooker.book1D(hname, htitle, 100, -0.5, 99.5);
  for (Int_t i = 0; i < 2; ++i) {
    meTrackerPx[i]->setAxisTitle("Number of Pixel Hits", 1);
    meTrackerPx[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hTrackerPxPhi");
  sprintf(htitle, "Pixel hits phi/rad");
  meTrackerPxPhi = ibooker.book1D(hname, htitle, 100, -3.2, 3.2);
  meTrackerPxPhi->setAxisTitle("Phi of Hits (rad)", 1);
  meTrackerPxPhi->setAxisTitle("Count", 2);

  sprintf(hname, "hTrackerPxEta");
  sprintf(htitle, "Pixel hits eta");
  meTrackerPxEta = ibooker.book1D(hname, htitle, 100, -3.5, 3.5);
  meTrackerPxEta->setAxisTitle("Eta of Hits", 1);
  meTrackerPxEta->setAxisTitle("Count", 2);

  sprintf(hname, "hTrackerPxBToF");
  sprintf(htitle, "Pixel barrel hits, ToF/ns");
  meTrackerPxBToF = ibooker.book1D(hname, htitle, 100, 0., 40.);
  meTrackerPxBToF->setAxisTitle("Time of Flight of Hits (ns)", 1);
  meTrackerPxBToF->setAxisTitle("Count", 2);

  sprintf(hname, "hTrackerPxBR");
  sprintf(htitle, "Pixel barrel hits, R/cm");
  meTrackerPxBR = ibooker.book1D(hname, htitle, 100, 0., 50.);
  meTrackerPxBR->setAxisTitle("R of Hits (cm)", 1);
  meTrackerPxBR->setAxisTitle("Count", 2);

  sprintf(hname, "hTrackerPxFToF");
  sprintf(htitle, "Pixel forward hits, ToF/ns");
  meTrackerPxFToF = ibooker.book1D(hname, htitle, 100, 0., 50.);
  meTrackerPxFToF->setAxisTitle("Time of Flight of Hits (ns)", 1);
  meTrackerPxFToF->setAxisTitle("Count", 2);

  sprintf(hname, "hTrackerPxFZ");
  sprintf(htitle, "Pixel forward hits, Z/cm");
  meTrackerPxFZ = ibooker.book1D(hname, htitle, 200, -100., 100.);
  meTrackerPxFZ->setAxisTitle("Z of Hits (cm)", 1);
  meTrackerPxFZ->setAxisTitle("Count", 2);

  // SiStrips
  ibooker.setCurrentFolder("GlobalHitsV/SiPixels");
  sprintf(hname, "hTrackerSi1");
  sprintf(htitle, "Silicon hits");
  meTrackerSi[0] = ibooker.book1D(hname, htitle, 100, 0., 10000.);
  sprintf(hname, "hTrackerSi2");
  meTrackerSi[1] = ibooker.book1D(hname, htitle, 100, -0.5, 99.5);
  for (Int_t i = 0; i < 2; ++i) {
    meTrackerSi[i]->setAxisTitle("Number of Silicon Hits", 1);
    meTrackerSi[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hTrackerSiPhi");
  sprintf(htitle, "Silicon hits phi/rad");
  meTrackerSiPhi = ibooker.book1D(hname, htitle, 100, -3.2, 3.2);
  meTrackerSiPhi->setAxisTitle("Phi of Hits (rad)", 1);
  meTrackerSiPhi->setAxisTitle("Count", 2);

  sprintf(hname, "hTrackerSiEta");
  sprintf(htitle, "Silicon hits eta");
  meTrackerSiEta = ibooker.book1D(hname, htitle, 100, -3.5, 3.5);
  meTrackerSiEta->setAxisTitle("Eta of Hits", 1);
  meTrackerSiEta->setAxisTitle("Count", 2);

  sprintf(hname, "hTrackerSiBToF");
  sprintf(htitle, "Silicon barrel hits, ToF/ns");
  meTrackerSiBToF = ibooker.book1D(hname, htitle, 100, 0., 50.);
  meTrackerSiBToF->setAxisTitle("Time of Flight of Hits (ns)", 1);
  meTrackerSiBToF->setAxisTitle("Count", 2);

  sprintf(hname, "hTrackerSiBR");
  sprintf(htitle, "Silicon barrel hits, R/cm");
  meTrackerSiBR = ibooker.book1D(hname, htitle, 100, 0., 200.);
  meTrackerSiBR->setAxisTitle("R of Hits (cm)", 1);
  meTrackerSiBR->setAxisTitle("Count", 2);

  sprintf(hname, "hTrackerSiFToF");
  sprintf(htitle, "Silicon forward hits, ToF/ns");
  meTrackerSiFToF = ibooker.book1D(hname, htitle, 100, 0., 75.);
  meTrackerSiFToF->setAxisTitle("Time of Flight of Hits (ns)", 1);
  meTrackerSiFToF->setAxisTitle("Count", 2);

  sprintf(hname, "hTrackerSiFZ");
  sprintf(htitle, "Silicon forward hits, Z/cm");
  meTrackerSiFZ = ibooker.book1D(hname, htitle, 200, -300., 300.);
  meTrackerSiFZ->setAxisTitle("Z of Hits (cm)", 1);
  meTrackerSiFZ->setAxisTitle("Count", 2);

  // muon
  ibooker.setCurrentFolder("GlobalHitsV/Muons");
  sprintf(hname, "hMuon1");
  sprintf(htitle, "Muon hits");
  meMuon[0] = ibooker.book1D(hname, htitle, 100, 0., 10000.);
  sprintf(hname, "hMuon2");
  meMuon[1] = ibooker.book1D(hname, htitle, 100, -0.5, 99.5);
  for (Int_t i = 0; i < 2; ++i) {
    meMuon[i]->setAxisTitle("Number of Muon Hits", 1);
    meMuon[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hMuonPhi");
  sprintf(htitle, "Muon hits phi/rad");
  meMuonPhi = ibooker.book1D(hname, htitle, 100, -3.2, 3.2);
  meMuonPhi->setAxisTitle("Phi of Hits (rad)", 1);
  meMuonPhi->setAxisTitle("Count", 2);

  sprintf(hname, "hMuonEta");
  sprintf(htitle, "Muon hits eta");
  meMuonEta = ibooker.book1D(hname, htitle, 100, -3.5, 3.5);
  meMuonEta->setAxisTitle("Eta of Hits", 1);
  meMuonEta->setAxisTitle("Count", 2);

  sprintf(hname, "hMuonCscToF1");
  sprintf(htitle, "Muon CSC hits, ToF/ns");
  meMuonCscToF[0] = ibooker.book1D(hname, htitle, 100, 0., 250.);
  sprintf(hname, "hMuonCscToF2");
  meMuonCscToF[1] = ibooker.book1D(hname, htitle, 100, 0., 50.);
  for (Int_t i = 0; i < 2; ++i) {
    meMuonCscToF[i]->setAxisTitle("Time of Flight of Hits (ns)", 1);
    meMuonCscToF[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hMuonCscZ");
  sprintf(htitle, "Muon CSC hits, Z/cm");
  meMuonCscZ = ibooker.book1D(hname, htitle, 200, -1500., 1500.);
  meMuonCscZ->setAxisTitle("Z of Hits (cm)", 1);
  meMuonCscZ->setAxisTitle("Count", 2);

  sprintf(hname, "hMuonDtToF1");
  sprintf(htitle, "Muon DT hits, ToF/ns");
  meMuonDtToF[0] = ibooker.book1D(hname, htitle, 100, 0., 250.);
  sprintf(hname, "hMuonDtToF2");
  meMuonDtToF[1] = ibooker.book1D(hname, htitle, 100, 0., 50.);
  for (Int_t i = 0; i < 2; ++i) {
    meMuonDtToF[i]->setAxisTitle("Time of Flight of Hits (ns)", 1);
    meMuonDtToF[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hMuonDtR");
  sprintf(htitle, "Muon DT hits, R/cm");
  meMuonDtR = ibooker.book1D(hname, htitle, 100, 0., 1500.);
  meMuonDtR->setAxisTitle("R of Hits (cm)", 1);
  meMuonDtR->setAxisTitle("Count", 2);

  sprintf(hname, "hMuonRpcFToF1");
  sprintf(htitle, "Muon RPC forward hits, ToF/ns");
  meMuonRpcFToF[0] = ibooker.book1D(hname, htitle, 100, 0., 250.);
  sprintf(hname, "hMuonRpcFToF2_4305");
  meMuonRpcFToF[1] = ibooker.book1D(hname, htitle, 100, 0., 50.);
  for (Int_t i = 0; i < 2; ++i) {
    meMuonRpcFToF[i]->setAxisTitle("Time of Flight of Hits (ns)", 1);
    meMuonRpcFToF[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hMuonRpcFZ");
  sprintf(htitle, "Muon RPC forward hits, Z/cm");
  meMuonRpcFZ = ibooker.book1D(hname, htitle, 201, -1500., 1500.);
  meMuonRpcFZ->setAxisTitle("Z of Hits (cm)", 1);
  meMuonRpcFZ->setAxisTitle("Count", 2);

  sprintf(hname, "hMuonRpcBToF1");
  sprintf(htitle, "Muon RPC barrel hits, ToF/ns");
  meMuonRpcBToF[0] = ibooker.book1D(hname, htitle, 100, 0., 250.);
  sprintf(hname, "hMuonRpcBToF2");
  meMuonRpcBToF[1] = ibooker.book1D(hname, htitle, 100, 0., 50.);
  for (Int_t i = 0; i < 2; ++i) {
    meMuonRpcBToF[i]->setAxisTitle("Time of Flight of Hits (ns)", 1);
    meMuonRpcBToF[i]->setAxisTitle("Count", 2);
  }

  sprintf(hname, "hMuonRpcBR");
  sprintf(htitle, "Muon RPC barrel hits, R/cm");
  meMuonRpcBR = ibooker.book1D(hname, htitle, 100, 0., 1500.);
  meMuonRpcBR->setAxisTitle("R of Hits (cm)", 1);
  meMuonRpcBR->setAxisTitle("Count", 2);
}

GlobalHitsHistogrammer::~GlobalHitsHistogrammer() {}

void GlobalHitsHistogrammer::analyze(const edm::Event &iEvent,
                                     const edm::EventSetup &iSetup) {
  std::string MsgLoggerCat = "GlobalHitsHistogrammer_analyze";

  // keep track of number of events processed
  ++count;

  // get event id information
  edm::RunNumber_t nrun = iEvent.id().run();
  edm::EventNumber_t nevt = iEvent.id().event();

  if (verbosity > 0) {
    edm::LogInfo(MsgLoggerCat) << "Processing run " << nrun << ", event "
                               << nevt << " (" << count << " events total)";
  } else if (verbosity == 0) {
    if (nevt % frequency == 0 || nevt == 1) {
      edm::LogInfo(MsgLoggerCat) << "Processing run " << nrun << ", event "
                                 << nevt << " (" << count << " events total)";
    }
  }

  // look at information available in the event
  if (getAllProvenances) {

    std::vector<const edm::StableProvenance *> AllProv;
    iEvent.getAllStableProvenance(AllProv);

    if (verbosity >= 0)
      edm::LogInfo(MsgLoggerCat)
          << "Number of Provenances = " << AllProv.size();

    if (printProvenanceInfo && (verbosity >= 0)) {
      TString eventout("\nProvenance info:\n");

      for (unsigned int i = 0; i < AllProv.size(); ++i) {
        eventout += "\n       ******************************";
        eventout += "\n       Module       : ";
        eventout += AllProv[i]->moduleLabel();
        eventout += "\n       ProductID    : ";
        eventout += AllProv[i]->productID().id();
        eventout += "\n       ClassName    : ";
        eventout += AllProv[i]->className();
        eventout += "\n       InstanceName : ";
        eventout += AllProv[i]->productInstanceName();
        eventout += "\n       BranchName   : ";
        eventout += AllProv[i]->branchName();
      }
      eventout += "\n       ******************************\n";
      edm::LogInfo(MsgLoggerCat) << eventout << "\n";
      printProvenanceInfo = false;
    }
    getAllProvenances = false;
  }

  // fill histograms
  edm::Handle<PGlobalSimHit> srcGlobalHits;
  iEvent.getByToken(GlobalHitSrc_Token_, srcGlobalHits);
  if (!srcGlobalHits.isValid()) {
    edm::LogWarning(MsgLoggerCat) << "Unable to find PGlobalSimHit in event!";
    return;
  }

  nPxlBrlHits = srcGlobalHits->getnPxlBrlHits();
  nPxlFwdHits = srcGlobalHits->getnPxlFwdHits();
  nPxlHits = nPxlBrlHits + nPxlFwdHits;
  nSiBrlHits = srcGlobalHits->getnSiBrlHits();
  nSiFwdHits = srcGlobalHits->getnSiFwdHits();
  nSiHits = nSiBrlHits + nSiFwdHits;
  nMuonDtHits = srcGlobalHits->getnMuonDtHits();
  nMuonCscHits = srcGlobalHits->getnMuonCscHits();
  nMuonRpcBrlHits = srcGlobalHits->getnMuonRpcBrlHits();
  nMuonRpcFwdHits = srcGlobalHits->getnMuonRpcFwdHits();
  nMuonHits = nMuonDtHits + nMuonCscHits + nMuonRpcBrlHits + nMuonRpcFwdHits;

  for (Int_t i = 0; i < 2; ++i) {
    meMCRGP[i]->Fill((float)srcGlobalHits->getnRawGenPart());
    meMCG4Vtx[i]->Fill((float)srcGlobalHits->getnG4Vtx());
    meMCG4Trk[i]->Fill((float)srcGlobalHits->getnG4Trk());
    meCaloEcal[i]->Fill((float)srcGlobalHits->getnECalHits());
    meCaloPreSh[i]->Fill((float)srcGlobalHits->getnPreShHits());
    meCaloHcal[i]->Fill((float)srcGlobalHits->getnHCalHits());
    meTrackerPx[i]->Fill((float)nPxlHits);
    meTrackerSi[i]->Fill((float)nSiHits);
    meMuon[i]->Fill((float)nMuonHits);
  }

  // get G4Vertex info
  std::vector<PGlobalSimHit::Vtx> G4Vtx = srcGlobalHits->getG4Vtx();
  for (unsigned int i = 0; i < G4Vtx.size(); ++i) {
    for (int j = 0; j < 2; ++j) {
      meGeantVtxX[j]->Fill(G4Vtx[i].x);
      meGeantVtxY[j]->Fill(G4Vtx[i].y);
      meGeantVtxZ[j]->Fill(G4Vtx[i].z);
    }
  }

  // get G4Track info
  std::vector<PGlobalSimHit::Trk> G4Trk = srcGlobalHits->getG4Trk();
  for (unsigned int i = 0; i < G4Trk.size(); ++i) {
    meGeantTrkPt->Fill(G4Trk[i].pt);
    meGeantTrkE->Fill(G4Trk[i].e);
  }

  // get Ecal info
  std::vector<PGlobalSimHit::CalHit> ECalHits = srcGlobalHits->getECalHits();
  for (unsigned int i = 0; i < ECalHits.size(); ++i) {
    for (Int_t j = 0; j < 2; ++j) {
      meCaloEcalE[j]->Fill(ECalHits[i].e);
      meCaloEcalToF[j]->Fill(ECalHits[i].tof);
    }
    meCaloEcalPhi->Fill(ECalHits[i].phi);
    meCaloEcalEta->Fill(ECalHits[i].eta);
  }

  // get PreShower info
  std::vector<PGlobalSimHit::CalHit> PreShHits = srcGlobalHits->getPreShHits();
  for (unsigned int i = 0; i < PreShHits.size(); ++i) {
    for (Int_t j = 0; j < 2; ++j) {
      meCaloPreShE[j]->Fill(PreShHits[i].e);
      meCaloPreShToF[j]->Fill(PreShHits[i].tof);
    }
    meCaloPreShPhi->Fill(PreShHits[i].phi);
    meCaloPreShEta->Fill(PreShHits[i].eta);
  }

  // get Hcal info
  std::vector<PGlobalSimHit::CalHit> HCalHits = srcGlobalHits->getHCalHits();
  for (unsigned int i = 0; i < HCalHits.size(); ++i) {
    for (Int_t j = 0; j < 2; ++j) {
      meCaloHcalE[j]->Fill(HCalHits[i].e);
      meCaloHcalToF[j]->Fill(HCalHits[i].tof);
    }
    meCaloHcalPhi->Fill(HCalHits[i].phi);
    meCaloHcalEta->Fill(HCalHits[i].eta);
  }

  // get Pixel Barrel info
  std::vector<PGlobalSimHit::BrlHit> PxlBrlHits =
      srcGlobalHits->getPxlBrlHits();
  for (unsigned int i = 0; i < PxlBrlHits.size(); ++i) {
    meTrackerPxPhi->Fill(PxlBrlHits[i].phi);
    meTrackerPxEta->Fill(PxlBrlHits[i].eta);
    meTrackerPxBToF->Fill(PxlBrlHits[i].tof);
    meTrackerPxBR->Fill(PxlBrlHits[i].r);
  }

  // get Pixel Forward info
  std::vector<PGlobalSimHit::FwdHit> PxlFwdHits =
      srcGlobalHits->getPxlFwdHits();
  for (unsigned int i = 0; i < PxlFwdHits.size(); ++i) {
    meTrackerPxPhi->Fill(PxlFwdHits[i].phi);
    meTrackerPxEta->Fill(PxlFwdHits[i].eta);
    meTrackerPxFToF->Fill(PxlFwdHits[i].tof);
    meTrackerPxFZ->Fill(PxlFwdHits[i].z);
  }

  // get Strip Barrel info
  std::vector<PGlobalSimHit::BrlHit> SiBrlHits = srcGlobalHits->getSiBrlHits();
  for (unsigned int i = 0; i < SiBrlHits.size(); ++i) {
    meTrackerSiPhi->Fill(SiBrlHits[i].phi);
    meTrackerSiEta->Fill(SiBrlHits[i].eta);
    meTrackerSiBToF->Fill(SiBrlHits[i].tof);
    meTrackerSiBR->Fill(SiBrlHits[i].r);
  }

  // get Strip Forward info
  std::vector<PGlobalSimHit::FwdHit> SiFwdHits = srcGlobalHits->getSiFwdHits();
  for (unsigned int i = 0; i < SiFwdHits.size(); ++i) {
    meTrackerSiPhi->Fill(SiFwdHits[i].phi);
    meTrackerSiEta->Fill(SiFwdHits[i].eta);
    meTrackerSiFToF->Fill(SiFwdHits[i].tof);
    meTrackerSiFZ->Fill(SiFwdHits[i].z);
  }

  // get Muon CSC info
  std::vector<PGlobalSimHit::FwdHit> MuonCscHits =
      srcGlobalHits->getMuonCscHits();
  for (unsigned int i = 0; i < MuonCscHits.size(); ++i) {
    meMuonPhi->Fill(MuonCscHits[i].phi);
    meMuonEta->Fill(MuonCscHits[i].eta);
    for (Int_t j = 0; j < 2; ++j) {
      meMuonCscToF[j]->Fill(MuonCscHits[i].tof);
    }
    meMuonCscZ->Fill(MuonCscHits[i].z);
  }

  // get Muon DT info
  std::vector<PGlobalSimHit::BrlHit> MuonDtHits =
      srcGlobalHits->getMuonDtHits();
  for (unsigned int i = 0; i < MuonDtHits.size(); ++i) {
    meMuonPhi->Fill(MuonDtHits[i].phi);
    meMuonEta->Fill(MuonDtHits[i].eta);
    for (Int_t j = 0; j < 2; ++j) {
      meMuonDtToF[j]->Fill(MuonDtHits[i].tof);
    }
    meMuonDtR->Fill(MuonDtHits[i].r);
  }

  // get Muon RPC forward info
  std::vector<PGlobalSimHit::FwdHit> MuonRpcFwdHits =
      srcGlobalHits->getMuonRpcFwdHits();
  for (unsigned int i = 0; i < MuonRpcFwdHits.size(); ++i) {
    meMuonPhi->Fill(MuonRpcFwdHits[i].phi);
    meMuonEta->Fill(MuonRpcFwdHits[i].eta);
    for (Int_t j = 0; j < 2; ++j) {
      meMuonRpcFToF[j]->Fill(MuonRpcFwdHits[i].tof);
    }
    meMuonRpcFZ->Fill(MuonRpcFwdHits[i].z);
  }

  // get Muon RPC barrel info
  std::vector<PGlobalSimHit::BrlHit> MuonRpcBrlHits =
      srcGlobalHits->getMuonRpcBrlHits();
  for (unsigned int i = 0; i < MuonRpcBrlHits.size(); ++i) {
    meMuonPhi->Fill(MuonRpcBrlHits[i].phi);
    meMuonEta->Fill(MuonRpcBrlHits[i].eta);
    for (Int_t j = 0; j < 2; ++j) {
      meMuonRpcBToF[j]->Fill(MuonRpcBrlHits[i].tof);
    }
    meMuonRpcBR->Fill(MuonRpcBrlHits[i].r);
  }

  return;
}
