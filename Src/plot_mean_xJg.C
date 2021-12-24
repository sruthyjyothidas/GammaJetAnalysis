void plot_mean_xJg(){

  bool sherpamc = 0;
  const char* mc;
  if (sherpamc) {
    mc = "SHERPA MC";
  }else{
    mc = "PYTHIA8 MC";
  }
  int Nfiles = 2;
  const char* fmc = "MC";
  const char* fdata = "data17nocalib";
  TFile* fin = new TFile(Form("../RootFiles/GammaJetPar_%s_%s.root",fdata,fmc),"READ");

  const int Neta = 5;
  const int NpT = 12;
  const string etacut[Neta] = {"_00_03", "_03_08", "_08_12", "_12_21","_00_21"};
  const string eta_l[Neta] = {"","0.3 <","0.8 <","1.2 <",""};
  const string eta_u[Neta] = {"< 0.3","< 0.8","< 1.2","< 2.1","< 2.1"};
  const string pTcut[NpT] = {"_50_60","_60_70", "_70_85","_85_100","_100_125","_125_150","_150_180","_180_210","_210_240","_240_280","_280_350","_350_500"};

  TH1F* h_mean_MC[Neta];
  TH1F* h_mean_data[Neta];
  TH1F* h_meanerr_MC[Neta];
  TH1F* h_meanerr_data[Neta];
    
  float pT[NpT+1] = {50,60,70,85,100,125,150,180,210,240,280,350,500};
  TH1F* xJg_data = new TH1F("xJg_data", "", NpT, pT);
  TH1F* xJg_MC = new TH1F("xJg_MC", "", NpT, pT);

  for(int j = 0; j < Neta; j++){
    
    h_mean_MC[j] = (TH1F*)fin->Get(Form("h_mean_MC%s",etacut[j].c_str()));
    h_meanerr_MC[j] = (TH1F*)fin->Get(Form("h_meanerr_MC%s",etacut[j].c_str()));
    h_mean_data[j] = (TH1F*)fin->Get(Form("h_mean_data%s",etacut[j].c_str()));
    h_meanerr_data[j] = (TH1F*)fin->Get(Form("h_meanerr_data%s",etacut[j].c_str()));
       
    TGraph* line0 = new TGraph(2);
    line0->SetPoint(0,-100,1);
    line0->SetPoint(1,500,1);
    line0->SetLineStyle(2);
    line0->SetLineColor(1);
    line0->SetLineWidth(1);
   
    TCanvas* c1 = new TCanvas("c1","New Canvas",50,50,600,500);
    TPad *pad1 = new TPad("pad1", "",0.,0.5,1.0,1.0);
    TPad *pad2 = new TPad("pad2", "",0.0,0.0,1.0,0.5);
    pad1->Draw();
    pad2->Draw();
    pad1->cd();
    pad1->SetBottomMargin(0.01);
    TH1F *h_frame = pad1->DrawFrame(59,0.85,501,1.29);
    h_frame->GetYaxis()->SetLabelSize(0.1);
    h_frame->GetYaxis()->SetTitle("#it{<X_{J_{g}}>}");
    h_frame->GetYaxis()->SetTitleSize(0.1);
    h_frame->GetYaxis()->SetTitleOffset(0.5);
    h_frame->GetXaxis()->SetLabelOffset(999);
    h_frame->GetXaxis()->SetTickLength(0.05);
    h_frame->GetYaxis()->SetNdivisions(506,kTRUE);
    h_frame->GetXaxis()->SetNdivisions(509,kTRUE);
    h_frame->Draw("AXIS");
    
    for(int k = 0; k < NpT; k++){
      xJg_MC->SetBinContent(k+1,h_mean_MC[j]->GetBinContent(k+1));
      xJg_MC->SetBinError(k+1,h_meanerr_MC[j]->GetBinContent(k+1));
      xJg_data->SetBinContent(k+1,h_mean_data[j]->GetBinContent(k+1));
      xJg_data->SetBinError(k+1,h_meanerr_data[j]->GetBinContent(k+1));
    }
    
    gStyle->SetErrorX();

    xJg_MC->SetMarkerColor(1);
    xJg_MC->SetLineColor(1);
    xJg_data->SetMarkerColor(2);
    xJg_data->SetLineColor(2);
    xJg_MC->Draw("PSAME");
    xJg_data->Draw("PSAME");
    
    auto legend1 = new TLegend(0.5,0.7,0.65,0.9);
    legend1->SetBorderSize(0);
    legend1->SetTextSize(0.08);
    legend1->SetTextFont(42);
    legend1->SetMargin(0.8);
    legend1->AddEntry(xJg_MC, mc,"lp");
    legend1->AddEntry(xJg_data,"data: pp 2017 5.02 TeV","lp");
    legend1->Draw();

    auto legend10 = new TLegend(0.2,0.6,0.3,0.9);
    legend10->SetBorderSize(0);
    legend10->SetTextSize(0.08);
    legend10->SetTextFont(42);
    legend10->SetMargin(0.0);
    legend10->AddEntry((TObject*)0,Form("%s |#eta^{jet}| %s" ,eta_l[j].c_str(), eta_u[j].c_str()),"");
    legend10->AddEntry((TObject*)0,"anti-k_{t} R=1.0 HI Jets","");
    legend10->AddEntry((TObject*)0,"in situ #gamma+jet","");
    legend10->Draw();

    //ratio
    
    TH1F* h_ratio = (TH1F*)xJg_MC->Clone(Form("h_ratio"));
    h_ratio->Divide(xJg_data);
    pad2->cd();
    pad2->SetBottomMargin(0.25);

    TH1F *h_frame1; 
    if (j != Neta-1){
	    h_frame1 = pad2->DrawFrame(59,0.5,501,1.5);
    }else{
      h_frame1 = pad2->DrawFrame(59,0.9,501,1.1);
    }
    h_frame1->GetYaxis()->SetLabelSize(0.1);
    h_frame1->GetXaxis()->SetTickLength(0.07);
    h_frame1->GetXaxis()->SetLabelSize(0.1);

    h_frame1->GetXaxis()->SetTitle("photon p_{T}");
    h_frame1->GetYaxis()->SetTitle("MC/Data");
    h_frame1->GetXaxis()->SetTitleSize(0.09);
    h_frame1->GetYaxis()->SetTitleOffset(0.7);
    h_frame1->GetXaxis()->SetTitleOffset(1.0);
    h_frame1->GetYaxis()->SetTitleSize(0.1);

    h_frame1->GetYaxis()->SetNdivisions(508,kTRUE);
    h_frame1->GetXaxis()->SetNdivisions(509,kTRUE);
    h_frame1->Draw("AXIS");
    line0->Draw("SAME");

    h_ratio->SetMarkerColor(4);
    h_ratio->SetLineColor(4);
    h_ratio->SetLineWidth(4);
    h_ratio->Draw("PSAME");
    
    c1->SaveAs(Form("../Plots/ratio_R1%s_%s_%s.pdf",etacut[j].c_str(),fmc,fdata));
    c1->Close();
    delete c1;
  }
}
