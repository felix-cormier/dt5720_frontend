#ifndef TDT743Waveform_h
#define TDT743Waveform_h

#include <string>
#include "THistogramArrayBase.h"
#include "TF1.h"

/// Class for making histograms of raw DT743 waveforms;
class TDT743Waveform : public THistogramArrayBase {
public:
  TDT743Waveform();
  virtual ~TDT743Waveform(){};

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
  float CFD_timing(TH1D* waveform, double baseline, const int global_imin, const float startp, const float endp, const float percentage);
  int global_timing(TH1D* waveform, double baseline);
  double waveform_baseline(TH1D* waveform);
  TF1 *tofFit;

};

#endif


