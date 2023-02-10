#include "TDT743RawData.hxx"
#include <bitset>

#include <iostream>



TDT743RawData::TDT743RawData(int bklen, int bktype, const char* name, void *pdata):
    TGenericData(bklen, bktype, name, pdata)
{

  
  fGlobalHeader.push_back(GetData32()[2]);
  fGlobalHeader.push_back(GetData32()[3]);
  fGlobalHeader.push_back(GetData32()[4]);
  fGlobalHeader.push_back(GetData32()[5]);
  fGlobalHeader.push_back(GetData32()[0]);
  fGlobalHeader.push_back(GetData32()[1]);
  
  fTriggerTime = 0;
  // Do some sanity checking.  
  // Make sure first word has right identifier
  //I think this can be re-used for DT5720, but I don't get why printout uses [0]
  if( (GetData32()[0] & 0xf0000000) != 0xa0000000) 
    std::cerr << "First word has wrong identifier; first word = 0x" 
	      << std::hex << GetData32()[0] << std::dec << std::endl;
  
  int counter = 6;
  
  //Switching from groups to channels for DT5720
  int number_available_channels = 0;
  for(int ch = 0; ch < 4; ch++){	
  	//std::cout <<  	(1<<ch)  <<  " " << GetChMask() << " " << ((1<<ch) & GetChMask()) << std::endl;
	//Bit-wise add!
    if((1<<ch) & GetChMask()){
      number_available_channels++;
    }
  }
  
  int nwords_per_channel = (GetEventSize() - 4)/number_available_channels;
  if(0)std::cout << "Words per group : " << nwords_per_channel << " " 
	    << number_available_channels << std::endl;

  // Loop over groups
  //For DT5720: loops over channels
  std::cout << "Available channels:" << number_available_channels << std::endl;
  std::cout << "nwords per channels:" << nwords_per_channel << std::endl;
  for(int channel = 0; channel < number_available_channels; channel++){
    if(channel == 0) fTriggerTime = 0;
    if((GetData32()[counter] & 0xff000000) != 0xaa000000){
      std::cout << "Bad first word for group: 0x" << std::hex << GetData32()[counter] << std::dec << std::endl;
    }

    if((1<<channel) & GetChMask()){
      
      std::vector<uint32_t> Samples;
      
      //std::cout << "++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
      //std::cout << nwords_per_group << " " << number_available_groups << std::endl;
      
	//Don't think extra info is in DT5720
      for(int i = 0; i < nwords_per_channel; i++){
	// Get extra info
	/*
	if(i == 1) hit0  += (GetData32()[counter] & 0xff000000) >> 24;
	if(i == 2) hit0  += (GetData32()[counter] & 0xff000000) >> 16;
	if(i == 3) time0 += (GetData32()[counter] & 0xff000000) >> 24;
	if(i == 4) time0 += (GetData32()[counter] & 0xff000000) >> 16;
	if(i == 5) hit1  += (GetData32()[counter] & 0xff000000) >> 24;
	if(i == 6) hit1  += (GetData32()[counter] & 0xff000000) >> 16;
	if(i == 7) time1 += (GetData32()[counter] & 0xff000000) >> 24;
	if(i == 8) time1 += (GetData32()[counter] & 0xff000000) >> 16;
	if(i == 9) freq  = (GetData32()[counter] & 0xff000000) >> 24;
    if(channel == 0){
		
		if(i == 13)  {
			fTriggerTime += (uint64_t)(GetData32()[counter] & 0xff000000) >> 24;
			//std::bitset<40> c(fTriggerTime);
			//std::cout <<(GetData32()[counter] & 0xff000000)<< std::endl;
		}
		if(i == 14)  fTriggerTime += (uint64_t)(GetData32()[counter] & 0xff000000) >> 16;
		if(i == 15)  fTriggerTime += (uint64_t)(GetData32()[counter] & 0xff000000) >> 8;
		if(i == 16)  fTriggerTime += (uint64_t)(GetData32()[counter] & 0xff000000) ;
		if(i == 17)  fTriggerTime += (uint64_t)(GetData32()[counter] & 0xff000000) << 8;
		if(i == 17) {
			std::bitset<40> c(fTriggerTime);
			std::cout << c << " " << ((uint64_t)(GetData32()[counter] & 0xff000000) << 8) <<  " " << GetEventCounter() << std::endl;
		}
	}
	*/
	
    /*if(gr == 0){
		
		if(i == 13)  {
			fTriggerTime += (uint64_t)(GetData32()[counter] & 0xff000000) << 8;
			//std::bitset<40> c(fTriggerTime);
			//std::cout <<(GetData32()[counter] & 0xff000000)<< std::endl;
		}
		if(i == 14)  fTriggerTime += (uint64_t)(GetData32()[counter] & 0xff000000);
		if(i == 15)  fTriggerTime += (uint64_t)(GetData32()[counter] & 0xff000000) >> 8;
		if(i == 16)  fTriggerTime += (uint64_t)(GetData32()[counter] & 0xff000000) >> 16;
		if(i == 17)  fTriggerTime += (uint64_t)(GetData32()[counter] & 0xff000000) >> 24;
		if(i == 17) {
			std::bitset<40> c(fTriggerTime);
			std::cout << c << " " << fTriggerTime << std::endl;
		}
	}*/
	// Get Samples (skip 17th sample)
	//Don't skip for DT5720
	//Changed format for DT5720
	  uint32_t sample = (GetData32()[counter] & 0xfff);
	  Samples.push_back(sample);
	  sample = (GetData32()[counter] & 0xfff0000) >> 16;
	  Samples.push_back(sample);				
	/*else{
	  std::cout << i << " " << (GetData32()[counter] & 0xfff) << " " << ((GetData32()[counter] & 0xfff000) >> 12) << std::endl;
	}

	if((i+1)%17 == 0){
	  std::cout << i << " " << (GetData32()[counter] & 0xfff) << " " << ((GetData32()[counter] & 0xfff000) >> 12) << std::endl;	
	}*/
	// increment pointer
	counter++;
      }
	  
      RawChannelMeasurement meas = RawChannelMeasurement(channel*2);
      meas.AddSamples(Samples);

      
      //for(int i = 0; i < Samples0.size(); i++) std::cout << std::hex << Samples0[i] <<std::dec << std::endl; 
    }
  }

}

void TDT743RawData::Print(){

  std::cout << "DT743 decoder for bank " << GetName().c_str() << std::endl;


}
