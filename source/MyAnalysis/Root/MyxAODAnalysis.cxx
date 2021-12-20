#include <AsgTools/MessageCheck.h>
#include <MyAnalysis/MyxAODAnalysis.h>
					
#include <xAODEventInfo/EventInfo.h>
#include <xAODJet/JetContainer.h>

// HI event includes                                                                                             
#include "xAODHIEvent/HIEventShapeContainer.h"

//xAOD
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TStore.h"
#include "xAODCore/AuxContainerBase.h"
#include "xAODEventInfo/EventInfo.h"


//EventLoop
#include <AsgTools/MessageCheck.h>
#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>

//ROOT                                                                                                                        
#include <TFile.h>
#include <TSystem.h>
//Cross Calibration                                                                                              
#include "AsgTools/AnaToolHandle.h"
#include "JetCalibTools/JetCalibrationTool.h"

#include <iostream>

// E/gamma tools                                                                                                                                                                          
#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"
// Egamma & muons                                                                                                                                                                         
#include "xAODEgamma/Photon.h"
#include "xAODEgamma/PhotonContainer.h"
#include "ElectronPhotonSelectorTools/egammaPIDdefs.h"

// Path resolver tool
#include <PathResolver/PathResolver.h>

// Tracking & vertex includes
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/TrackParticlexAODHelpers.h"

// Truth includes
#include "xAODTruth/TruthEvent.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthVertex.h"
#include "xAODTruth/TruthParticle.h"
#include "xAODTruth/xAODTruthHelpers.h"
		
MyxAODAnalysis :: MyxAODAnalysis (const std::string& name,ISvcLocator *pSvcLocator): 
	EL::AnaAlgorithm (name, pSvcLocator),
	m_trigDecisionTool ("Trig::TrigDecisionTool/TrigDecisionTool"),
	m_trigConfigTool("TrigConf::xAODConfigTool/xAODConfigTool"){
	
	// Here you put any code for the base initialization of variables,
        // e.g. initialize all pointers to 0.  This is also where you
        // declare all properties for your algorithm.  Note that things like
        // resetting statistics variables or booking histograms should
        // rather go into the initialize() function.

	m_photonTightIsEMSelector = nullptr;
	m_grl = nullptr;
	m_Akt10HI_EtaJES_Insitu_CalibTool = nullptr;
	m_Akt4HI_EtaJES_Insitu_CalibTool = nullptr;
	m_egammaCalibrationAndSmearingTool = nullptr;
	m_egammaCalibrationAndSmearingTool_sys = nullptr;
}



