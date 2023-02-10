{

     TFile *fi = new TFile("/home/wcte/data_root/root_run_000103_0000.root");

     TH1D *waveforms[16];
     int timestamp;
     int serialnumber;
     int freqsetting;

     TH1D *lgc_amp = new TH1D("lgc_amp","Lead Glass Amplitude",100,0,0.1);

     TTree *midas_data = (TTree*)fi->Get("midas_data");
     midas_data->SetBranchAddress("timestamp",&timestamp);
     midas_data->SetBranchAddress("serialnumber",&serialnumber);
     midas_data->SetBranchAddress("freqsetting",&freqsetting);
     for(int i=0; i<16; i++) midas_data->SetBranchAddress(Form("Channel%d",i),&(waveforms[i]));


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
