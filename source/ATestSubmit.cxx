#include <EventLoopAlgs/NTupleSvc.h>
#include <EventLoop/OutputStream.h>
#include <SampleHandler/ToolsDiscovery.h>

void ATestSubmit (const std::string& submitDir)
{
  // Set up the job for xAOD access:                                                                                                    
  xAOD::Init().ignore();

  // create a new sample handler to describe the data files we use                                                                     
  SH::SampleHandler sh;

  // scan for datasets in the given directory                                                                                           
  // this works if you are on lxplus, otherwise you'd want to copy over files                                                           
  // to your local machine and use a local path.  if you do so, make sure                                                               
  // that you copy all subdirectories and point this to the directory                                                                  
  // containing all the files, not the subdirectories.                                    
  // use SampleHandler to scan all of the subdirectories of a directory for particular MC single file:                                 
  //const char* inputFilePath = gSystem->ExpandPathName("/afs/cern.ch/work/s/srdas/data17_5TeV");
  //SH::ScanDir().filePattern("*AOD*.1").scan(sh,inputFilePath);

  //SH::scanRucio (sh, "data17_5TeV.00340718.physics_Main.merge.AOD.r11382_p3827_tid17838594_00/"); //pp 2017 Data: one data  
  
  //SH::scanRucio (sh, "data15_5TeV.00286474.physics_Main.recon.AOD.r11819/"); //pp 2015 Data: run 1
  //SH::scanRucio (sh, "data15_5TeV.00286411.physics_Main.recon.AOD.r11819/"); //pp 2015 Data: run 2
  //SH::scanRucio (sh, "data15_5TeV.00286367.physics_Main.recon.AOD.r11819/"); //pp 2015 Data: run 3
  //SH::scanRucio (sh, "data15_5TeV.00286364.physics_Main.recon.AOD.r11819/"); //pp 2015 Data: run 4
  //SH::scanRucio (sh, "data15_5TeV.00286361.physics_Main.recon.AOD.r11819/"); //pp 2015 Data: run 5
  //SH::scanRucio (sh, "data15_5TeV.00286282.physics_Main.recon.AOD.r11819/"); //pp 2015 Data: run 6
  
  //2015 pp merge dataset
  //SH::scanRucio (sh, "data15_5TeV.00286474.physics_Main.merge.AOD.r11819_p4120"); //pp 2015 Data: run 1
  //SH::scanRucio (sh, "data15_5TeV.00286411.physics_Main.merge.AOD.r11819_p4120"); //pp 2015 Data: run 2
  //SH::scanRucio (sh, "data15_5TeV.00286367.physics_Main.merge.AOD.r11819_p4120"); //pp 2015 Data: run 3
  //SH::scanRucio (sh, "data15_5TeV.00286364.physics_Main.merge.AOD.r11819_p4120"); //pp 2015 Data: run 4
  //SH::scanRucio (sh, "data15_5TeV.00286361.physics_Main.merge.AOD.r11819_p4120"); //pp 2015 Data: run 5
  //SH::scanRucio (sh, "data15_5TeV.00286282.physics_Main.merge.AOD.r11819_p4120"); //pp 2015 Data: run 6

  SH::scanRucio (sh, "data17_5TeV.periodM.physics_Main.PhysCont.AOD.pro23_v02/"); //pp Data: whole dataset  
  
  //SH::scanRucio (sh, "mc16_5TeV.423102.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP50_70.merge.AOD.e5094_s3238_r10441_r10210/"); //pp MC 5 TeV  DP:50_70
  //SH::scanRucio (sh, "mc16_5TeV.423103.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP70_140.merge.AOD.e5094_s3238_r10441_r10210/"); //pp MC 5 TeV  DP:70_140
  //SH::scanRucio (sh, "mc16_5TeV.423104.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP140_280.merge.AOD.e5094_s3238_r10441_r10210"); //pp MC 5 TeV  DP:140_280

  //SH::scanRucio (sh, "mc16_5TeV.42310*.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP*.deriv.DAOD_HION5.e5094_s3238_r10441_r10210_p4699"); //pp MC DAOD_HION5 5 TeV  DP:50_70
  //SH::scanRucio (sh, "mc16_5TeV.423102.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP50_70.deriv.DAOD_HION5.e5094_s3238_r10441_r10210_p4699"); //pp MC DAOD_HION5 5 TeV  DP:50_70
  //SH::scanRucio (sh, "mc16_5TeV.423103.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP70_140.deriv.DAOD_HION5.e5094_s3238_r10441_r10210_p4699_tid26402675_00/"); //pp MC DAOD_HION5 5 TeV  DP:70_140
  //SH::scanRucio (sh, "mc16_5TeV.423104.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP140_280.deriv.DAOD_HION5.e5094_s3238_r10441_r10210_p4699/"); //pp MC DAOD_HION5 5 TeV  DP:140_280
  //SH::scanRucio (sh, "mc16_5TeV.423105.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP280_500.deriv.DAOD_HION5.e5094_s3238_r10441_r10210_p4699/"); //pp MC DAOD_HION5 5 TeV  DP:280_500
  //SH::scanRucio (sh, "mc16_5TeV.423105.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP280_500.deriv.DAOD_HION5.e5094_s3238_r10441_r10210_p4699_tid26646360_00/"); //pp MC DAOD_HION5 5 TeV  DP:280_500 second sample
  
  //SH::scanRucio (sh, "mc16_5TeV.42015*.Sherpa_224_NNPDF30NNLO_SinglePhotonPt*_EtaFilter.deriv.DAOD_HION5.e6940_s3238_r10441_r10210_p4699/"); //Sherpa MC DAOD_HION5 pp 2017 5 TeV DP:50_70
  //SH::scanRucio (sh, "mc16_5TeV.420157.Sherpa_224_NNPDF30NNLO_SinglePhotonPt70_140_EtaFilter.deriv.DAOD_HION5.e6940_s3238_r10441_r10210_p4699_tid26849181_00/"); //Sherpa MC DAOD_HION5 pp 2017 5 TeV DP:70_140
  //SH::scanRucio (sh, "mc16_5TeV.420158.Sherpa_224_NNPDF30NNLO_SinglePhotonPt140_280_EtaFilter.deriv.DAOD_HION5.e6940_s3238_r10441_r10210_p4699_tid26849179_00/"); //Sherpa MC DAOD_HION5 pp 2017 5 TeV DP:140_280
  //SH::scanRucio (sh, "mc16_5TeV.420159.Sherpa_224_NNPDF30NNLO_SinglePhotonPt280_500_EtaFilter.deriv.DAOD_HION5.e6940_s3238_r10441_r10210_p4699_tid26849183_00/"); //Sherpa MC DAOD_HION5 pp 2017 5 TeV DP:280_500
  
  //SH::scanRucio (sh, "mc16_valid.423102.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP50_70.recon.AOD.e5094_s3552_r11867_tid26698144_00/"); //MC pp 2015 5 TeV validation  DP:50_70
  //SH::scanRucio (sh, "mc16_valid.423103.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP70_140.recon.AOD.e5094_s3552_r11867_tid26697998_00/"); //MC pp 2015 5 TeV validation  DP:70_140
  //SH::scanRucio (sh, "mc16_valid.423104.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP140_280.recon.AOD.e5094_s3552_r11867_tid26698012_00/"); //MC pp 2015 5 TeV validation  DP:140_280
  
  //SH::scanRucio (sh, "mc16_5TeV.423102.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP50_70.recon.AOD.e5094_s3552_r11867_tid26825651_00/"); //MC with pp 2015 5 TeV conditions DP:50_70
  //SH::scanRucio (sh, "mc16_5TeV.423103.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP70_140.recon.AOD.e5094_s3552_r11867_tid26825655_00/"); //MC with pp 2015 5 TeV conditions DP:70_140
  //SH::scanRucio (sh, "mc16_5TeV.423104.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP140_280.recon.AOD.e5094_s3552_r11867_tid26825659_00/"); //MC with pp 2015 5 TeV conditions DP:140_280
  //SH::scanRucio (sh, "mc16_5TeV.423105.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP280_500.recon.AOD.e5094_s3552_r11867_tid26825663_00/"); //MC with pp 2015 5 TeV conditions DP:280_500
  
  // set the name of the tree in our files                                                                                              
  // in the xAOD the TTree containing the EDM containers is "CollectionTree"                                                            
  sh.setMetaString("nc_tree", "CollectionTree");
  //sh.load ("output-ANALYSIS");

  // further sample handler configuration may go here                                                                                   
  // print out the samples we found                                                                                                    
  sh.print();
  // this is the basic description of our job                                                                                          
  EL::Job job;
  job.sampleHandler(sh); // use SampleHandler in this job                                                                               
  //  job.options()->setDouble (EL::Job::optMaxEvents, 500); // for testing purposes, limit to run over the first 500 events only!      
  //job.options()->setString( EL::Job::optSubmitDirMode, "unique-link");                                                                
  
  job.outputAdd (EL::OutputStream ("ANALYSIS"));
  //std::string output = "myOutput";
  //EL::OutputStream outputFile  (output);


  EL::AnaAlgorithmConfig alg;                                                                                                         
  alg.setType ("MyxAODAnalysis");                                                                                                  
  // set the name of the algorithm (this is the name use with                                                                          
  // messages)                                                                                                                         
  
  alg.setName ("AnalysisAlg");

  //MyJetxAODAnalysis *alg = new MyJetxAODAnalysis();
  //alg->m_isLocal = true;
  //alg->m_outputName = output;
  //alg->setName ("JetAnalysis");

  // later on we'll add some configuration options for our algorithm that go here                                                       

  job.algsAdd (alg);
  job.options()->setDouble (EL::Job::optRemoveSubmitDir,1);
  // make the driver we want to use:                                                                                                    
  // this one works by running the algorithm directly:                                                                                  
  //EL::DirectDriver driver;
  // we can use other drivers to run things on the Grid, with PROOF, etc.                                                               
  EL::PrunDriver driver;
  driver.options()->setString("nc_outputSampleName", "user.srdas.12182021.GammaR1Jet.%in:name[3]%.0000"); 
  //driver.options()->setString("nc_outputSampleName", "user.srdas.09162021.GammaJet.ppMC16_5TeV_DP280_500.0001"); 
  //driver.options()->setString("nc_outputSampleName", "user.srdas.10212021.GammaR1Jet.ppdata17_5TeV.0000"); 
  //driver.options()->setString("nc_outputSampleName", "user.srdas.09212021.GammaR4Jet.ppdata15_5TeV_run6.0000"); 
  driver.options ()->setString (EL::Job::optGridNFilesPerJob, "5");

  // process the job using the driver                                                                                                   
  driver.submitOnly (job, submitDir);
}

