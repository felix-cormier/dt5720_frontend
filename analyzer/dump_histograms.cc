{

	TFile *fi = new TFile("/home/wcte/data_root/root_run_000207.root");
        int event = 21550;

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

	midas_data->GetEntry(event);

	if(freqsetting<0 || freqsetting>3) std::cout << "Bad Frequency ID " << freqsetting << std::endl;
	else std::cout << "Frequency: " << freq[freqsetting] << " GHz" << std::endl;

	for(int i=0; i<5; i++){
		TCanvas *c = new TCanvas(Form("c%d",i),Form("c%d",i),600,500);
		waveforms[i]->Draw();
	}

}





