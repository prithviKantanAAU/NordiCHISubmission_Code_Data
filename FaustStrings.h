#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
using namespace juce;

class FaustStrings
{
public:
	FaustStrings()
	{

	}

	~FaustStrings(){}
	
	//String baseName = "/STS_Synth";
	String baseName = "";

	String masterGain = baseName + "/Mapping_Tabs/Mixer_And_Master/Master_Gain/Master_Gain";
	String reverbOn = baseName + "/Mapping_Tabs/Mixer_And_Master/Master_Gain/Reverb_ON";

	std::string getFBVar_FAUSTAddress_Full(short fbVar_Idx, short synthCtrl_Idx)
	{
		std::string out = "";

		out = baseName.toStdString() + "/Mapping_Tabs/Sonification_Control/Soni_" 
			+ String(fbVar_Idx + 1).toStdString() + "/Control_" + String(synthCtrl_Idx).toStdString();

		return out;
	}

	std::string getTrackGainAddress(short trackIdx)
	{
		return baseName.toStdString() + "/Mapping_Tabs/Mixer_And_Master/Track_Gain/Track_"
			+ String(trackIdx + 1).toStdString();
	}

	std::string trackIDs[8]
		= { "Track_1/","Track_2/","Track_3/","Track_4/","Track_5/","Track_6/","Track_7/","Track_8/" };


	//EQ Setters
	std::string EQ_base = "Mapping_Tabs/EQ_Section/";
	std::string EQ_filterIDs[4] = { "1_HPF/", "2_Parametric_1/", "3_Parametric_2/", "4_LPF/" };
	std::string EQ_filterParamIDs[3] = { "Freq", "Gain", "Q" };
	std::string FetchEQ_String(int trackID, int filterID, int paramID)
	{
		return baseName.toStdString() + EQ_base + trackIDs[trackID] + EQ_filterIDs[filterID] + EQ_filterParamIDs[paramID];
	};

	//Comp Setters
	std::string Comp_base = "Mapping_Tabs/Comp_Section/";
	std::string Comp_ParamIDs[4] = { "Threshold","Ratio","Attack","Release" };
	std::string FetchComp_String(int trackID, int paramID)
	{
		return baseName.toStdString() + Comp_base + trackIDs[trackID] + Comp_ParamIDs[paramID];
	};
};
