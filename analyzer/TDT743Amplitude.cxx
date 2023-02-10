#include "TDT743Amplitude.h"

#include "TDT743RawData.hxx"
#include "TDirectory.h"


/// Reset the histograms for this canvas
TDT743Amplitude::TDT743Amplitude(){

  SetSubTabName("DT743 Amplitudes");
  SetUpdateOnlyWhenPlotted(true);
  
  CreateHistograms();
  FrequencySetting = -1;
}


void TDT743Amplitude::CreateHistograms(){

  // check if we already have histogramss.
  char tname[100];
  sprintf(tname,"DT743_%i",0);

  TH1D *tmp = (TH1D*)gDirectory->Get(tname);
  if (tmp) return;

  int fWFLength = 7; // Need a better way of detecting this...
  int numSamples = fWFLength / 1;
  
  // Otherwise make histograms
  clear();
  
  for(int i = 0; i < 16; i++){ // loop over 2 channels
    
    char name[200];
    char title[200];
    sprintf(name,"DT743_%i_ForAmp",i);
    
    sprintf(title,"DT743 Waveform for channel=%i For Amp.",i);	
    
    TH1D *tmp = new TH1D(name, title, numSamples, 0, fWFLength);
    tmp->SetXTitle("ns");
    tmp->SetYTitle("ADC value");
    
    push_back(tmp);
  }

  for(int i=0; i<16; i++){
    TH1D *tmp = new TH1D(Form("Channel%dAmplitude",i),Form("Channel %d Peak Amplitude",i),200,0,0.3);
    tmp->SetXTitle("Peak Amplitude (V)");
    tmp->SetYTitle("Number of Events");
    push_back(tmp);
  }   


  /*TH1D *tmp3 = new TH1D("LGCPH","Lead Glass Calorimeter Pulse Height",100,0,1);
  tmp3->SetXTitle("Pulse Height");
  tmp3->SetYTitle("Events");
  push_back(tmp3);*/

  std::cout << "TDT743Amplitude done init...... " << std::endl;
  FrequencySetting = -1;
}


void TDT743Amplitude::UpdateHistograms(TDataContainer& dataContainer){
  
  int eventid = dataContainer.GetMidasData().GetEventId();
  int timestamp = dataContainer.GetMidasData().GetTimeStamp();
  
  TDT743RawData *dt743 = dataContainer.GetEventData<TDT743RawData>("43FS");
  TDT743RawData *dt743_mod2 = dataContainer.GetEventData<TDT743RawData>("43F2");
  
  if(dt743){      
    
    
    std::vector<RawChannelMeasurement> measurements = dt743->GetMeasurements();

    bool changeHistogram = false; 
    for(int i = 0; i < measurements.size(); i++){
           
      int chan = measurements[i].GetChannel();
      int nsamples = measurements[i].GetNSamples();

      // Check if we need to change timebase and number of samples using first channel.
      if(i == 0){
	if(FrequencySetting == -1 or FrequencySetting != measurements[i].GetFrequency()
	   or nsamples != GetHistogram(chan)->GetNbinsX()){
	  std::cout << "New setting for histogram : " << FrequencySetting << " " << measurements[i].GetFrequency()
		    << " " << nsamples << " " << GetHistogram(chan)->GetNbinsX() << std::endl;
	  changeHistogram = true;
	  FrequencySetting = measurements[i].GetFrequency();
	}  	  
      }
      
      // Update the histogram
      if(changeHistogram){
	float ns_per_bin = 0;
	if(FrequencySetting == 0) ns_per_bin = 0.3125;
	else if(FrequencySetting == 1) ns_per_bin = 0.625;
	else if(FrequencySetting == 2) ns_per_bin = 1.25;
	else if(FrequencySetting == 3) ns_per_bin = 2.5;
	GetHistogram(chan)->Reset();
	GetHistogram(chan)->SetBins(nsamples, 0, nsamples*ns_per_bin);
      }
      
      //std::cout << "Nsamples " <<  measurements[i].GetNSamples() << std::endl;
      for(int ib = 0; ib < nsamples; ib++){
	GetHistogram(chan)->SetBinContent(ib+1, measurements[i].GetSample(ib));
      }

      GetHistogram(chan+16)->Fill(CalculatePeakAmplitude((TH1D*)GetHistogram(chan)));
      
    }
  }

  if(dt743_mod2){


    std::vector<RawChannelMeasurement> measurements = dt743_mod2->GetMeasurements();

    bool changeHistogram = false;
    for(int i = 0; i < measurements.size(); i++){

      int chan = measurements[i].GetChannel();
      int nsamples = measurements[i].GetNSamples();

      // Check if we need to change timebase and number of samples using first channel.
      if(i == 0){
        if(FrequencySetting == -1 or FrequencySetting != measurements[i].GetFrequency()
           or nsamples != GetHistogram(chan+8)->GetNbinsX()){
          std::cout << "New setting for histogram : " << FrequencySetting << " " << measurements[i].GetFrequency()
                    << " " << nsamples << " " << GetHistogram(chan+8)->GetNbinsX() << std::endl;
          changeHistogram = true;
          FrequencySetting = measurements[i].GetFrequency();
        }
      }

      // Update the histogram
      if(changeHistogram){
        float ns_per_bin = 0;
        if(FrequencySetting == 0) ns_per_bin = 0.3125;
        else if(FrequencySetting == 1) ns_per_bin = 0.625;
        else if(FrequencySetting == 2) ns_per_bin = 1.25;
        else if(FrequencySetting == 3) ns_per_bin = 2.5;
        GetHistogram(chan+8)->Reset();
        GetHistogram(chan+8)->SetBins(nsamples, 0, nsamples*ns_per_bin);
      }

      //std::cout << "Nsamples " <<  measurements[i].GetNSamples() << std::endl;
      for(int ib = 0; ib < nsamples; ib++){
        GetHistogram(chan+8)->SetBinContent(ib+1, measurements[i].GetSample(ib));
      }

      GetHistogram(chan+24)->Fill(CalculatePeakAmplitude((TH1D*)GetHistogram(chan+8)));

    }
  }

  /*TH1D *lgc = (TH1D*)GetHistogram(6);
  double ped = 0.;
  double maxHeight = 5000.;
  for(int i=1; i<=lgc->GetNbinsX(); i++){
    if(i<20) ped += lgc->GetBinContent(i);
    if(lgc->GetBinContent(i)<maxHeight) maxHeight = lgc->GetBinContent(i);
  }
  ped = ped/20.;
  std::cout << "LGC Pulse Height " << -1.*(maxHeight-ped)*2.5/4096. << " " << maxHeight << " " << ped << std::endl;
  GetHistogram(18)->Fill( -1.*(maxHeight-ped)*2.5/4096.);  */

  
}



void TDT743Amplitude::Reset(){
  
  
  for(int i = 0; i < 16; i++){ // loop over channels
    int index =  i;
    
    // Reset the histogram...
    for(int ib = 0; ib < GetHistogram(index)->GetNbinsX(); ib++) {
      GetHistogram(index)->SetBinContent(ib, 0);
    }
    
    GetHistogram(index)->Reset();
    
  }
}

double TDT743Amplitude::CalculatePeakAmplitude(TH1D *hist){

   double ped = 0.;
   double peakAmp = 5000.;
   for(int i=1; i<=hist->GetNbinsX(); i++){
	if(i<=20) ped += hist->GetBinContent(i);
        if(hist->GetBinContent(i)<peakAmp) peakAmp = hist->GetBinContent(i);
   }
   ped = ped/20.;
   return -1.0*(peakAmp-ped)/4096.*2.5;   

}	
