#ifndef SUSY_HLT_DoubleEle_Hadronic_H
#define SUSY_HLT_DoubleEle_Hadronic_H

// event
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"

// DQM
#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DQMServices/Core/interface/DQMStore.h"
#include "DQMServices/Core/interface/MonitorElement.h"

// Electron
#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/EgammaCandidates/interface/ElectronFwd.h"

#include "DataFormats/EgammaCandidates/interface/GsfElectron.h"
#include "DataFormats/EgammaCandidates/interface/GsfElectronFwd.h"

// Jets
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/JetReco/interface/PFJet.h"

// Trigger
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
#include "DataFormats/HLTReco/interface/TriggerEventWithRefs.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

class SUSY_HLT_DoubleEle_Hadronic : public DQMEDAnalyzer {

public:
  SUSY_HLT_DoubleEle_Hadronic(const edm::ParameterSet &ps);
  ~SUSY_HLT_DoubleEle_Hadronic() override;

protected:
  void dqmBeginRun(edm::Run const &, edm::EventSetup const &) override;
  void bookHistograms(DQMStore::IBooker &, edm::Run const &,
                      edm::EventSetup const &) override;
  void analyze(edm::Event const &e, edm::EventSetup const &eSetup) override;
  void endRun(edm::Run const &run, edm::EventSetup const &eSetup) override;

private:
  // histos booking function
  void bookHistos(DQMStore::IBooker &);

  // variables from config file
  edm::EDGetTokenT<reco::GsfElectronCollection> theElectronCollection_;
  edm::EDGetTokenT<reco::PFJetCollection> thePfJetCollection_;
  edm::EDGetTokenT<reco::CaloJetCollection> theCaloJetCollection_;
  edm::EDGetTokenT<edm::TriggerResults> triggerResults_;
  edm::EDGetTokenT<trigger::TriggerEvent> theTrigSummary_;

  HLTConfigProvider fHltConfig;

  std::string HLTProcess_;
  std::string triggerPath_;
  std::string triggerPathAuxiliaryForElectron_;
  std::string triggerPathAuxiliaryForHadronic_;
  edm::InputTag triggerFilter_;
  double ptThrJet_;
  double etaThrJet_;

  // Histograms
  MonitorElement *h_triggerElePt;
  MonitorElement *h_triggerEleEta;
  MonitorElement *h_triggerElePhi;
  MonitorElement *h_triggerDoubleEleMass;
  MonitorElement *h_pfHTTurnOn_num;
  MonitorElement *h_pfHTTurnOn_den;
  MonitorElement *h_EleTurnOn_num;
  MonitorElement *h_EleTurnOn_den;
};

#endif
