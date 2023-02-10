{

     TFile *fi = new TFile("/home/wcte/data_root/root_run_000103_0000.root");

     TH1D *waveforms[16];
     int timestamp1;
     int serialnumber1;
     int freqsetting1;

     TGraph *ampDiff = new TGraph();

     TTree *midas_data1 = (TTree*)fi->Get("midas_data1");
     midas_data1->SetBranchAddress("timestamp",&timestamp);
     midas_data1->SetBranchAddress("serialnumber",&serialnumber);
     midas_data1->SetBranchAddress("freqsetting",&freqsetting);
     for(int i=0; i<8; i++) midas_data->SetBranchAddress(Form("Channel%d",i),&(waveforms[i]));


     for(int i=0; i<(int)midas_data->GetEntries(); i++){
       midas_data->GetEntry(i);
       TH1D *lgc = waveforms[6];
       double ped = 0.;
       double maxHeight = 5000.;
       for(int i=1; i<=lgc->GetNbinsX(); i++){
         if(i<=20) ped += lgc->GetBinContent(i);
         if(lgc->GetBinContent(i)<maxHeight) maxHeight = lgc->GetBinContent(i);
       }
       ped = ped/20.;
       std::cout << "LGC Pulse Height " << i << " " << -1.*(maxHeight-ped)*2.5/4096. << " " << maxHeight << " " << ped << std::endl;
       lgc_amp->Fill( -1.*(maxHeight-ped)*2.5/4096.); 
     }

     lgc_amp->Draw();


}