StatusCode MyxAODAnalysis :: initialize (){
	// Here you do everything that needs to be done at the very
	// beginning on each worker node, e.g. create histograms and output
	// trees.  This method gets called before any input files are
	// connected.

	ANA_CHECK (book (TTree ("analysis", "My analysis ntuple")));
	TTree* mytree = tree ("analysis");

	//Run and Event Number

	mytree->Branch ("RunNumber", &m_b_runNumber);
	mytree->Branch ("EventNumber", &m_b_eventNumber);

	//FCal

	mytree->Branch ("fcalA_et", &m_b_fcalA_et, "fcalA_et/F");
	mytree->Branch ("fcalC_et", &m_b_fcalC_et, "fcalC_et/F");

	//photon triggers pp 2017

	if (m_Collisions){
		if (m_pp17){
			mytree->Branch("HLT_g30_loose_L1EM15",     &HLT_g30_loose_L1EM15, "HLT_g30_loose_L1EM15/O");
			mytree->Branch("HLT_g35_loose_L1EM15",     &HLT_g35_loose_L1EM15, "HLT_g35_loose_L1EM15/O");
		}
		if (m_pp15){
			mytree->Branch("HLT_g30_loose",     &HLT_g30_loose, "HLT_g30_loose/O");
			mytree->Branch("HLT_g30_loose_L1EM15",     &HLT_g30_loose_L1EM15, "HLT_g30_loose_L1EM15/O");
		}
	}

	
 	//R = 1.0 jets -- Reco

	mytree->Branch ("akt10hi_jet_n",   &m_b_akt10hi_jet_n, "akt10hi_jet_n/I");
	mytree->Branch ("akt10hi_constit_jet_pt",  &m_b_akt10hi_constit_jet_pt);
	mytree->Branch ("akt10hi_constit_jet_eta", &m_b_akt10hi_constit_jet_eta);
	mytree->Branch ("akt10hi_constit_jet_phi", &m_b_akt10hi_constit_jet_phi);
	mytree->Branch ("akt10hi_constit_jet_e",   &m_b_akt10hi_constit_jet_e);

	mytree->Branch ("akt10hi_em_jet_pt",  &m_b_akt10hi_em_jet_pt);
	mytree->Branch ("akt10hi_em_jet_eta", &m_b_akt10hi_em_jet_eta);
	mytree->Branch ("akt10hi_em_jet_phi", &m_b_akt10hi_em_jet_phi);
	mytree->Branch ("akt10hi_em_jet_e",   &m_b_akt10hi_em_jet_e);

	mytree->Branch ("akt10hi_jet_etaJES_calib_pt",  &m_b_akt10hi_jet_etaJES_calib_pt);
	mytree->Branch ("akt10hi_jet_etaJES_calib_eta",  &m_b_akt10hi_jet_etaJES_calib_eta);
	mytree->Branch ("akt10hi_jet_etaJES_calib_phi",  &m_b_akt10hi_jet_etaJES_calib_phi);
	mytree->Branch ("akt10hi_jet_etaJES_calib_e",  &m_b_akt10hi_jet_etaJES_calib_e);

	mytree->Branch ("akt10hi_jet_Insitu_calib_pt",  &m_b_akt10hi_jet_Insitu_calib_pt);
	mytree->Branch ("akt10hi_jet_Insitu_calib_eta", &m_b_akt10hi_jet_Insitu_calib_eta);
	mytree->Branch ("akt10hi_jet_Insitu_calib_phi", &m_b_akt10hi_jet_Insitu_calib_phi);
	mytree->Branch ("akt10hi_jet_Insitu_calib_e",   &m_b_akt10hi_jet_Insitu_calib_e);

	
	//R = 0.4 jets -- Reco

	mytree->Branch ("akt4hi_jet_n",   &m_b_akt4hi_jet_n, "akt4hi_jet_n/I");
	mytree->Branch ("akt4hi_constit_jet_pt",  &m_b_akt4hi_constit_jet_pt);
	mytree->Branch ("akt4hi_constit_jet_eta", &m_b_akt4hi_constit_jet_eta);
	mytree->Branch ("akt4hi_constit_jet_phi", &m_b_akt4hi_constit_jet_phi);
	mytree->Branch ("akt4hi_constit_jet_e",   &m_b_akt4hi_constit_jet_e);

	mytree->Branch ("akt4hi_em_jet_pt",  &m_b_akt4hi_em_jet_pt);
	mytree->Branch ("akt4hi_em_jet_eta", &m_b_akt4hi_em_jet_eta);
	mytree->Branch ("akt4hi_em_jet_phi", &m_b_akt4hi_em_jet_phi);
	mytree->Branch ("akt4hi_em_jet_e",   &m_b_akt4hi_em_jet_e);

	mytree->Branch ("akt4hi_jet_etaJES_calib_pt",  &m_b_akt4hi_jet_etaJES_calib_pt);
	mytree->Branch ("akt4hi_jet_etaJES_calib_eta",  &m_b_akt4hi_jet_etaJES_calib_eta);
	mytree->Branch ("akt4hi_jet_etaJES_calib_phi",  &m_b_akt4hi_jet_etaJES_calib_phi);
	mytree->Branch ("akt4hi_jet_etaJES_calib_e",  &m_b_akt4hi_jet_etaJES_calib_e);
	  
	mytree->Branch ("akt4hi_jet_Insitu_calib_pt",  &m_b_akt4hi_jet_Insitu_calib_pt);
	mytree->Branch ("akt4hi_jet_Insitu_calib_eta", &m_b_akt4hi_jet_Insitu_calib_eta);
	mytree->Branch ("akt4hi_jet_Insitu_calib_phi", &m_b_akt4hi_jet_Insitu_calib_phi);
	mytree->Branch ("akt4hi_jet_Insitu_calib_e",   &m_b_akt4hi_jet_Insitu_calib_e);

	//photons -- Reco

	mytree->Branch ("photon_pt_n",  &m_b_photon_n);
	mytree->Branch ("photon_pt_precalib",  &m_b_photon_pt_precalib);
	mytree->Branch ("photon_pt",  &m_b_photon_pt);
	mytree->Branch ("photon_pt_new",  &m_b_photon_pt_new);
	mytree->Branch ("photon_eta", &m_b_photon_eta);
	mytree->Branch ("photon_phi", &m_b_photon_phi);
	mytree->Branch ("photon_tight", &m_b_photon_tight);
	mytree->Branch ("photon_etcone30", &m_b_photon_etcone30);
	mytree->Branch ("photon_topoetcone40", &m_b_photon_topoetcone40);


	if (!m_Collisions){
		
		//R = 1.0 jets (MC truth)

		mytree->Branch ("akt10_truth_jet_n",   &m_b_akt10_truth_jet_n, "akt10_truth_jet_n/I");
		mytree->Branch ("akt10_truth_jet_pt",  &m_b_akt10_truth_jet_pt);
		mytree->Branch ("akt10_truth_jet_eta", &m_b_akt10_truth_jet_eta);
		mytree->Branch ("akt10_truth_jet_phi", &m_b_akt10_truth_jet_phi);
		mytree->Branch ("akt10_truth_jet_e",   &m_b_akt10_truth_jet_e);
		
		//R = 0.4 jets (MC truth)

		mytree->Branch ("akt4_truth_jet_n",   &m_b_akt4_truth_jet_n, "akt4_truth_jet_n/I");
		mytree->Branch ("akt4_truth_jet_pt",  &m_b_akt4_truth_jet_pt);
		mytree->Branch ("akt4_truth_jet_eta", &m_b_akt4_truth_jet_eta);
		mytree->Branch ("akt4_truth_jet_phi", &m_b_akt4_truth_jet_phi);
		mytree->Branch ("akt4_truth_jet_e",   &m_b_akt4_truth_jet_e);

		//photons (MC truth)

		mytree->Branch ("truth_photon_pt_n",  &m_b_truth_photon_n);
		mytree->Branch ("truth_photon_pt",  &m_b_truth_photon_pt);
		mytree->Branch ("truth_photon_eta", &m_b_truth_photon_eta);
		mytree->Branch ("truth_photon_phi", &m_b_truth_photon_phi);

		// Photon pT systematics

		mytree->Branch("photon_pt_sys1", &m_b_photon_pt_sys1);
		mytree->Branch("photon_pt_sys2", &m_b_photon_pt_sys2);
		mytree->Branch("photon_pt_sys3", &m_b_photon_pt_sys3);
		mytree->Branch("photon_pt_sys4", &m_b_photon_pt_sys4);
	
		//MC Eventweights

		mytree->Branch ("mcEventWeights", &m_b_mcEventWeights);
	}

	//vertex

	mytree->Branch("nvert",   &m_b_nvert, "nvert/I");
	mytree->Branch ("vert_x",  &m_b_vert_x);
	mytree->Branch ("vert_y",  &m_b_vert_y);
	mytree->Branch ("vert_z",  &m_b_vert_z);
	mytree->Branch ("vert_type",  &m_b_vert_type);
	mytree->Branch ("vert_ntrk",  &m_b_vert_ntrk);

	if (m_Collisions){
		//initialize and configure trigger tools

		ANA_CHECK (m_trigConfigTool.initialize());
		ANA_CHECK (m_trigDecisionTool.setProperty ("ConfigTool", m_trigConfigTool.getHandle())); // connect the TrigDecisionTool to the ConfigTool
		ANA_CHECK (m_trigDecisionTool.setProperty ("TrigDecisionKey", "xTrigDecision"));
		ANA_CHECK (m_trigDecisionTool.initialize());
	}

	//initialize photon selection tools

	m_photonTightIsEMSelector = new AsgPhotonIsEMSelector  ("PhotonTightIsEMSelector");
	ANA_CHECK (m_photonTightIsEMSelector->setProperty ("isEMMask", egammaPID::PhotonTight));
	ANA_CHECK (m_photonTightIsEMSelector->setProperty ("ConfigFile","ElectronPhotonSelectorTools/offline/mc15_20150712/PhotonIsEMTightSelectorCutDefs.conf"));
	ANA_CHECK (m_photonTightIsEMSelector->initialize ());

	//Egamma calibration tool

	m_egammaCalibrationAndSmearingTool = new CP::EgammaCalibrationAndSmearingTool ("EgammaCalibrationAndSmearingTool");
	if (m_pp17){
		ANA_CHECK (m_egammaCalibrationAndSmearingTool->setProperty ("ESModel", "es2017_R21_v1"));
		}
	if (m_pp15){
		ANA_CHECK (m_egammaCalibrationAndSmearingTool->setProperty ("ESModel", "es2017_R21_ofc0_v1"));//ofc0
		}
	ANA_CHECK (m_egammaCalibrationAndSmearingTool->setProperty ("decorrelationModel", "1NP_v1")); // 1NP_v1,FULL_v1
	ANA_CHECK (m_egammaCalibrationAndSmearingTool->setProperty ("useAFII", 0));
	if (!m_Collisions){
		ANA_CHECK (m_egammaCalibrationAndSmearingTool->setProperty ("randomRunNumber", EgammaCalibPeriodRunNumbersExample::run_2016));
	}
	ANA_CHECK (m_egammaCalibrationAndSmearingTool->initialize ());
	
	//Egamma calibration toolsys

	m_egammaCalibrationAndSmearingTool_sys = new CP::EgammaCalibrationAndSmearingTool ("EgammaCalibrationAndSmearingTool_sys");
	if (m_pp17){
		ANA_CHECK (m_egammaCalibrationAndSmearingTool_sys->setProperty ("ESModel", "es2017_R21_v1"));
		}
	if (m_pp15){
		ANA_CHECK (m_egammaCalibrationAndSmearingTool_sys->setProperty ("ESModel", "es2017_R21_ofc0_v1"));//ofc0
		}
	ANA_CHECK (m_egammaCalibrationAndSmearingTool_sys->setProperty ("decorrelationModel", "1NP_v1")); // 1NP_v1,FULL_v1
	ANA_CHECK (m_egammaCalibrationAndSmearingTool_sys->setProperty ("useAFII", 0));
	if (!m_Collisions){
		ANA_CHECK (m_egammaCalibrationAndSmearingTool_sys->setProperty ("randomRunNumber", EgammaCalibPeriodRunNumbersExample::run_2016));
	}
	ANA_CHECK (m_egammaCalibrationAndSmearingTool_sys->initialize ());

	
	//initialize xCalib/Insitu JetCalibTool -- R = 1.0
	  
	m_Akt10HI_EtaJES_Insitu_CalibTool = new JetCalibrationTool("Akt10HI_EtaJES_Insitu_CalibTool");
	ANA_CHECK(m_Akt10HI_EtaJES_Insitu_CalibTool->setProperty("JetCollection", "AntiKt10HI"));
	ANA_CHECK(m_Akt10HI_EtaJES_Insitu_CalibTool->setProperty("ConfigFile", "JES_MC16_HI_Jan2021_5TeV.config"));//default is Jan
	ANA_CHECK(m_Akt10HI_EtaJES_Insitu_CalibTool->setProperty("DEVmode",true)); //true if need to be read locally
	ANA_CHECK(m_Akt10HI_EtaJES_Insitu_CalibTool->setProperty("CalibSequence", m_Collisions ? "EtaJES_Insitu" : "EtaJES"));
	ANA_CHECK(m_Akt10HI_EtaJES_Insitu_CalibTool->setProperty("IsData", m_Collisions));
	m_Akt10HI_EtaJES_Insitu_CalibTool->initializeTool("Akt10HI_EtaJES_Insitu_CalibTool");
	
	//initialize xCalib/Insitu JetCalibTool -- R = 0.4
	  
	m_Akt4HI_EtaJES_Insitu_CalibTool = new JetCalibrationTool("Akt4HI_EtaJES_Insitu_CalibTool");
	ANA_CHECK(m_Akt4HI_EtaJES_Insitu_CalibTool->setProperty("JetCollection", "AntiKt4HI"));
	ANA_CHECK(m_Akt4HI_EtaJES_Insitu_CalibTool->setProperty("ConfigFile", "JES_MC16_HI_Jan2021_5TeV.config"));//default is Jan
	ANA_CHECK(m_Akt4HI_EtaJES_Insitu_CalibTool->setProperty("DEVmode",false));
	ANA_CHECK(m_Akt4HI_EtaJES_Insitu_CalibTool->setProperty("CalibSequence", m_Collisions ? "EtaJES_Insitu" : "EtaJES"));
	ANA_CHECK(m_Akt4HI_EtaJES_Insitu_CalibTool->setProperty("IsData", m_Collisions));
	m_Akt4HI_EtaJES_Insitu_CalibTool->initializeTool("Akt4HI_EtaJES_Insitu_CalibTool");


	if (m_Collisions){
		//GRL

		const char* GRLFilePath = "$UserAnalysis_DIR/data/MyAnalysis";   
		const char* pp17_fullGRLFilePath = gSystem->ExpandPathName(Form("%s/data17_5TeV.periodAllYear_DetStatus-v98-pro21-16_Unknown_PHYS_StandardGRL_All_Good_25ns_ignore_GLOBAL_LOWMU.xml", GRLFilePath));
		const char* pp15_fullGRLFilePath = gSystem->ExpandPathName(Form("%s/LB_collection_pp2015.xml", GRLFilePath));
		std::string toolStr = "";
		std::string pathStr = "";
		
		if (m_pp17){
			pathStr = pp17_fullGRLFilePath;
			toolStr = "pp17";
		}
		
		if (m_pp15){
			pathStr = pp15_fullGRLFilePath;
			toolStr = "pp15";
		}

		std::vector<std::string> vecStringGRL = {pathStr};
		m_grl = new GoodRunsListSelectionTool((toolStr + "_GoodRunsListSelectionTool").c_str());
		ANA_CHECK(m_grl->setProperty("GoodRunsListVec", vecStringGRL));
		ANA_CHECK(m_grl->setProperty("PassThrough", false)); // if true(default) will ignore result of GRL and will just pass all events
		ANA_CHECK(m_grl->initialize());
	}

	//initialize photon recommended systematics list

	Info ("initialize ()", "Initializing photon recommended CP systematic set...");
	CP::SystematicSet recommendedPhotonSystematics;
	recommendedPhotonSystematics.insert (m_egammaCalibrationAndSmearingTool->recommendedSystematics ());
	for (auto sys : recommendedPhotonSystematics) {
		TString syst_name = TString (sys.name ());
		if (!syst_name.BeginsWith ("EG_SCALE_ALL") && !syst_name.BeginsWith ("EG_RESOLUTION_ALL")) {
			continue;
		}
		Info ("initialize ()", Form ("  --> adding photon systematic %s", sys.name ().c_str ()));
		photonSysList.push_back (CP::SystematicSet ({sys}));
	}
	Info ("initialize ()", "Photon recommended CP systematic set ready.");

	return StatusCode::SUCCESS;
}



