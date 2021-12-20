#ifndef MyAnalysis_MyxAODAnalysis_H
#define MyAnalysis_MyxAODAnalysis_H

#include <AnaAlgorithm/AnaAlgorithm.h>
#include <iostream>

// Trigger                                                                                       
#include <TrigConfInterfaces/ITrigConfigTool.h>
#include <TrigDecisionTool/TrigDecisionTool.h>

// ROOT                                                                                                                           
#include <TH1.h>
#include <TTree.h>

#include "AsgTools/AnaToolHandle.h"

// std includes                                                                                  
#include <string>
#include <vector>
#include <map> 
#include <set>

// GRL
#include "GoodRunsLists/GoodRunsListSelectionTool.h"

// Jet calibration tool
#include "JetCalibTools/JetCalibrationTool.h"


// E/gamma tools                                                                                                                  
#include "ElectronPhotonSelectorTools/AsgPhotonIsEMSelector.h"
#include "ElectronPhotonFourMomentumCorrection/EgammaCalibrationAndSmearingTool.h"

// Track selection tool
#include "InDetTrackSelectionTool/InDetTrackSelectionTool.h"

// HI event shape access
#include "xAODHIEvent/HIEventShapeContainer.h"

// in the header
#include "PATInterfaces/SystematicVariation.h"
#include "PATInterfaces/SystematicRegistry.h"
#include "PATInterfaces/SystematicCode.h"

class MyxAODAnalysis : public EL::AnaAlgorithm
{
public:
	// this is a standard algorithm constructor
	MyxAODAnalysis (const std::string& name, ISvcLocator* pSvcLocator);

	// these are the functions inherited from Algorithm
	virtual StatusCode initialize () override;
	virtual StatusCode execute () override;
	virtual StatusCode finalize () override;

private:
	// Configuration, and any other types of variables go here.

	// Bool is set to true if running over data
	bool m_Collisions = true;
	bool m_pp17 = true;
	bool m_pp15 = false;
	bool m_der = false;

	TTree* mytree; //!

	//trigger tools                                                                                                                       
	asg::AnaToolHandle<Trig::TrigDecisionTool> m_trigDecisionTool;
	asg::AnaToolHandle<TrigConf::ITrigConfigTool> m_trigConfigTool;

	// Photon identification tools

	AsgPhotonIsEMSelector* m_photonTightIsEMSelector;

	//Photon systematics tool

	std::vector<CP::SystematicSet> photonSysList;
	
	// GRL tools

	GoodRunsListSelectionTool* m_grl;

	// EtaJES and In situ + cross calibration tool

	JetCalibrationTool* m_Akt10HI_EtaJES_Insitu_CalibTool;
	JetCalibrationTool* m_Akt4HI_EtaJES_Insitu_CalibTool;

	// EGamma calibration tool
	CP::EgammaCalibrationAndSmearingTool* m_egammaCalibrationAndSmearingTool; //!
	CP::EgammaCalibrationAndSmearingTool* m_egammaCalibrationAndSmearingTool_sys; //!

	// Tree branches         

	int m_b_runNumber; //!                                                                  
	int m_b_eventNumber; //!

	bool HLT_g30_loose = false; //
	bool HLT_g30_loose_L1EM15 = false; //
	bool HLT_g35_loose_L1EM15 = false; //

	// fcal energy                                                                         

	float m_b_fcalA_et; //!                                                  
	float m_b_fcalC_et; //!

	//R = 1.0 jets -- uncalibrated 

	int m_b_akt10hi_jet_n;
	std::vector<float> m_b_akt10hi_constit_jet_pt; //!                                     
	std::vector<float> m_b_akt10hi_constit_jet_eta; //!                                    
	std::vector<float> m_b_akt10hi_constit_jet_phi; //!                                     
	std::vector<float> m_b_akt10hi_constit_jet_e; //!  

	std::vector<float> m_b_akt10hi_em_jet_pt; //!                                     
	std::vector<float> m_b_akt10hi_em_jet_eta; //!                                     
	std::vector<float> m_b_akt10hi_em_jet_phi; //!                                     
	std::vector<float> m_b_akt10hi_em_jet_e; //!                                     

	//R = 1.0 jets -- calibrated

	std::vector<float> m_b_akt10hi_jet_Insitu_calib_pt; //!                                     
	std::vector<float> m_b_akt10hi_jet_Insitu_calib_eta; //!                                     
	std::vector<float> m_b_akt10hi_jet_Insitu_calib_phi; //!                                     
	std::vector<float> m_b_akt10hi_jet_Insitu_calib_e; //!                                     

