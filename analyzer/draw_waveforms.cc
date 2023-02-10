{

	TFile *fi = new TFile("/home/wcte/data_root/root_run_000103_0000.root");


	TH1D *waveforms[16];
	int timestamp;
        int serialnumber;
        int freqsetting;
	int entry = 1833;


	TTree *midas_data = (TTree*)fi->Get("midas_data");
	midas_data->SetBranchAddress("timestamp",&timestamp);
	midas_data->SetBranchAddress("serialnumber",&serialnumber);
	midas_data->SetBranchAddress("freqsetting",&freqsetting);
	for(int i=0; i<16; i++) midas_data->SetBranchAddress(Form("Channel%d",i),&(waveforms[i]));


	midas_data->GetEntry(entry);

	TCanvas *canv[16];
	TCanvas *canvas = new TCanvas("blah","blah",1500,1000);
	canvas->Divide(4,4);

	for(int i=0; i<16; i++){
           if( i!=8  ) continue;
	   //canv[i] = new TCanvas(Form("c%d",i),Form("c%d",i),600,500);
	   canvas->cd(i+1);
	   waveforms[i]->Draw();
	}   
}