StatusCode MyxAODAnalysis :: execute ()
{
	// Here you do everything that needs to be done on every single
	// events, e.g. read input variables, apply cuts, and fill
	// histograms and trees.  This is where most of your actual analysis
	// code will go.

	ANA_MSG_INFO ("in execute");

	// retrieve the eventInfo object from the event store

	const xAOD::EventInfo *eventInfo = nullptr;
	ANA_CHECK (evtStore()->retrieve (eventInfo, "EventInfo"));
	

	//pp photon triggers

	if (m_Collisions){
		
		if (m_pp17){
			HLT_g30_loose_L1EM15 = false;
			HLT_g35_loose_L1EM15 = false;
		}
		
		if (m_pp15){
			HLT_g30_loose = false;
			HLT_g30_loose_L1EM15 = false;
		}
	}

	//FCal

	m_b_fcalA_et = 0;
	m_b_fcalC_et = 0;

	
	//R = 1.0 jets -- Reco

	m_b_akt10hi_jet_n = 0;

	m_b_akt10hi_constit_jet_pt.clear ();
	m_b_akt10hi_constit_jet_eta.clear ();
	m_b_akt10hi_constit_jet_phi.clear ();
	m_b_akt10hi_constit_jet_e.clear ();

	m_b_akt10hi_em_jet_pt.clear ();
	m_b_akt10hi_em_jet_eta.clear ();
	m_b_akt10hi_em_jet_phi.clear ();
	m_b_akt10hi_em_jet_e.clear ();

	m_b_akt10hi_jet_etaJES_calib_pt.clear ();
	m_b_akt10hi_jet_etaJES_calib_eta.clear ();
	m_b_akt10hi_jet_etaJES_calib_phi.clear ();
	m_b_akt10hi_jet_etaJES_calib_e.clear ();

	m_b_akt10hi_jet_Insitu_calib_pt.clear ();
	m_b_akt10hi_jet_Insitu_calib_eta.clear ();
	m_b_akt10hi_jet_Insitu_calib_phi.clear ();
	m_b_akt10hi_jet_Insitu_calib_e.clear ();

	//R = 0.4 jets -- Reco

	m_b_akt4hi_jet_n = 0;

	m_b_akt4hi_constit_jet_pt.clear ();
	m_b_akt4hi_constit_jet_eta.clear ();
	m_b_akt4hi_constit_jet_phi.clear ();
	m_b_akt4hi_constit_jet_e.clear ();

	m_b_akt4hi_em_jet_pt.clear ();
	m_b_akt4hi_em_jet_eta.clear ();
	m_b_akt4hi_em_jet_phi.clear ();
	m_b_akt4hi_em_jet_e.clear ();

	m_b_akt4hi_jet_etaJES_calib_pt.clear ();
	m_b_akt4hi_jet_etaJES_calib_eta.clear ();
	m_b_akt4hi_jet_etaJES_calib_phi.clear ();
	m_b_akt4hi_jet_etaJES_calib_e.clear ();

	m_b_akt4hi_jet_Insitu_calib_pt.clear ();
	m_b_akt4hi_jet_Insitu_calib_eta.clear ();
	m_b_akt4hi_jet_Insitu_calib_phi.clear ();
	m_b_akt4hi_jet_Insitu_calib_e.clear ();

	//Photon

	m_b_photon_n = 0;

	m_b_photon_pt_precalib.clear ();
	m_b_photon_pt.clear ();
	m_b_photon_pt_new.clear ();
	m_b_photon_eta.clear ();
	m_b_photon_phi.clear ();
	m_b_photon_tight.clear();
	m_b_photon_etcone30.clear();
	m_b_photon_topoetcone40.clear();


	if (!m_Collisions){

		//R = 1.0 jets -- MC Truth

		m_b_akt10_truth_jet_n = 0;
		m_b_akt10_truth_jet_pt.clear ();
		m_b_akt10_truth_jet_eta.clear ();
		m_b_akt10_truth_jet_phi.clear ();
		m_b_akt10_truth_jet_e.clear ();

		//R = 0.4 jets -- MC Truth

		m_b_akt4_truth_jet_n = 0;
		m_b_akt4_truth_jet_pt.clear ();
		m_b_akt4_truth_jet_eta.clear ();
		m_b_akt4_truth_jet_phi.clear ();
		m_b_akt4_truth_jet_e.clear ();

		//MC Truth Photon

		m_b_truth_photon_n = 0;
		m_b_truth_photon_pt.clear ();
		m_b_truth_photon_eta.clear ();
		m_b_truth_photon_phi.clear ();

		m_b_photon_pt_sys1.clear();
		m_b_photon_pt_sys2.clear();
		m_b_photon_pt_sys3.clear();
		m_b_photon_pt_sys4.clear();
	
	}

	//vertex

	m_b_nvert = 0;
	m_b_vert_x.clear();	
	m_b_vert_y.clear();	
	m_b_vert_z.clear();	
	m_b_vert_type.clear();	
	m_b_vert_ntrk.clear();	

	//event no and run no

	m_b_runNumber = eventInfo->runNumber();
	m_b_eventNumber = eventInfo->eventNumber();

	//event satisfies grl criterion

	if(m_Collisions){
		if(!m_grl->passRunLB(*eventInfo)) return EL::StatusCode::SUCCESS; 
	}

	bool photon_min_pt_tight = false;

        	
	//get HI Jets with R=1.0 from jet container

	const xAOD::JetContainer* jet10Container = 0;                                                   
	if (!evtStore ()->retrieve (jet10Container, m_der ? "DFAntiKt10HIJets" : "AntiKt10HIJets").isSuccess ()) {
		Error ("GetAntiKt10HIJets ()", "Failed to retrieve AntiKt10HIJets collection. Exiting.");
		return EL::StatusCode::FAILURE;
	}else{
		Info ("GetAntiKt10HIJets ()", "Successfully retrieved AntiKt10HIJets container.");
	}      

	for (const auto* init_jet : *jet10Container) {                                                             
		xAOD::Jet* jet_1 = new xAOD::Jet();
		jet_1->makePrivateStore(*init_jet);
		
		const xAOD::JetFourMom_t jet_4mom_subtracted = init_jet->jetP4("JetSubtractedScaleMomentum");
		jet_1->setJetP4("JetConstitScaleMomentum", jet_4mom_subtracted);
		jet_1->setJetP4("JetEMScaleMomentum", jet_4mom_subtracted);//required in derivations
		
		const float constit_pt = 1e-3 * jet_4mom_subtracted.pt();
		const float constit_eta = jet_4mom_subtracted.eta();
		const float constit_phi = jet_4mom_subtracted.phi();
		const float constit_e = 1e-3*jet_4mom_subtracted.e();
		
		
		//const xAOD::JetFourMom_t jet_4mom_constit = init_jet->jetP4("JetConstitScaleMomentum");
		//const float constit_pt = 1e-3 * jet_4mom_constit.pt();
		//const float constit_eta = jet_4mom_constit.eta();
		//const float constit_phi = jet_4mom_constit.phi();
		//const float constit_e = 1e-3*jet_4mom_constit.e();

		// for derivations, only unsubtracted and subtracted momenta are stored, so we need to set the constituent and EM scales(which are the same, respectively)
		
		//jet_1->setJetP4("JetConstitScaleMomentum", jet_1->jetP4("JetUnsubtractedScaleMomentum"));
		//jet_1->setJetP4("JetEMScaleMomentum", jet_1->jetP4("JetSubtractedScaleMomentum"));
		

		// set pre-EtaJES 4-momentum at the EM scale

		const xAOD::JetFourMom_t jet_4mom_em = jet_1->jetP4("JetEMScaleMomentum");
		jet_1->setJetP4("JetPileupScaleMomentum", jet_4mom_em);

		const float em_pt = 1e-3 * jet_4mom_em.pt();
		const float em_eta = jet_4mom_em.eta();
		const float em_phi = jet_4mom_em.phi();
		const float em_e = 1e-3 * jet_4mom_em.e();

		//apply EtaJES calibration

		ANA_CHECK(m_Akt10HI_EtaJES_Insitu_CalibTool->applyCalibration(*jet_1));
		const xAOD::JetFourMom_t jet_4mom_etajes = jet_1->jetP4("JetEtaJESScaleMomentum");
		const float etajes_pt = 1e-3 * jet_4mom_etajes.pt();
		const float etajes_eta = jet_4mom_etajes.eta();
		const float etajes_phi = jet_4mom_etajes.phi();
		const float etajes_e = 1e-3 * jet_4mom_etajes.e();

		//apply insitu calibration to data

		if (!m_Collisions){
			jet_1->setJetP4("JetInsituScaleMomentum", jet_4mom_etajes);
		}

		const xAOD::JetFourMom_t jet_4mom_insitu = jet_1->jetP4("JetInsituScaleMomentum");
		const float xcalib_pt_insitu = 1e-3 * jet_4mom_insitu.pt ();
		const float xcalib_eta_insitu = jet_4mom_insitu.eta ();
		const float xcalib_phi_insitu = jet_4mom_insitu.phi ();
		const float xcalib_e_insitu = 1e-3 * jet_4mom_insitu.e ();

		if (xcalib_pt_insitu  < 15.) {
			delete jet_1;
			continue;            
		}  

		m_b_akt10hi_constit_jet_pt.push_back (constit_pt);
		m_b_akt10hi_constit_jet_eta.push_back (constit_eta);
		m_b_akt10hi_constit_jet_phi.push_back (constit_phi);                         
		m_b_akt10hi_constit_jet_e.push_back (constit_e);  

		m_b_akt10hi_em_jet_pt.push_back (em_pt);
		m_b_akt10hi_em_jet_eta.push_back (em_eta);
		m_b_akt10hi_em_jet_phi.push_back (em_phi);
		m_b_akt10hi_em_jet_e.push_back (em_e);

		m_b_akt10hi_jet_etaJES_calib_pt.push_back (etajes_pt);
		m_b_akt10hi_jet_etaJES_calib_eta.push_back (etajes_eta);
		m_b_akt10hi_jet_etaJES_calib_phi.push_back (etajes_phi);
		m_b_akt10hi_jet_etaJES_calib_e.push_back (etajes_e);

		m_b_akt10hi_jet_Insitu_calib_pt.push_back (xcalib_pt_insitu);
		m_b_akt10hi_jet_Insitu_calib_eta.push_back (xcalib_eta_insitu);
		m_b_akt10hi_jet_Insitu_calib_phi.push_back (xcalib_phi_insitu);
		m_b_akt10hi_jet_Insitu_calib_e.push_back (xcalib_e_insitu);

		m_b_akt10hi_jet_n++;

		if (jet_1) delete jet_1;
	}// end jet loop 
	
	//get MC truth jets with R=1.0 from truth jet container

	if (!m_Collisions){
	      const xAOD::JetContainer* truthJet10Container = 0;
	      if(!evtStore()->retrieve(truthJet10Container, "AntiKt10TruthJets").isSuccess()) {
		Error("GetAntiKt10TruthJets()", "Failed to retrieve AntiKt10TruthJets collection. Exiting.");
		return EL::StatusCode::FAILURE;
	      }
		for(const auto* truthJet : *truthJet10Container) {
			if(truthJet->pt()*1e-3 < 15)
				continue;

			m_b_akt10_truth_jet_pt.push_back(truthJet->pt()*1e-3);
			m_b_akt10_truth_jet_eta.push_back(truthJet->eta());
			m_b_akt10_truth_jet_phi.push_back(truthJet->phi());
			m_b_akt10_truth_jet_e.push_back(truthJet->e()*1e-3);

			m_b_akt10_truth_jet_n++;
		} //end truth jet loop
	}

	
	//get HI Jets with R=0.4 from jet container

	const xAOD::JetContainer* jet4Container = 0;
	if (!evtStore ()->retrieve (jet4Container, "AntiKt4HIJets").isSuccess ()) {
		Error ("GetAntiKt4HIJets ()", "Failed to retrieve AntiKt4HIJets collection. Exiting." );
		return EL::StatusCode::FAILURE;
	}else{
		Info ("GetAntiKt4HIJets ()", "Successfully retrieved AntiKt4HIJets container.");
	}

	for (const auto* init_jet : *jet4Container) {

		xAOD::Jet* jet_1 = new xAOD::Jet();
		jet_1->makePrivateStore(*init_jet);
		
		const xAOD::JetFourMom_t jet_4mom_subtracted = init_jet->jetP4("JetSubtractedScaleMomentum");
		jet_1->setJetP4("JetConstitScaleMomentum", jet_4mom_subtracted);
		jet_1->setJetP4("JetEMScaleMomentum", jet_4mom_subtracted);//req in der

		const float constit_pt = 1e-3 * jet_4mom_subtracted.pt();
		const float constit_eta = jet_4mom_subtracted.eta();
		const float constit_phi = jet_4mom_subtracted.phi();
		const float constit_e = 1e-3*jet_4mom_subtracted.e();
		

		/*const xAOD::JetFourMom_t jet_4mom_constit = init_jet->jetP4("JetConstitScaleMomentum");
		const float constit_pt = 1e-3 * jet_4mom_constit.pt();
		const float constit_eta = jet_4mom_constit.eta();
		const float constit_phi = jet_4mom_constit.phi();
		const float constit_e = 1e-3*jet_4mom_constit.e();

		// for derivations, only unsubtracted and subtracted momenta are stored, so we need to set the constituent and EM scales(which are the same, respectively)
		
		jet_1->setJetP4("JetConstitScaleMomentum", jet_1->jetP4("JetUnsubtractedScaleMomentum"));
		jet_1->setJetP4("JetEMScaleMomentum", jet_1->jetP4("JetSubtractedScaleMomentum"));*/
		
		// set pre-EtaJES 4-momentum at the EM scale

		const xAOD::JetFourMom_t jet_4mom_em = jet_1->jetP4("JetEMScaleMomentum");
		jet_1->setJetP4("JetPileupScaleMomentum", jet_4mom_em);

		const float em_pt = 1e-3 * jet_4mom_em.pt();
		const float em_eta = jet_4mom_em.eta();
		const float em_phi = jet_4mom_em.phi();
		const float em_e = 1e-3 * jet_4mom_em.e();

		//apply EtaJES+Insitu calibration

		ANA_CHECK(m_Akt4HI_EtaJES_Insitu_CalibTool->applyCalibration(*jet_1));
		const xAOD::JetFourMom_t jet_4mom_etajes = jet_1->jetP4("JetEtaJESScaleMomentum");
		const float etajes_pt = 1e-3 * jet_4mom_etajes.pt();
		const float etajes_eta = jet_4mom_etajes.eta();
		const float etajes_phi = jet_4mom_etajes.phi();
		const float etajes_e = 1e-3 * jet_4mom_etajes.e();

		//apply insitu calibration to data

		if(!m_Collisions){
			jet_1->setJetP4("JetInsituScaleMomentum", jet_4mom_etajes);
		}

		const xAOD::JetFourMom_t jet_4mom_insitu = jet_1->jetP4("JetInsituScaleMomentum");
		const float xcalib_pt_insitu = 1e-3 * jet_4mom_insitu.pt ();
		const float xcalib_eta_insitu = jet_4mom_insitu.eta ();
		const float xcalib_phi_insitu = jet_4mom_insitu.phi ();
		const float xcalib_e_insitu = 1e-3 * jet_4mom_insitu.e ();

		if (xcalib_pt_insitu  < 15.) {
			delete jet_1;
			continue;
		}

		m_b_akt4hi_constit_jet_pt.push_back (constit_pt);
		m_b_akt4hi_constit_jet_eta.push_back (constit_eta);
		m_b_akt4hi_constit_jet_phi.push_back (constit_phi);
		m_b_akt4hi_constit_jet_e.push_back (constit_e);

		m_b_akt4hi_em_jet_pt.push_back (em_pt);
		m_b_akt4hi_em_jet_eta.push_back (em_eta);
		m_b_akt4hi_em_jet_phi.push_back (em_phi);
		m_b_akt4hi_em_jet_e.push_back (em_e);

		m_b_akt4hi_jet_etaJES_calib_pt.push_back (etajes_pt);
		m_b_akt4hi_jet_etaJES_calib_eta.push_back (etajes_eta);
		m_b_akt4hi_jet_etaJES_calib_phi.push_back (etajes_phi);
		m_b_akt4hi_jet_etaJES_calib_e.push_back (etajes_e);

		m_b_akt4hi_jet_Insitu_calib_pt.push_back (xcalib_pt_insitu);
		m_b_akt4hi_jet_Insitu_calib_eta.push_back (xcalib_eta_insitu);
		m_b_akt4hi_jet_Insitu_calib_phi.push_back (xcalib_phi_insitu);
		m_b_akt4hi_jet_Insitu_calib_e.push_back (xcalib_e_insitu);

		m_b_akt4hi_jet_n++;

		if (jet_1) delete jet_1;
	}// end jet loop  

	//get MC truth jets with R=0.4 from truth jet container

	if (!m_Collisions){

		const xAOD::JetContainer* truthJet4Container = 0;
		if(!evtStore()->retrieve(truthJet4Container, "AntiKt4TruthJets").isSuccess()) {
			Error("GetAntiKt4TruthJets()", "Failed to retrieve AntiKt4TruthJets collection. Exiting.");
			return EL::StatusCode::FAILURE;
		}
		for(const auto* truthJet : *truthJet4Container) {
			if(truthJet->pt()*1e-3 < 15)
				continue;

			m_b_akt4_truth_jet_pt.push_back(truthJet->pt()*1e-3);
			m_b_akt4_truth_jet_eta.push_back(truthJet->eta());
			m_b_akt4_truth_jet_phi.push_back(truthJet->phi());
			m_b_akt4_truth_jet_e.push_back(truthJet->e()*1e-3);

			m_b_akt4_truth_jet_n++;
		} //end truth jet loop
	} 

	//get photon from photon container

	const xAOD::PhotonContainer* photons = 0;
	if (!evtStore ()->retrieve (photons, "Photons").isSuccess ()) {
		Error ("GetPhotons ()", "Failed to retrieve Photons collection. Exiting." );
		return EL::StatusCode::FAILURE;
	}

	for (const auto* init_photon : *photons) {

		// Create calibrated copy of photon

		xAOD::Photon* photon = NULL;
		if (m_egammaCalibrationAndSmearingTool->correctedCopy (*init_photon, photon) != CP::CorrectionCode::Ok || !photon) {
			Error ("GetPhotons ()", "Photon unsuccessfully calibrated. Exiting.");
			return EL::StatusCode::FAILURE;
		}
		else{
			Info ("GetPhotons ()", "Successfully calibrated photon.");
		}


		const float photon_pt_precalib = init_photon->pt () * 1e-3;
		const float photon_pt = photon->pt () * 1e-3;
		const float photon_eta = photon->eta ();
		const float photon_phi = photon->phi ();
		const float photon_tight = m_photonTightIsEMSelector->accept (photon);
		const float photon_etcone30 = photon->auxdata<float> ("etcone30") * 1e-3;
		const float photon_topoetcone40 = photon->auxdata<float> ("topoetcone40") * 1e-3;
		
		// Photon kinematic cut
		if (photon_pt < 15.) {
			if (photon) {
				delete photon;
			}
		  continue;
		}

		m_b_photon_pt_precalib.push_back(photon_pt_precalib);
		m_b_photon_pt.push_back(photon_pt);
		m_b_photon_eta.push_back(photon_eta);
		m_b_photon_phi.push_back(photon_phi);

		//store photon identification info
		m_b_photon_tight.push_back(photon_tight);

		//store photon isolation info
		m_b_photon_etcone30.push_back(photon_etcone30);
		m_b_photon_topoetcone40.push_back(photon_topoetcone40);

		if(!m_Collisions){
			for (auto& systematic : photonSysList) {
				if (m_egammaCalibrationAndSmearingTool_sys->applySystematicVariation(systematic) != CP::SystematicCode::Ok) {
					std::cout << "Cannot configure calibration tool for systematics" << std::endl;
				}
				//xAOD::Photon* photonSysVar = nullptr;	

				//if (m_egammaCalibrationAndSmearingTool->correctedCopy (*photon, photonSysVar)  != CP::CorrectionCode::Ok) {
				//	Info ("GetPhotons ()", "Cannot apply systematic variation to photon energy scale. Continuing.");
				//	continue;
				//}
				if (!m_egammaCalibrationAndSmearingTool_sys->applyCorrection (*photon)){
					Info ("GetPhotons ()", "Cannot apply systematic variation to photon energy scale. Continuing.");
					continue;
				}
				//std::cout << " --> Calibrated pt with systematic " << systematic.name() << " = " << photon->pt() << std::endl;


				const float photon_pt_sys = photon->pt()*1e-3;
	
				if(systematic.name() == "EG_SCALE_ALL__1up") m_b_photon_pt_sys1.push_back(photon_pt_sys);
				if(systematic.name() == "EG_SCALE_ALL__1down") m_b_photon_pt_sys2.push_back(photon_pt_sys);
				if(systematic.name() == "EG_RESOLUTION_ALL__1up") m_b_photon_pt_sys3.push_back(photon_pt_sys);
				if(systematic.name() == "EG_RESOLUTION_ALL__1down") m_b_photon_pt_sys4.push_back(photon_pt_sys);
				//if(systematic.name() == "EG_SCALE_AF2__1up") m_b_photon_pt_new.push_back(photon_pt_sys);

				//if (photonSysVar) {
				//	delete photonSysVar;
				//}
			}
		}

		//std::cout << m_b_photon_pt.back() << ",  " << m_b_photon_pt_sys1.back() << ",  " << m_b_photon_pt_sys2.back() << ",  " << m_b_photon_pt_sys3.back() << ",  " << m_b_photon_pt_sys4.back() << std::endl; 
		
		//setting bool to be true 
		if ((photon_pt > 50.)&& (photon_tight==1)&& (photon_topoetcone40 < 5.))  {
			photon_min_pt_tight = true;
		}

		if (photon){
			delete photon;
		}

		m_b_photon_n ++;

	}
							
	//get truth photons from MC

	if(!m_Collisions){
		const xAOD::TruthParticleContainer* truthParticleContainer = 0;
		if (!evtStore ()->retrieve (truthParticleContainer, "TruthParticles").isSuccess ()) {
			Error ("GetTruthPhotons ()", "Failed to retrieve TruthParticles container. Exiting.");
			return EL::StatusCode::FAILURE;
		}
		for (const auto* truthParticle : *truthParticleContainer) {
			if (truthParticle->status() != 1) {
				continue; // if not final state continue
			}
			if (truthParticle->absPdgId () != 22) {
				continue; // check if photon
			}
			if (truthParticle->pt () * 1e-3 < 15.) {
				continue; // pT cut
			}
			if (fabs (truthParticle->eta()) > 3) {
				continue; // require photons inside EMCal
			}
			const float truth_photon_pt = truthParticle->pt () * 1e-3;
			const float truth_photon_eta = truthParticle->eta ();
			const float truth_photon_phi = truthParticle->phi ();

			m_b_truth_photon_pt.push_back(truth_photon_pt);
			m_b_truth_photon_eta.push_back(truth_photon_eta);
			m_b_truth_photon_phi.push_back(truth_photon_phi);
			m_b_truth_photon_n ++;
		}
		
	}

	//get truth event information

	if(!m_Collisions){
		m_b_mcEventWeights = eventInfo->mcEventWeights ();
	}

	//get vertex info
	const xAOD::VertexContainer* primaryVertices = 0;
	if(!evtStore()->retrieve(primaryVertices, "PrimaryVertices").isSuccess()) {
		Error("GetPrimaryVertices()", "Failed to retrieve PrimaryVertices container. Exiting.");
		return EL::StatusCode::FAILURE;
	}
	m_b_nvert = primaryVertices->size();
	for(const auto* vert : *primaryVertices) {
		m_b_vert_x.push_back(vert->x());
		m_b_vert_y.push_back(vert->y());
		m_b_vert_z.push_back(vert->z());
		m_b_vert_type.push_back(vert->vertexType());
		m_b_vert_ntrk.push_back(vert->nTrackParticles());
	}

	//triggers

	if (m_Collisions){		
		if (m_pp17){
			{
			auto chainGroup = m_trigDecisionTool->getChainGroup("HLT_g30_loose_L1EM15");
			HLT_g30_loose_L1EM15 = chainGroup->isPassed();
			} 
			{
			auto chainGroup = m_trigDecisionTool->getChainGroup("HLT_g35_loose_L1EM15");
			HLT_g35_loose_L1EM15 = chainGroup->isPassed();
			}
		}
		
		if (m_pp15){
				{	
				auto chainGroup = m_trigDecisionTool->getChainGroup("HLT_g30_loose");
				HLT_g30_loose = chainGroup->isPassed();
				} 
				{
				auto chainGroup = m_trigDecisionTool->getChainGroup("HLT_g30_loose_L1EM15");
				HLT_g30_loose_L1EM15 = chainGroup->isPassed();
				}
			}
		}
		
		if (m_Collisions){
			if (m_pp17){
				if ((HLT_g30_loose_L1EM15 || HLT_g35_loose_L1EM15) && photon_min_pt_tight){ 
					tree ("analysis")->Fill();
				}
			}
			
			if (m_pp15){
				if ((HLT_g30_loose || HLT_g30_loose_L1EM15) && photon_min_pt_tight){ 
				tree ("analysis")->Fill();
			}
		}
	}
	if (!m_Collisions){
		if (photon_min_pt_tight){
			tree ("analysis")->Fill();
		}
	}

	return StatusCode::SUCCESS;
}



