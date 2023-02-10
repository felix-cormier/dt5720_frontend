{

     TFile *fi = new TFile("/home/wcte/data_root/root_run_000089_0000.root");

     TH1D *waveforms[8];
     int timestamp;
     int serialnumber;
     int freqsetting;

     TH1D *act0_amp = new TH1D("act0_amp","ACT Amplitude",200,0,0.2);

     TTree *midas_data = (TTree*)fi->Get("midas_data1");
     midas_data->SetBranchAddress("timestamp",&timestamp);
     midas_data->SetBranchAddress("serialnumber",&serialnumber);
     midas_data->SetBranchAddress("freqsetting",&freqsetting);
     for(int i=0; i<8; i++) midas_data->SetBranchAddress(Form("Channel%d",i),&(waveforms[i]));

     int max = (int)midas_data->GetEntries();
     if(max>10000) max = 10000;

     for(int i=0; i<max; i++){
       midas_data->GetEntry(i);
       TH1D *act00 = waveforms[0];
       TH1D *act01 = waveforms[1];
       double ped0 = 0.;
       double maxHeight0 = 5000.;
       double ped1 = 0.;
       double maxHeight1 = 5000.;
       std::cout << act00->GetNbinsX() << std::endl;
       for(int i=1; i<=act00->GetNbinsX(); i++){
         if(i<=20) ped0 += act00->GetBinContent(i);
         if(act00->GetBinContent(i)<maxHeight0) maxHeight0 = act00->GetBinContent(i);
       }
       for(int i=1; i<=act01->GetNbinsX(); i++){
         if(i<=20) ped1 += act01->GetBinContent(i);
         if(act01->GetBinContent(i)<maxHeight1) maxHeight1 = act01->GetBinContent(i);
       }
       ped0 = ped0/20.;
       ped1 = ped1/20.;
       act0_amp->Fill( -1.*(maxHeight0-ped0)*2.5/4096.-1.*(maxHeight1-ped1)*2.5/4096.);
     }

     act0_amp->Draw();

}     

