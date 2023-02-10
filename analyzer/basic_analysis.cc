{

	TFile *fi = new TFile("/home/wcte/data_root/root_run_000065_0000.root");


	TH1D *waveforms[16];
	int timestamp;
        int serialnumber;
        int freqsetting;


	TTree *midas_data = (TTree*)fi->Get("midas_data");
	midas_data->SetBranchAddress("timestamp",&timestamp);
	midas_data->SetBranchAddress("serialnumber",&serialnumber);
	midas_data->SetBranchAddress("freqsetting",&freqsetting);
	for(int i=0; i<16; i++) midas_data->SetBranchAddress(Form("Channel%d",i),&(waveforms[i]));

	float freq[4] = {3.2,1.6,0.8,0.6};

	bool good_event = false;
	int stop_event = (int)midas_data->GetEntries();

	TH1D *tofDiff = new TH1D("tofDiff","",2000,-100,100);
	TH1D *tofDiffNoElec = new TH1D("tofDiffNoElec","",2000,-100,100);

	TH1D *actPeakElec = new TH1D("actPeakElec","",100,-0.02,0.1);
	TH1D *actPeakOther = new TH1D("actPeakOther","",100,-0.02,0.1);

	for(int i=0; i<stop_event; i++){
	  midas_data->GetEntry(i);
	  bool scint1 = false;
	  bool scint2 = false;
	  bool gct1Signal = false;
	  bool gct2Signal = false;
	  //std::cout << "Event " << i << std::endl;
	  double sci1Time = 0.;
	  double sci2Time = 0.;
	  double actPed = 0.;
	  int actPedBins = 0;
	  double actPeak = 4096.;
	  int actPeakBin = 0;
	  for(int j=2; j<=waveforms[1]->GetNbinsX(); j++){
	    //std::cout << waveforms[1]->GetBinContent(j) <<  " " << waveforms[2]->GetBinContent(j) << std::endl;
	    double sci1T1 = waveforms[1]->GetXaxis()->GetBinCenter(j);
	    double sci1T0 = waveforms[1]->GetXaxis()->GetBinCenter(j-1);
	    double sci1V1 = waveforms[1]->GetBinContent(j);
	    double sci1V0 = waveforms[1]->GetBinContent(j-1);
	    if(sci1V1<3540. && sci1V0>=3540.){
 	      scint1 = true;
	      double a = (sci1V1-sci1V0)/(sci1T1-sci1T0);
	      double b = sci1V1-a*sci1T0;
	      sci1Time = (3540.0-b)/a;
	      //std::cout << sci1Time << std::endl;
	    }  
	    double sci2T1 = waveforms[2]->GetXaxis()->GetBinCenter(j);
            double sci2T0 = waveforms[2]->GetXaxis()->GetBinCenter(j-1);
            double sci2V1 = waveforms[2]->GetBinContent(j);
            double sci2V0 = waveforms[2]->GetBinContent(j-1);
	    if(sci2V1<3350. && sci2V0>=3350.){
	      scint2 = true;
	      double a = (sci2V1-sci2V0)/(sci2T1-sci2T0);
              double b = sci2V1-a*sci2T0;
              sci2Time = (3350.0-b)/a;
              //std::cout << sci2Time << std::endl;
	    }   
	    if(waveforms[4]->GetBinContent(j)<3500 &&
	       waveforms[4]->GetXaxis()->GetBinCenter(j)>320.0 &&
	       waveforms[4]->GetXaxis()->GetBinCenter(j)<350.0 ) gct1Signal = true;
	    if(waveforms[3]->GetBinContent(j)<3540 &&
               waveforms[3]->GetXaxis()->GetBinCenter(j)>360.0 &&
               waveforms[3]->GetXaxis()->GetBinCenter(j)<390.0 ) gct2Signal = true;
	    if(waveforms[0]->GetXaxis()->GetBinCenter(j)<200.0) {
		    actPed += waveforms[0]->GetBinContent(j);
		    actPedBins++;
            }		    
	    if(waveforms[0]->GetXaxis()->GetBinCenter(j)>300.0 &&
	       waveforms[0]->GetXaxis()->GetBinCenter(j)<450.0 ) {
		    double value = waveforms[0]->GetBinContent(j);
		    if(value < actPeak){
                       actPeak = value;
		       actPeakBin = j;
		    }   
		   
            }		    

          }
	  actPed = actPed/(float)actPedBins;
	  //std::cout << actPed << " " << actPeak << std::endl;
	  actPeak = -1.0*(actPeak-actPed)*2.5/4096.0;
	  /*TF1 quad("quad","[0]*pow(x-[1],2)+[2]",waveforms[0]->GetXaxis()->GetBinLowEdge(actPeakBin-4),waveforms[0]->GetXaxis()->GetBinUpEdge(actPeakBin+4));
          quad.SetParameter(0,1.0);
	  quad.SetParameter(1,waveforms[0]->GetXaxis()->GetBinCenter(actPeakBin));
	  quad.SetParLimits(1,waveforms[0]->GetXaxis()->GetBinLowEdge(actPeakBin-4),waveforms[0]->GetXaxis()->GetBinUpEdge(actPeakBin+4));
	  quad.SetParameter(2,actPeak);
	  waveforms[0]->Fit("quad","R");
          double actPeakFit = quad.GetParameter(2);
	  actPeakFit = -1.0*(actPeakFit-actPed)*2.5/4096.0;*/

	  //std::cout << actPeak << std::endl;
          if(scint1 && scint2) {
             //do the analysis on channel 0
	     double ped = 0.;
	     double sig = 0.;
	    // for(int j=1; j<=waveforms[0]->GetNbinsX(); j++){
       // 	double btime = waveforms[0]->GetNbinsX();       
       //
             //if(sci1Time>320. && sci1Time<360. && sci2Time>340. && sci2Time<390.) tofDiff->Fill(sci2Time-sci1Time);
             tofDiff->Fill(sci2Time-sci1Time);
             //if(!(gct1Signal && gct2Signal) && sci1Time>340. && sci1Time<390. && sci2Time>320. && sci2Time<360.) tofDiffNoElec->Fill(sci2Time-sci1Time);
             if(!(gct1Signal || gct2Signal)) tofDiffNoElec->Fill(sci2Time-sci1Time);

	     if(gct1Signal && gct2Signal) actPeakElec->Fill(actPeak);
	     else if(!(gct1Signal || gct2Signal) && (sci2Time-sci1Time)<0.) actPeakOther->Fill(actPeak);


          }		  
        }   	  


	/*if(freqsetting<0 || freqsetting>3) std::cout << "Bad Frequency ID " << freqsetting << std::endl;
	else std::cout << "Frequency: " << freq[freqsetting] << " GHz" << std::endl;

	if(good_event){
	for(int i=0; i<5; i++){
		TCanvas *c = new TCanvas(Form("c%d",i),Form("c%d",i),600,500);
		waveforms[i]->Draw();
		c->Print(Form("Channel%d.png",i));
	}}*/
	TCanvas *c1 = new TCanvas("c1","c1",600,500);  
	tofDiff->Draw();
	TCanvas *c2 = new TCanvas("c2","c2",600,500);  
	actPeakOther->Draw();
	TCanvas *c3 = new TCanvas("c3","c3",600,500);  
	actPeakElec->Draw();
	TCanvas *c4 = new TCanvas("c4","c4",600,500);  
	tofDiffNoElec->Draw();

}





