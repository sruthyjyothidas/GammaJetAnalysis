void GammaJetAnalysis(){
  
  float pi = TMath::Pi();
  bool data = 0; //if data running over data; if !data running over MC. 
  bool calib = 0; //1->calibration applied on data; 0-> No calibration applied on data
  bool sherpa = 0; // turn it on when running over sherpa sample
  const char* gen;
  int Nfiles;
  if (data) {
    if (calib) gen = "data17";
    else gen = "data17nocalib";
    Nfiles = 1;
  }else{
    if (sherpa) gen = "MCsherpa";
    else gen = "MC";
    Nfiles = 4;
  }

  bool syst_jetpTmin = 0;
  bool syst_jetpTratio = 0;
  bool syst_deltaphi = 0;
  bool syst_purity = 0;
  bool syst_sys1 = 0;
  bool syst_sys2 = 0;
  bool syst_sys3 = 0;
  bool syst_sys4 = 0;

  double mingpT = 60;
  double jetpT_min = 20;//18 20 25
  double jetpT_ratio = 0.20;//0.18 0.20 0.22
  double deltaphi = 3*pi/4;//2pi/3 3pi/4 5pi/6
  double purity = 1;// -0.5,1,0.5 change line
  
  TFile* f[Nfiles];
  TFile* fout;
  
  //Input files data (w/wo calib), MC(PYTHIA/SHERPA)

  if (data){
    f[0] = new TFile("../../user.srdas.11032021.GammaR1Jet.physics_Main.0000_ANALYSIS.root/user.srdas.ANALYSIS.root"); //pp 17 with calibration factors
  }else{
    if (sherpa){
      f[0] = new TFile("../../user.srdas.10262021.GammaR1Jet.ppsherpaMC16.Sherpa_224_NNPDF30NNLO_SinglePhotonPt50_70_EtaFilter.0000_ANALYSIS.root/user.srdas.ANALYSIS.root");
      f[1] = new TFile("../../user.srdas.10262021.GammaR1Jet.ppsherpaMC16.Sherpa_224_NNPDF30NNLO_SinglePhotonPt70_140_EtaFilter.0000_ANALYSIS.root/user.srdas.ANALYSIS.root");
      f[2] = new TFile("../../user.srdas.10262021.GammaR1Jet.ppsherpaMC16.Sherpa_224_NNPDF30NNLO_SinglePhotonPt140_280_EtaFilter.0000_ANALYSIS.root/user.srdas.ANALYSIS.root");
      f[3] = new TFile("../../user.srdas.10262021.GammaR1Jet.ppsherpaMC16.Sherpa_224_NNPDF30NNLO_SinglePhotonPt280_500_EtaFilter.0000_ANALYSIS.root/user.srdas.ANALYSIS.root");
    }else{
      f[0] = new TFile("../../user.srdas.11112021.GammaR1Jet.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP50_70.0000_ANALYSIS.root/user.srdas.ANALYSIS.root");
      f[1] = new TFile("../../user.srdas.11112021.GammaR1Jet.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP70_140.0000_ANALYSIS.root/user.srdas.ANALYSIS.root");
      f[2] = new TFile("../../user.srdas.11112021.GammaR1Jet.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP140_280.0000_ANALYSIS.root/user.srdas.ANALYSIS.root");
      f[3] = new TFile("../../user.srdas.11112021.GammaR1Jet.Pythia8EvtGen_A14NNPDF23LO_gammajet_DP280_500.0000_ANALYSIS.root/user.srdas.ANALYSIS.root");
    }
  }
  
  //Output files for data (w/wo calib), MC(PYTHIA/SHERPA), systematics

  if (syst_jetpTmin){
    fout = new TFile(Form("../RootFiles/GammaJet_%sjetpTmin_%.f.root",gen,jetpT_min),"RECREATE");
  }else if (syst_jetpTratio){
    fout = new TFile(Form("../RootFiles/GammaJet_%sjetpTratio_%.2f.root",gen,jetpT_ratio),"RECREATE");
  }else if (syst_deltaphi){
    fout = new TFile(Form("../RootFiles/GammaJet_%sdeltaphi_%.2f.root",gen,deltaphi),"RECREATE");
  }else if (syst_purity){
    fout = new TFile(Form("../RootFiles/GammaJet_%spurity_%.1f.root",gen,purity),"RECREATE");
  }else if (syst_sys1){
    fout = new TFile("../RootFiles/GammaJet_MCsys1.root","RECREATE");
  }else if (syst_sys2){
    fout = new TFile("../RootFiles/GammaJet_MCsys2.root","RECREATE");
  }else if (syst_sys3){
    fout = new TFile("../RootFiles/GammaJet_MCsys3.root","RECREATE");
  }else if (syst_sys4){
    fout = new TFile("../RootFiles/GammaJet_MCsys4.root","RECREATE");
  }else{
    fout = new TFile(Form("../RootFiles/GammaJet_%s.root",gen),"RECREATE");
  }
  
  vector<float> *akt10hi_jet_Insitu_calib_pt = 0;
  vector<float> *akt10hi_jet_Insitu_calib_eta = 0;
  vector<float> *akt10hi_jet_Insitu_calib_phi = 0;
  vector<float> *akt4hi_jet_Insitu_calib_pt = 0;
  vector<float> *akt4hi_jet_Insitu_calib_eta = 0;
  vector<float> *akt4hi_jet_Insitu_calib_phi = 0;
  vector<float> *photon_pt = 0;
  vector<float> *photon_pt_sys = 0;
  vector<float> *photon_eta = 0;
  vector<float> *photon_phi = 0;
  vector<float> *photon_topoetcone40 = 0;
  vector<float> *photon_tight = 0;
  vector<float> *mcEventWeights = 0;
   

  TTree* T[Nfiles];

  for (int i = 0; i < Nfiles; i++){
    T[i] = (TTree*)f[i]->Get("analysis");
    if (calib){
      T[i]->SetBranchAddress("akt10hi_jet_Insitu_calib_pt",&akt10hi_jet_Insitu_calib_pt);
      T[i]->SetBranchAddress("akt10hi_jet_Insitu_calib_eta",&akt10hi_jet_Insitu_calib_eta);
      T[i]->SetBranchAddress("akt10hi_jet_Insitu_calib_phi",&akt10hi_jet_Insitu_calib_phi);
      T[i]->SetBranchAddress("akt4hi_jet_Insitu_calib_pt",&akt4hi_jet_Insitu_calib_pt);
      T[i]->SetBranchAddress("akt4hi_jet_Insitu_calib_eta",&akt4hi_jet_Insitu_calib_eta);
      T[i]->SetBranchAddress("akt4hi_jet_Insitu_calib_phi",&akt4hi_jet_Insitu_calib_phi);
    }else{
      T[i]->SetBranchAddress("akt10hi_jet_etaJES_calib_pt",&akt10hi_jet_Insitu_calib_pt);
      T[i]->SetBranchAddress("akt10hi_jet_etaJES_calib_eta",&akt10hi_jet_Insitu_calib_eta);
      T[i]->SetBranchAddress("akt10hi_jet_etaJES_calib_phi",&akt10hi_jet_Insitu_calib_phi);
      T[i]->SetBranchAddress("akt4hi_jet_etaJES_calib_pt",&akt4hi_jet_Insitu_calib_pt);
      T[i]->SetBranchAddress("akt4hi_jet_etaJES_calib_eta",&akt4hi_jet_Insitu_calib_eta);
      T[i]->SetBranchAddress("akt4hi_jet_etaJES_calib_phi",&akt4hi_jet_Insitu_calib_phi);
    }
    T[i]->SetBranchAddress("photon_pt",&photon_pt);
    T[i]->SetBranchAddress("photon_eta",&photon_eta);
    T[i]->SetBranchAddress("photon_phi",&photon_phi);
    T[i]->SetBranchAddress("photon_topoetcone40",&photon_topoetcone40);
    T[i]->SetBranchAddress("photon_tight",&photon_tight);
    if (!data) T[i]->SetBranchAddress("mcEventWeights",&mcEventWeights);
    if (syst_sys1){
      T[i]->SetBranchAddress("photon_pt_sys1",&photon_pt);
    }
    if (syst_sys2){
      T[i]->SetBranchAddress("photon_pt_sys2",&photon_pt);
    }
    if (syst_sys3){
      T[i]->SetBranchAddress("photon_pt_sys3",&photon_pt);
    }
    if (syst_sys4){
      T[i]->SetBranchAddress("photon_pt_sys4",&photon_pt);
    }
  }

  //photon denoted by g. Jet of radius 1.0 denoted by JR1. Jet of radius 1.0 denoted by JR10.
  //Leading jet is the jet with highest pT denoted by LJ

  const int ptCut = 500;
  const int pt_interval = 2;
  const int Nbins_pt = ptCut / pt_interval;
  double pt_range[Nbins_pt+1];
  for (int i=0; i<Nbins_pt+1; i++) {
    pt_range[i] = i*pt_interval;
  }
  float eta_range[57];
  float eta_width = 0.2;
  for (int i=0; i<57; i++) {
    eta_range[i] = -5.6 + eta_width*i;
  }
  float phi_range[21];
  float phi_width = 2*pi/20.;
  for (int i=0; i<21; i++) {
    phi_range[i] = -pi + phi_width*i;
  }

  TH1D* h_JR10_pT = new TH1D("h_JR10_pT","pT", Nbins_pt, pt_range);
  TH1D* h_JR10_phi = new TH1D("h_JR10_phi","#phi", 20, phi_range);
  TH1D* h_JR10_eta = new TH1D("h_JR10_eta","#eta", 56, eta_range);

  TH1D* h_g_pT = new TH1D("h_g_pT","pT", Nbins_pt, pt_range);
  TH1D* h_g_phi = new TH1D("h_g_phi","#phi", 20, phi_range);
  TH1D* h_g_eta = new TH1D("h_g_eta","#eta", 56, eta_range);
  TH2D* h_g_pT_LJR10_pT_nocut = new TH2D("h_g_pT_LJR10_pT_nocut","", 80, 0, 500, 80, 0, 500);
  TH2D* h_g_pT_LJR10_pT = new TH2D("h_g_pT_LJR10_pT","", 80, 0, 500, 80, 0, 500);
  TH2D* h_g_pT_LJR10g_dphi = new TH2D("h_g_pT_LJR10g_dphi","", 100, 0, 500, 64, 0, 2*pi);

  const int Neta = 5;
  const int NpT = 12;
  const string etacut[Neta] = {"_00_03", "_03_08", "_08_12", "_12_21", "_00_21"};
  const string pTcut[NpT] = {"_50_60","_60_70", "_70_85","_85_100","_100_125","_125_150","_150_180","_180_210","_210_240","_240_280","_280_350","_350_500"};

  int Nbins = 250;
  TH1D* h_JR10_xJg_g_pT_eta[NpT][Neta];

  for(int i = 0; i < NpT; i++){
      for( int j = 0; j < Neta; j++){
        h_JR10_xJg_g_pT_eta[i][j] = new TH1D(Form("h_JR10_xJg_g_pT_eta%s%s",pTcut[i].c_str(),etacut[j].c_str()),"", Nbins, 0, 4);
      }
  }

  // weights for MC
  
    vector<float> cross_sec;
    vector<float> filter_eff;
    vector<float> total_events;
    if (sherpa){
      cross_sec = {2.47, 0.895, 0.0476, 0.00142};
      filter_eff = {0.82189, 0.89328, 0.98239, 0.9999};
      total_events = {995000,993000,1000000,1000000};
    }else{
      cross_sec = {85898, 21551, 1044, 37.592};
      filter_eff = {0.00003339, 0.000045787, 0.000050981, 0.000043848};
      total_events = {1996000,1998000,1997000,2037200};
    }
  
  for(int dp = 0; dp < Nfiles; dp ++){
    int nevents = T[dp]->GetEntries();
    cout << "..........Running..........dp = " << dp << ".........." << nevents << endl;
    float wt;
    for(int ievent = 0; ievent < nevents; ievent++){
      T[dp]->GetEntry(ievent);
      if (data){
        wt = 1;
      }else{
        wt = cross_sec[dp] * filter_eff[dp] * mcEventWeights->at(0) / total_events[dp];
      }
      if (ievent%1000 == 0)
      cout << "------------------running event " << ievent << "------------------ weight = " << wt << endl;
      
      //Analysis

      for(int i = 0; i < photon_pt->size(); i++){
        float pT_g = photon_pt->at(i);
        float phi_g = photon_phi->at(i);
        float eta_g = photon_eta->at(i);
        float topoetcone40_g = photon_topoetcone40->at(i);
        float tight_g = photon_tight->at(i);

        //cuts on photon

        if (pT_g < mingpT) continue;// min pT cut
        if (syst_purity){
          if (topoetcone40_g >= (0.022*pT_g) + purity) continue; //only isolated photons
        }else{
          if (topoetcone40_g >= purity) continue; //only isolated photons
        }
        if (tight_g != 1) continue; //only tight id'ed photons
        if ((abs(eta_g) >= 1.37) && (abs(eta_g) <= 1.52)) continue; //excluding crack region

        //check if there is an R=0.4 jet with pT> 20 GeV
        // with 0.4 < delta R < 1.0 away from photon

        bool xJg_reject = false;

        for(int j = 0; j < akt4hi_jet_Insitu_calib_pt->size(); j++){
          float pT_R4 = akt4hi_jet_Insitu_calib_pt->at(j);
          float phi_R4 = akt4hi_jet_Insitu_calib_phi->at(j);
          float eta_R4 = akt4hi_jet_Insitu_calib_eta->at(j);

          //applying min pT cut on jet

          if (pT_R4 < jetpT_min) continue;

          float R4g_dphi = phi_R4 - phi_g;
          float R4g_deta = eta_R4 - eta_g;
          float delta_R4 = sqrt(TMath::Power(R4g_dphi,2)+TMath::Power(R4g_deta,2));

          if (delta_R4 >= 0.4 && delta_R4 <= 1.0) {
            xJg_reject =  true;
            break;
          }
        }
        if (xJg_reject == true) {
          //cout << ievent << "... this xJg is rejected " << endl;
          continue;
        }

        //xJGammas for R=1.0 jets; fnding the leading jet in the event; jet with highest pT; denoted by LJ

        float LJR10_pT = 0;
        float SLJR10_pT = 0;
        float LJR10_phi = 0;
        float LJR10g_dphi = 0;
        float LJR10_eta = 0;

        for(int j = 0; j < akt10hi_jet_Insitu_calib_pt->size(); j++){
          float pT_R10 = akt10hi_jet_Insitu_calib_pt->at(j);
          float phi_R10 = akt10hi_jet_Insitu_calib_phi->at(j);
          float eta_R10 = akt10hi_jet_Insitu_calib_eta->at(j);

          //applying min pT cut on jet

          if (pT_R10 < jetpT_min) continue;

          //removing jet "version" of photon

          float R10g_dphi = phi_R10 - phi_g;
          float R10g_deta = eta_R10 - eta_g;
          float delta_R10 = sqrt(TMath::Power(R10g_dphi,2)+TMath::Power(R10g_deta,2));
          if (delta_R10 <= 1.0) continue;

          //finding leading jet

          if(akt10hi_jet_Insitu_calib_pt->at(j) > LJR10_pT){
              LJR10_pT = akt10hi_jet_Insitu_calib_pt->at(j);
              LJR10_phi = akt10hi_jet_Insitu_calib_phi->at(j);
              LJR10_eta = akt10hi_jet_Insitu_calib_eta->at(j);
          }

          //finding subleading jet

          if(akt10hi_jet_Insitu_calib_pt->at(j) > SLJR10_pT){
              if(akt10hi_jet_Insitu_calib_pt->at(j) == LJR10_pT)
                continue;
              SLJR10_pT = akt10hi_jet_Insitu_calib_pt->at(j);
          }
        } //end of jet loop

        //finding dphi between leading jet and photon

        LJR10g_dphi = phi_g - LJR10_phi;
        while (LJR10g_dphi > pi) LJR10g_dphi -= 2*pi;
        while (LJR10g_dphi < -1*pi) LJR10g_dphi += 2*pi;

        //topological selection

        if (TMath::Abs(LJR10g_dphi) <= deltaphi) continue;

        //don't want to fill any zeros in the histograms

        if (LJR10_pT == 0 || LJR10_phi == 0 || LJR10g_dphi == 0) continue;

        h_g_pT_LJR10_pT_nocut->Fill(pT_g, LJR10_pT);

        //require p_T,2/p_T,g < 0.2

        float pT_ratio = SLJR10_pT/pT_g;

        if (pT_ratio >= jetpT_ratio) continue;

        //printing and making sure it's doing the right thing

        //cout << "leading jet pT = " << LJR10_pT << " leading jet phi = " << LJR10_phi << " leading jet and gamma: dphi = " << LJR10g_dphi << endl;

        //calculating xJg and filling histograms
        h_JR10_pT->Fill(LJR10_pT);
        h_JR10_phi->Fill(LJR10_phi);
        h_JR10_eta->Fill(LJR10_eta);
        h_g_pT->Fill(pT_g);
        h_g_eta->Fill(eta_g);
        h_g_phi->Fill(phi_g);
        
        float JR10_xJg = LJR10_pT/pT_g;

        h_g_pT_LJR10_pT->Fill(pT_g, LJR10_pT);
        while (LJR10g_dphi < 0) LJR10g_dphi += 2*pi; //putting everything in (-pi,0) to (pi,2pi)
        h_g_pT_LJR10g_dphi->Fill(pT_g, LJR10g_dphi);

        int pTg[NpT+1] = {50,60,70,85,100,125,150,180,210,240,280,350,500};
        float etag[Neta] = {0.0,0.3,0.8,1.2,2.1};

        for( int j = 0; j < Neta-1; j++){
          if ((abs(LJR10_eta) >= etag[j]) && (abs(LJR10_eta) < etag[j+1])){
            for(int i = 0; i < NpT; i++){
              if (pT_g >= pTg[i] && pT_g < pTg[i+1]) {
                  h_JR10_xJg_g_pT_eta[i][j]->Fill(JR10_xJg);
              }
            }
          }
        }
        //eta integrated from -2.1 to 2.1
        if (abs(LJR10_eta) <= 2.1){
          for(int i = 0; i < NpT; i++){
            if (pT_g >= pTg[i] && pT_g < pTg[i+1]) {
                h_JR10_xJg_g_pT_eta[i][4]->Fill(JR10_xJg);
            }
          }
        }
      }//end of photon loop
    }//end of event loop
  }//end of DP loop

  h_JR10_pT->GetXaxis()->SetTitle("akt10hi_constit_jet_pt");
  h_JR10_phi->GetXaxis()->SetTitle("akt10hi_constit_jet_phi");
  h_JR10_eta->GetXaxis()->SetTitle("akt10hi_constit_jet_eta");
  h_g_pT->GetXaxis()->SetTitle("photon_pt");
  h_g_pT->GetXaxis()->SetRangeUser(50,500);
  h_g_phi->GetXaxis()->SetTitle("photon_phi");
  h_g_eta->GetXaxis()->SetTitle("photon_eta");
  h_g_pT_LJR10_pT->GetXaxis()->SetTitle("photon pT");
  h_g_pT_LJR10_pT->GetYaxis()->SetTitle("highest jet pT with R = 1.0");
  h_g_pT_LJR10_pT_nocut->GetXaxis()->SetTitle("photon pT");
  h_g_pT_LJR10_pT_nocut->GetYaxis()->SetTitle("highest jet pT with R = 1.0");
  h_g_pT_LJR10g_dphi->GetXaxis()->SetTitle("photon pT");
  h_g_pT_LJR10g_dphi->GetYaxis()->SetTitle("photon phi - highest jet phi with R = 1.0");

  h_JR10_phi->SetMinimum(0);
  h_g_phi->SetMinimum(0);

  fout->cd();
  h_JR10_pT->Write();
  h_JR10_phi->Write();
  h_JR10_eta->Write();
  h_g_pT->Write();
  h_g_eta->Write();
  h_g_phi->Write();
  h_g_pT_LJR10_pT->Write();
  h_g_pT_LJR10_pT_nocut->Write();
  h_g_pT_LJR10g_dphi->Write();
  for(int j = 0; j < Neta; j++){
    for(int i = 0; i < NpT; i++){
      h_JR10_xJg_g_pT_eta[i][j]->Write();
    }
  }
  fout->Close();
}

