{

	TFile *fi = new TFile("/home/wcte/data_root/root_run_000262.root");
        int start_event=1500;
        int stop_event;


	TH1D *waveforms[8];
	int timestamp;
        int serialnumber;
        int freqsetting;

	TTree *midas_data = (TTree*)fi->Get("midas_data");
	midas_data->SetBranchAddress("timestamp",&timestamp);
	midas_data->SetBranchAddress("serialnumber",&serialnumber);
	midas_data->SetBranchAddress("freqsetting",&freqsetting);
	for(int i=0; i<8; i++) midas_data->SetBranchAddress(Form("Channel%d",i),&(waveforms[i]));

	float freq[4] = {3.2,1.6,0.8,0.6};

	bool good_event = false;
	stop_event = (int)midas_data->GetEntries();
	std::cout << start_event << " " << stop_event << std::endl;

	for(int i=start_event; i<stop_event; i++){
	  midas_data->GetEntry(i);
	  bool scint1 = false;
	  bool scint2 = false;
	  bool act = false;
	  std::cout << "Event " << i << std::endl;
	  for(int j=1; j<=waveforms[1]->GetNbinsX(); j++){
	    //std::cout << waveforms[1]->GetBinContent(j) <<  " " << waveforms[2]->GetBinContent(j) << std::endl;
	    if(waveforms[1]->GetBinContent(j)<3500) scint1 = true;
	    if(waveforms[2]->GetBinContent(j)<3500) scint2 = true;
	    if(waveforms[0]->GetBinContent(j)<3615) act = true;
          }
          if(scint1 && scint2 ) {
		  good_event = true;
		  break;
          }		  
        }   	  


	if(freqsetting<0 || freqsetting>3) std::cout << "Bad Frequency ID " << freqsetting << std::endl;
	else std::cout << "Frequency: " << freq[freqsetting] << " GHz" << std::endl;

	if(good_event){
	for(int i=0; i<5; i++){
		TCanvas *c = new TCanvas(Form("c%d",i),Form("c%d",i),600,500);
		waveforms[i]->Draw();
		c->Print(Form("Channel%d.png",i));
	}}

}





