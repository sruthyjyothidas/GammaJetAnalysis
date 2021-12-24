Int_t OptimalRebin(TH1F* h){
  int method=1;
  double N=h->GetEffectiveEntries();
  double optWidth = 3.5*h->GetRMS()/TMath::Power(N,1.0/3);//3.5
  int Nbins=h->GetNbinsX();
  double range=h->GetBinLowEdge(Nbins+1)-h->GetBinLowEdge(1);
  int rebin=1;
  double prevWidth=range/Nbins;
  for (int i=1;i<Nbins;++i) {
    if (Nbins%i!=0) continue;
    double binWidth=range/Nbins*i;
    if (method==1) {
      // optimistic
      if (binWidth<optWidth) rebin=i;
      if (h->GetEntries() < 15) rebin = 1;
    } else if (method==2) {
      if (TMath::Abs(binWidth-optWidth) < TMath::Abs(prevWidth-optWidth)) rebin=i;
    }
    else rebin=i; // method 3
    if (binWidth>optWidth) break;
    prevWidth=binWidth;
  }
  return rebin;
}

void xJg_fit() {
  int Nfiles = 2;
  TFile* fin[Nfiles+1];
 
  bool sherpamc = 0;
  const char* mc;
  if (sherpamc) {
    mc = "SHERPA MC";
  }else{
    mc = "PYTHIA8 MC";
  }

  const char* fmc = "MC";
  const char* fdata = "data17";
  fin[0] = new TFile(Form("../RootFiles/GammaJet_%s.root",fmc),"READ");
  fin[1] = new TFile(Form("../RootFiles/GammaJet_%s.root",fdata),"READ");
  fin[2] = new TFile(Form("../RootFiles/GammaJetPar_%s_%s.root",fdata,fmc),"RECREATE");

  const int Neta = 5;
  const int NpT = 12;
  float pt[NpT+1] = {50,60,70,85,100,125,150,180,210,240,280,350,500};
  const string etacut[Neta] = {"_00_03", "_03_08", "_08_12", "_12_21", "_00_21"};
  const string eta_l[Neta] = {"","0.3 <","0.8 <","1.2 <", ""};
  const string eta_u[Neta] = {"< 0.3","< 0.8","< 1.2","< 2.1","< 2.1"};
  const string pTcut[NpT] = {"_50_60","_60_70","_70_85","_85_100","_100_125","_125_150","_150_180","_180_210","_210_240","_240_280","_280_350","_350_500"};
  const string pT[NpT] = {": [50,60]",": [60,70]",": [70,85]",": [85,100]",": [100,125]",": [125,150]",": [150,180]",": [180,210]",": [210,240]",": [240,280]",": [280,350]",": [350,500]"};
 
  //Defining histograms

  TH1F* h_xJg_MC[NpT];
  TH1F* h_xJg_data[NpT];
  TF1* h_fit_MC[NpT];
  TF1* h_fit_data[NpT];
  TH1F* h_mean_MC[Neta]; 
  TH1F* h_meanerr_MC[Neta]; 
  TH1F* h_mean_data[Neta];
  TH1F* h_meanerr_data[Neta];
  for( int j = 0; j < Neta; j++){
    h_mean_MC[j] = new TH1F(Form("h_mean_MC%s",etacut[j].c_str()), "", NpT, pt);
    h_meanerr_MC[j] = new TH1F(Form("h_meanerr_MC%s",etacut[j].c_str()), "", NpT, pt);
    h_mean_data[j] = new TH1F(Form("h_mean_data%s",etacut[j].c_str()), "", NpT, pt);
    h_meanerr_data[j] = new TH1F(Form("h_meanerr_data%s",etacut[j].c_str()), "", NpT, pt);
  }

  //Loop over pT bins and eta bins

  for(int j = 0; j < Neta; j++){
    for( int i = 1; i < NpT; i++){ 
      
      h_xJg_MC[i] = (TH1F*)fin[0]->Get(Form("h_JR10_xJg_g_pT_eta%s%s",pTcut[i].c_str(),etacut[j].c_str()));
      h_xJg_data[i] = (TH1F*)fin[1]->Get(Form("h_JR10_xJg_g_pT_eta%s%s",pTcut[i].c_str(),etacut[j].c_str()));
      
      int counts = h_xJg_data[i]->GetEntries(); 
      cout << "counts in data is " <<  counts << endl;;

      int rebin = OptimalRebin(h_xJg_data[i]);
      cout << "rebin is " <<  rebin << endl;

      h_xJg_data[i]->Rebin(rebin); //rebinning is done according to the function OptimalRebin defined above (taken from Insitu group)
      h_xJg_MC[i]->Rebin(rebin);

      TF1* h_fit_MC0;
      TF1* h_fit_data0;
      
      h_xJg_MC[i]->Fit("gaus","0Q","",0.0,4.0); //Gaussian fitting done on MC and data
      h_xJg_data[i]->Fit("gaus","0QL","",0.0,4.0);
      
      //Restricting range of fitting

      double Nsigma = 2.0;
      h_fit_MC0 = (TF1*)h_xJg_MC[i]->GetListOfFunctions()->FindObject("gaus");
      double minrangeMC = h_fit_MC0->GetParameter(1) - Nsigma*h_fit_MC0->GetParameter(2);
      double maxrangeMC = h_fit_MC0->GetParameter(1) + Nsigma*h_fit_MC0->GetParameter(2);

      h_fit_data0 = (TF1*)h_xJg_data[i]->GetListOfFunctions()->FindObject("gaus");
      double minrangedata = h_fit_data0->GetParameter(1) - Nsigma*h_fit_data0->GetParameter(2);
      double maxrangedata = h_fit_data0->GetParameter(1) + Nsigma*h_fit_data0->GetParameter(2); 

      // Plot-style features

      TCanvas* c1 = new TCanvas("c1","New Canvas",50,50,700,600);
      TPad* thePad = (TPad*)c1->cd();
      TH1F *h_frame = thePad->DrawFrame(0,0,2.0,1.6*h_xJg_data[i]->GetMaximum()+4);
      h_frame->GetXaxis()->SetTitle("xJg for Jets with R=1.0");
      h_frame->GetYaxis()->SetNdivisions(506,kTRUE);
      h_frame->GetXaxis()->SetNdivisions(509,kTRUE);
      h_frame->Draw("AXIS");
      
      h_xJg_MC[i]->SetLineColor(1);
      h_xJg_data[i]->SetLineColor(2);
      h_xJg_MC[i]->SetMarkerColor(1);
      h_xJg_MC[i]->SetMarkerSize(1.0);
      h_xJg_data[i]->Draw("same");

      TF1* g1 = new TF1("g1","gaus");
      g1->SetLineColor(1);
      g1->SetLineWidth(5);
      TF1* g2 = new TF1("g2","gaus");
      g2->SetLineColor(2);
      g2->SetLineWidth(5);
      g2->SetParameters(h_xJg_data[i]->GetMaximum(),h_xJg_data[i]->GetMean(),h_xJg_data[i]->GetRMS());
      g2->SetParLimits(2,0.,1.0);

      h_xJg_MC[i]->Scale(h_xJg_data[i]->Integral()/h_xJg_MC[i]->Integral());
      h_xJg_MC[i]->Draw("same");

      //Gaussian fitting done again with the restricted range; (number of entries in data should be greater than 10)

      if (counts > 10){
        h_xJg_MC[i]->Fit(g1,"same","",minrangeMC,maxrangeMC);
        h_xJg_data[i]->Fit(g2,"RSLsame","",minrangedata,maxrangedata);//,"",datarange[0],datarange[1]);

        h_fit_MC[i] = (TF1*)h_xJg_MC[i]->GetListOfFunctions()->FindObject("g1");
        h_fit_data[i] = (TF1*)h_xJg_data[i]->GetListOfFunctions()->FindObject("g2");
        
        h_mean_MC[j]->SetBinContent(i+1, h_fit_MC[i]->GetParameter(1)); //collecting the parameter mean
        h_meanerr_MC[j]->SetBinContent(i+1, h_fit_MC[i]->GetParError(1)); // collecting the error on parameter mean
        h_mean_data[j]->SetBinContent(i+1, h_fit_data[i]->GetParameter(1));
        h_meanerr_data[j]->SetBinContent(i+1, h_fit_data[i]->GetParError(1));
      } 
      
      auto legend = new TLegend(0.65,0.75,0.93,0.92);
      legend->SetBorderSize(0);
      legend->SetTextSize(0.04);
      legend->SetTextFont(42);
      legend->AddEntry(h_xJg_MC[i], mc,"ep");
      legend->AddEntry(g1, "Fit on MC","l");
      legend->AddEntry(h_xJg_data[i],"data: pp 2017","f");
      if (counts>10) legend->AddEntry(g2, "Fit on data","l");
      legend->Draw();
      
      myText( 0.18,0.8,1,Form("Counts in data is %d",counts));
      myText( 0.18,0.9,1,Form("photon p_{T} %s GeV",pT[i].c_str()));
      myText( 0.18,0.85,1,Form("%s |#eta^{jet}| %s" ,eta_l[j].c_str(), eta_u[j].c_str()));
      c1->SaveAs(Form("../Plots/xJg_R1%s%s_%s_%s.pdf",pTcut[i].c_str(),etacut[j].c_str(),fmc,fdata));
      c1->Close();
      delete c1;
    }  
  }
  
  //Saving the fit parameters to a new file

  fin[2]->cd();
  for(int j = 0; j < Neta; j++){
    h_mean_MC[j]->Write();
    h_mean_data[j]->Write();
    h_meanerr_MC[j]->Write();
    h_meanerr_data[j]->Write();
  }
  fin[2]->Close();  

}
