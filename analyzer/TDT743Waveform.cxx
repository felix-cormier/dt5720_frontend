#include "TDT743Waveform.h"

#include "TDT743RawData.hxx"
#include "TDirectory.h"


/// Reset the histograms for this canvas
TDT743Waveform::TDT743Waveform(){

  SetSubTabName("DT743 Waveforms");
  SetUpdateOnlyWhenPlotted(true);
  
  CreateHistograms();
  FrequencySetting = -1;
}


void TDT743Waveform::CreateHistograms(){

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
    
    char name[100];
    char title[100];
    sprintf(name,"DT743_%i",i);
    
    sprintf(title,"DT743 Waveform for channel=%i",i);	
    
    TH1D *tmp = new TH1D(name, title, numSamples, 0, fWFLength);
    tmp->SetXTitle("ns");
    tmp->SetYTitle("ADC value");
    
    push_back(tmp);
  }

  for(int i=0; i<16; i++){
    double amplitude = 3.0;	  
    if(i>=2 && i<7) amplitude = 0.3;
    TH1D *tmp = new TH1D(Form("Channel%dAmp",i),Form("Channel %d Amplitude",i),200,0,amplitude);
    tmp->SetXTitle("Peak Amplitude (V)");
    tmp->SetYTitle("Number of Events");
    push_back(tmp);
  }   

  TH1D *tmp1 = new TH1D("TOF","Time of Flight",720,20,110);
  tmp1->SetXTitle("Time Difference (ns)");
  tmp1->SetYTitle("Number of Events");
  push_back(tmp1);

  tofFit = new TF1("tofFit","[0]/[2]*TMath::Exp(-0.5*pow((x-[1])/[2],2))+[3]/[2]*TMath::Exp(-0.5*pow((x-([4]+[1]))/[2],2))+[5]/[2]*TMath::Exp(-0.5*pow((x-([6]+[4]))/[2],2))",38,43);

  /*TH1D *tmp3 = new TH1D("LGCPH","Lead Glass Calorimeter Pulse Height",100,0,1);
  tmp3->SetXTitle("Pulse Height");
  tmp3->SetYTitle("Events");
  push_back(tmp3);*/

  std::cout << "TDT743Waveform done init...... " << std::endl;
  FrequencySetting = -1;
}


void TDT743Waveform::UpdateHistograms(TDataContainer& dataContainer){
  
  int eventid = dataContainer.GetMidasData().GetEventId();
  int timestamp = dataContainer.GetMidasData().GetTimeStamp();
  
  TDT743RawData *dt743 = dataContainer.GetEventData<TDT743RawData>("D720");
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
  else {
	  std::cout << "DID NOT LOAD EVENT" << std::endl;
       }

  if(dt743_mod2){


    double times[8]; 
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

      double baseline = waveform_baseline((TH1D*)GetHistogram(chan+8));
      //std::cout << "Baseline = " << baseline << std::endl;
      int gTime = global_timing((TH1D*)GetHistogram(chan+8), baseline);
      //std::cout << "Time Minimum = " << gTime << std::endl;
      times[chan] = CFD_timing((TH1D*)GetHistogram(chan+8), baseline, gTime, 0.1, 0.9, 0.5);
      //std::cout << "Time CFD = " << times[chan] << std::endl;

    }

    float ns_per_bin = 0;
    if(FrequencySetting == 0) ns_per_bin = 0.3125;
    else if(FrequencySetting == 1) ns_per_bin = 0.625;
    else if(FrequencySetting == 2) ns_per_bin = 1.25;
    else if(FrequencySetting == 3) ns_per_bin = 2.5;

    double tof = ns_per_bin*( (times[0]+times[1]+times[2]+times[3])/4.0-(times[4]+times[5]+times[6]+times[7])/4.0 );

    //std::cout << tof << std::endl;
    GetHistogram(32)->Fill(-1.*tof);
    tofFit->SetParameter(0,20);
    tofFit->SetParLimits(0,0,500000);
    tofFit->SetParameter(1,39.5);
    tofFit->SetParLimits(1,35,45);
    tofFit->SetParameter(2,0.3);
    tofFit->SetParLimits(2,0.2,0.5);
    tofFit->SetParameter(3,5);
    tofFit->SetParLimits(3,0,500000);
    tofFit->SetParameter(4,0.5);
    tofFit->SetParLimits(4,0.05,2);
    tofFit->SetParameter(5,5);
    tofFit->SetParLimits(5,0,500000);
    tofFit->SetParameter(6,0.5);
    tofFit->SetParLimits(6,0.05,2);
    //GetHistogram(32)->Fit("tofFit","R");

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



void TDT743Waveform::Reset(){
  
  
  for(int i = 0; i < 16; i++){ // loop over channels
    int index =  i;
    
    // Reset the histogram...
    for(int ib = 0; ib < GetHistogram(index)->GetNbinsX(); ib++) {
      GetHistogram(index)->SetBinContent(ib, 0);
    }
    
    GetHistogram(index)->Reset();
    
  }
}

double TDT743Waveform::CalculatePeakAmplitude(TH1D *hist){

   double ped = 0.;
   double peakAmp = 5000.;
   for(int i=1; i<=hist->GetNbinsX(); i++){
	if(i<=20) ped += hist->GetBinContent(i);
        if(hist->GetBinContent(i)<peakAmp) peakAmp = hist->GetBinContent(i);
   }
   ped = ped/20.;
   return -1.0*(peakAmp-ped)/4096.*2.5;   

}	


double TDT743Waveform::waveform_baseline(TH1D* waveform) {
    double sample_sum = 0.;
    int count = 0;
    for (int k=1; k <20; k++) {
        sample_sum += waveform->GetBinContent(k);
        count++;
    }
    double baseline = sample_sum/float(count);
    return baseline;
}

int TDT743Waveform::global_timing(TH1D* waveform, double baseline) {
        // Find global minimum
        double ymin = 99999999.;
        int imin = 0;
	int nsamples = waveform->GetNbinsX();
        for (int k = 1; k < nsamples-2; k++){
            if (waveform->GetBinContent(k)-baseline < ymin) {
                ymin = waveform->GetBinContent(k)-baseline;
                imin = k;
                // std::cout<< "   " << ymin << std::endl;
            }
        }
    return imin;
}


float TDT743Waveform::CFD_timing(TH1D* waveform, double baseline, const int global_imin, const float startp, const float endp, const float percentage) {
    double y_min = waveform->GetBinContent(global_imin)-baseline;
    double y_end = endp*y_min;
    double y_start = startp*y_min;
    double rise_amplitude = (endp-startp)*y_min;
    int j = global_imin;
    int j_end = 0;
    while (((waveform->GetBinContent(j)-baseline) < y_end) && (j > 1)) {
        j--;
        j_end = j-1;
    }
    j = global_imin;
    int j_start = 0;
    while (((waveform->GetBinContent(j)-baseline) < y_start) && (j > 1)) {
        j--;
        j_start = j+1;
    }
    double b = (y_end-y_start)/(double(j_end)-double(j_start));
    double a = y_start - j_start*b;
    double iCFD = (percentage*rise_amplitude-a)/b;
    return iCFD;
}