	std::vector<float> m_b_akt10hi_jet_etaJES_calib_pt; //!                                     
	std::vector<float> m_b_akt10hi_jet_etaJES_calib_eta; //!                                     
	std::vector<float> m_b_akt10hi_jet_etaJES_calib_phi; //!                                     
	std::vector<float> m_b_akt10hi_jet_etaJES_calib_e; //!                                     

	//R = 1.0 jets -- MC truth

	int m_b_akt10_truth_jet_n;
	std::vector<float> m_b_akt10_truth_jet_pt; //!                                     
	std::vector<float> m_b_akt10_truth_jet_eta; //!                                    
	std::vector<float> m_b_akt10_truth_jet_phi; //!                                     
	std::vector<float> m_b_akt10_truth_jet_e; //!  
	
	std::vector<float> m_b_mcEventWeights; //!

	//R = 0.4 jets -- uncalibrated

	int m_b_akt4hi_jet_n;
	std::vector<float> m_b_akt4hi_constit_jet_pt; //!                                     
	std::vector<float> m_b_akt4hi_constit_jet_eta; //!                                    
	std::vector<float> m_b_akt4hi_constit_jet_phi; //!                                     
	std::vector<float> m_b_akt4hi_constit_jet_e; //!  

	std::vector<float> m_b_akt4hi_em_jet_pt; //!                                     
	std::vector<float> m_b_akt4hi_em_jet_eta; //!                                     
	std::vector<float> m_b_akt4hi_em_jet_phi; //!                                     
	std::vector<float> m_b_akt4hi_em_jet_e; //!                                     


	//R = 0.4 jets -- calibrated

	std::vector<float> m_b_akt4hi_jet_Insitu_calib_pt; //!                                     
	std::vector<float> m_b_akt4hi_jet_Insitu_calib_eta; //!                                     
	std::vector<float> m_b_akt4hi_jet_Insitu_calib_phi; //!                                     
	std::vector<float> m_b_akt4hi_jet_Insitu_calib_e; //!                                     

	std::vector<float> m_b_akt4hi_jet_etaJES_calib_pt; //!                                     
	std::vector<float> m_b_akt4hi_jet_etaJES_calib_eta; //!                                     
	std::vector<float> m_b_akt4hi_jet_etaJES_calib_phi; //!                                     
	std::vector<float> m_b_akt4hi_jet_etaJES_calib_e; //!                                     

	//R = 0.4 jets -- MC truth

	int m_b_akt4_truth_jet_n;
	std::vector<float> m_b_akt4_truth_jet_pt; //!                                     
	std::vector<float> m_b_akt4_truth_jet_eta; //!                                    
	std::vector<float> m_b_akt4_truth_jet_phi; //!                                     
	std::vector<float> m_b_akt4_truth_jet_e; //!  

	//--Un-Calibrated Photons 

	int m_b_photon_n;
	std::vector<float> m_b_photon_pt; //!                                     
	std::vector<float> m_b_photon_pt_new; //!                                     
	std::vector<float> m_b_photon_pt_precalib; //!                                     
	std::vector<float> m_b_photon_eta; //!                                    
	std::vector<float> m_b_photon_phi; //!                                     
	std::vector<float> m_b_photon_tight; //!                                     
	std::vector<float> m_b_photon_etcone30; //!                                     
	std::vector<float> m_b_photon_topoetcone40; //!                                     

	// MC truth photons

	int m_b_truth_photon_n;
	std::vector<float> m_b_truth_photon_pt; //!                                     
	std::vector<float> m_b_truth_photon_eta; //!                                    
	std::vector<float> m_b_truth_photon_phi; //!                                     

	// Photon systematics

	std::vector<float> m_b_photon_pt_sys1; //!
	std::vector<float> m_b_photon_pt_sys2; //!
	std::vector<float> m_b_photon_pt_sys3; //!
	std::vector<float> m_b_photon_pt_sys4; //!
	
	// Vertex info

	int m_b_nvert; 
	std::vector<float> m_b_vert_x;                                      
	std::vector<float> m_b_vert_y;                                      
	std::vector<float> m_b_vert_z;                                      
	std::vector<float> m_b_vert_type;                                      
	std::vector<float> m_b_vert_ntrk;                                      

};

#endif

