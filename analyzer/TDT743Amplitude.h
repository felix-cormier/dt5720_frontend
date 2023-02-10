#ifndef TDT743Amplitude_h
#define TDT743Amplitude_h

#include <string>
#include "THistogramArrayBase.h"

/// Class for making histograms of raw DT743 waveforms;
class TDT743Amplitude : public THistogramArrayBase {
public:
  TDT743Amplitude();
  virtual ~TDT743Amplitude(){};

  void UpdateHistograms(TDataContainer& dataContainer);

  // Reset the histograms; needed before we re-fill each histo.
  void Reset();
  
  void CreateHistograms();
  
  /// Take actions at begin run
  void BeginRun(int transition,int run,int time){		
    CreateHistograms();		
  }

private:

  int FrequencySetting;

  double CalculatePeakAmplitude(TH1D *hist);
};

#endif