StatusCode MyxAODAnalysis :: finalize (){
	// This method is the mirror image of initialize(), meaning it gets
	// called after the last event has been processed on the worker node
	// and allows you to finish up any objects you created in
	// initialize() before they are written to disk.  This is actually
	// fairly rare, since this happens separately for each worker node.
	// Most of the time you want to do your post-processing on the
	// submission node after all your histogram outputs have been
	// merged.

	//delete photon identification tools
	if (m_photonTightIsEMSelector) {
		delete m_photonTightIsEMSelector;
		m_photonTightIsEMSelector = nullptr;
	}

	//delete GRL tool
	delete m_grl;
	m_grl = nullptr;

	//delete jet calibration tools
	if (m_Akt10HI_EtaJES_Insitu_CalibTool) {
		delete m_Akt10HI_EtaJES_Insitu_CalibTool;
		m_Akt10HI_EtaJES_Insitu_CalibTool = nullptr;
	}
	if (m_Akt4HI_EtaJES_Insitu_CalibTool) {
		delete m_Akt4HI_EtaJES_Insitu_CalibTool;
		m_Akt4HI_EtaJES_Insitu_CalibTool = nullptr;
	}
	if (m_egammaCalibrationAndSmearingTool) {
		delete m_egammaCalibrationAndSmearingTool;
	        m_egammaCalibrationAndSmearingTool = nullptr;
	}
	if (m_egammaCalibrationAndSmearingTool_sys) {
		delete m_egammaCalibrationAndSmearingTool_sys;
	        m_egammaCalibrationAndSmearingTool_sys = nullptr;
	}

	return StatusCode::SUCCESS;
}

