#pragma once

#include <JuceHeader.h>
#include "2DSonification_MUS3.h"
#include "FaustStrings.h"
#include <ctime>
#include "windows.h"
#define PI 3.14159265
using namespace juce;
class dsp;
class mapUI;

class STS_Sim_2DGuidanceAudioProcessor  : public juce::AudioProcessor, public HighResolutionTimer
{
public:
    STS_Sim_2DGuidanceAudioProcessor();
    ~STS_Sim_2DGuidanceAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	bool user_inStartBox = false;
	bool user_inTargetBox = false;
	// ADD TO OTHERS //
	bool user_inTargetX = false;
	bool user_inTargetY = false;
	bool user_inTargetANG = false;
	bool user_inTargetDIST = false;

	// ADD TO OTHERS //
    float absMin_X = 0;
    float absMax_X = 1;
    float absMin_Y = 0;
    float absMax_Y = 1;

    float rangeMin_X = 0;
    float rangeMax_X = 1;
    float rangeMin_Y = 0;
    float rangeMax_Y = 1;

	float startPos_X = 0.5;
	float startPos_Y = 0.5;

    float userPos_X = 0.5;
    float userPos_Y = 0.5;
	float userPos_ANG_X = 0;
	float userPos_ANG_T = 0;
	float userPos_DIST_C = 0;
	float userPos_DIST_T = 0;

    float targetPos_X = 0.75;
    float targetPos_Y = 0.75;
	double targetPos_ANG_X = 0;
	double targetPos_DIST_C = 0;
	double targetLine_L = 0;

	float errorX_ABS = 0;
	float errorX_REL = 0;
	float errorY_ABS = 0;
	float errorY_REL = 0;
	bool movePitch = true;

	bool isReady = false;
	bool isDSP_ON = true;
	float musicLevel_dB = 0;

	// GUIDANCE MODE
	String guidanceMode = "Cartesian"; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	String freqMode = "Scale"; // OR SWEEP
	String strategyMode = "Separate";

	// MIDI INFO
	int tonic = 69;
	int majScale[24] = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16, 17, 19, 21, 23, 24, 26, 28, 29, 31};
	int numScaleNotes = 8;

	void startMusicDSP()
	{
		fDSP = new mydsp();
		fDSP->init(getSampleRate());
		fUI = new MapUI();
		fDSP->buildUserInterface(fUI);
		outputs = new float* [2];
		for (int channel = 0; channel < 2; ++channel) {
			outputs[channel] = new float[getBlockSize()];
		}
		isDSP_ON = true;
		isReady = true;
	}

	void stopMusicDSP()
	{
		if (isDSP_ON)
		{
			isReady = false;
			isDSP_ON = false;
			delete fDSP;
			delete fUI;
			for (int channel = 0; channel < 2; ++channel) {
				delete[] outputs[channel];
			}
			delete[] outputs;
		}
	}

	void mapToFAUST()
	{
		for (int i = 0; i < numStrategies; i++)
		{
			for (int j = 0; j < numSliders_Strategywise[i]; j++)
			{
				int sliderID = i+1;
				String address = faustStrings.getFBVar_FAUSTAddress_Full(sliderID - 1, j);
				float mapVal = mapVals[i][j];
				fUI->setParamValue(address.toStdString().c_str(), mapVal);
			}
		}
	}

	void startSound()
	{
		fUI->setParamValue(faustStrings.masterGain.toStdString().c_str(), -12);
	}

	void stopSound()
	{
		fUI->setParamValue(faustStrings.masterGain.toStdString().c_str(), -96);
	}

	void update_Ang_Dist()
	{
		targetPos_ANG_X = atan2(targetPos_Y-startPos_Y, targetPos_X-startPos_X) * 180.0 / PI;
		//userPos_ANG_X = atan2(userPos_Y,userPos_X) * 180.0 / PI;

		if (userPos_ANG_X < 0)		userPos_ANG_X += 360;
		if (userPos_ANG_X >= 360)	userPos_ANG_X -= 360 * (int)(userPos_ANG_X / 360.0);

		if (guidanceMode == "Polar")
		{
			userPos_X = startPos_X + abs(userPos_DIST_C) * cos(userPos_ANG_X * PI / 180);
			userPos_Y = startPos_Y + abs(userPos_DIST_C) * sin(userPos_ANG_X * PI / 180);
		}

		userPos_ANG_T = targetPos_ANG_X - userPos_ANG_X;
		if (userPos_ANG_T < -180) userPos_ANG_T += 360;
		if (userPos_ANG_T > 180) userPos_ANG_T -= 360;
		
		float userPos_X_NORM = userPos_X - startPos_X;
		float userPos_Y_NORM = userPos_Y - startPos_Y;
		float targetPos_X_NORM = targetPos_X - startPos_X;
		float targetPos_Y_NORM = targetPos_Y - startPos_Y;

		userPos_DIST_C = sqrt(userPos_X_NORM * userPos_X_NORM + userPos_Y_NORM * userPos_Y_NORM);
		targetPos_DIST_C = sqrt(targetPos_X_NORM * targetPos_X_NORM + targetPos_Y_NORM * targetPos_Y_NORM);
		userPos_DIST_T = userPos_DIST_C - targetPos_DIST_C;
		targetLine_L = 0.5 / (sin(PI / 180.0 * targetPos_ANG_X));
		targetLine_L = min(targetLine_L, 0.707);
	}

	void updateErrors()
	{
		if (guidanceMode == "Cartesian")
		{
			errorX_ABS = userPos_X - targetPos_X;
			errorX_REL = (errorX_ABS >= 0) ?
				errorX_ABS / (rangeMax_X - targetPos_X) :
				errorX_ABS / (targetPos_X - rangeMin_X);
			errorX_REL = pow(abs(errorX_REL), 0.5) * ((errorX_REL > 0) ? 1 : -1);

			errorY_ABS = userPos_Y - targetPos_Y;
			errorY_REL = (errorY_ABS >= 0) ?
				errorY_ABS / (rangeMax_Y - targetPos_Y) :
				errorY_ABS / (targetPos_Y - rangeMin_Y);
			errorY_REL = pow(abs(errorY_REL), 1);

			errorX_REL = (user_inTargetX) ? 0 : errorX_REL;
			errorY_REL = (user_inTargetY) ? 0 : errorY_REL;
		}

		if (guidanceMode == "Polar")
		{
			user_inTargetANG = false;
			user_inTargetDIST = false;
			errorX_ABS = userPos_ANG_T;
			errorX_REL = (errorX_ABS) / 180.0;
 			errorY_ABS = (userPos_DIST_C - targetPos_DIST_C);
			
			if (errorY_ABS > 0)
				errorY_REL = abs(errorY_ABS) / (targetLine_L - targetPos_DIST_C);
			else
				errorY_REL = abs(errorY_ABS) / (targetPos_DIST_C);

			if (abs(userPos_ANG_T) < 1.2 || user_inTargetBox)
			{
				errorX_REL = 0;
				user_inTargetANG = true;
			}

			if (abs(userPos_DIST_T) < 0.01 || user_inTargetBox)
			{
				errorY_REL = 0;
				user_inTargetDIST = true;
			}
			
		}
	}

	void updateFVs()
	{
		float beatMod = 0;
		int saw_quantized = 0;
		float saw_frac = 0;
		float musFreqs[4] = { 440, 440, 440, 440 };

		float sign = (errorX_REL >= 0) ? 1 : -1;
		float order = (guidanceMode == "Polar") ? 0.35 : 0.65;
		sawtooths[0] -= movePitch ? sign * pow(abs(errorX_REL), order) * 0.01 * numScaleNotes : 0;
		sawtooths[0] = (sawtooths[0] > numScaleNotes) ? sawtooths[0] - numScaleNotes : sawtooths[0];
		sawtooths[0] = (sawtooths[0] < 0) ? sawtooths[0] + numScaleNotes : sawtooths[0];
		saw_quantized = (int)sawtooths[0];
		saw_frac = sawtooths[0] - saw_frac;

		if (freqMode == "Scale")
		{
			musFreqs[0] = MidiMessage::getMidiNoteInHertz(tonic + majScale[saw_quantized]);
			musFreqs[1] = MidiMessage::getMidiNoteInHertz(tonic + majScale[saw_quantized]);
			musFreqs[2] = MidiMessage::getMidiNoteInHertz(tonic + majScale[saw_quantized + 7]);
			musFreqs[3] = MidiMessage::getMidiNoteInHertz(tonic + majScale[saw_quantized + 7]);
		}

		if (freqMode == "Sweep")
		{
			musFreqs[0] = 440 + sawtooths[0] * 440 / numScaleNotes;
			musFreqs[1] = musFreqs[0];
			musFreqs[2] = musFreqs[0] * 2;
			musFreqs[3] = musFreqs[0] * 2;
		}

		beatMod = saw_frac * abs(sin(3.14156 * saw_frac));

		mapVals[0][0] = musFreqs[0] / 2;
		mapVals[0][1] = musFreqs[1] / 2;
		mapVals[0][2] = musFreqs[2] / 2;
		mapVals[0][3] = musFreqs[3] / 2;

		// SET STRATEGIES TO ZERO INITIALLY
		mapVals[1][0] = 0;						// ROUGHNESS
		mapVals[2][0] = 0;						// BEATING
		mapVals[3][0] = 0;						// TARGET BELL

		if (guidanceMode == "Polar")
		{
			//movePitch = (!user_inTargetX) ? true : false;
			if (strategyMode == "Combined")
			{
				if (abs(errorY_REL) > 0.001)
				{
					if (errorY_ABS < 0)
					{
						mapVals[1][0] = errorY_REL;
					}
					else
					{
						mapVals[2][0] = abs(errorY_REL);
					}
				}
			}

			if (strategyMode == "Separate")
			{
				if (user_inTargetANG && abs(errorY_REL) > 0.001)
				{
					if (errorY_ABS < 0)
					{
						mapVals[1][0] = errorY_REL;
					}
					else
					{
						mapVals[2][0] = abs(errorY_REL);
					}
				}	
			}
		}

		if (guidanceMode == "Cartesian")
		{
			if (strategyMode == "Separate")
			{
				if (user_inTargetX && errorY_REL > 0.001)
				{
					if (errorY_ABS < 0)
					{
						mapVals[1][0] = errorY_REL;
					}

					else
					{
						mapVals[2][0] = errorY_REL;
					}
				}
				movePitch = true;
			}

			if (strategyMode == "Combined")
			{
				if (errorY_REL > 0.001)
				{
					if (errorY_ABS < 0)
					{
						mapVals[1][0] = errorY_REL;
					}

					else
					{
						mapVals[2][0] = errorY_REL;
					}
				}
				movePitch = true;
			}
		}

		mapVals[2][0] *= 0.13;
		mapVals[2][0] = pow(mapVals[2][0], 0.5);
		mapVals[3][0] = (user_inTargetBox) ? 0.4 : 0;
	}

	/////////////////////////////////////////////////// I N F O ///////////////////////////////////////////////////

	bool isRecording_Trials = false;
	bool isTrial_ON = false;
	bool isTrial_ON_Z1 = false;
	int trials_Complete = 0;
	int targetsFound = 0;
	float trial_TargetPos[2] = { 0.75,0.75 };
	std::vector<float> user_Traj_X;
	std::vector<float> user_Traj_Y;
	std::vector<float> timeStamps;
	std::vector<float> series_inTargetBox;
	std::vector<float> user_angX;
	float timeSinceStateChange = 0;
	float timeElapsed_presentTrial = 0;
	String rec_LogPath = "";
	FILE* trialLog;

	void createRecFolder()
	{
		// GENERATE PATH AND DIRECTORY FOR LOG STORAGE
		rec_LogPath = File::getSpecialLocation(File::currentApplicationFile).getFullPathName();
		rec_LogPath = rec_LogPath.upToLastOccurrenceOf("\\", true, false);
		rec_LogPath += strategyMode + " - " + guidanceMode + " - " + getCurrentTime() + "\\";
		CreateDirectory(rec_LogPath.toStdString().c_str(), NULL);
	}

	void saveTrialFile()
	{
		if (trials_Complete >= 1)
		{
			String rec_LogPath_TRIAL = rec_LogPath + "Trial " + String(trials_Complete) + ".csv";
			trialLog = fopen(rec_LogPath_TRIAL.toStdString().c_str(), "w");
			std::string formatSpec = "%s,\n";

			String headerLine = "TimeElapsed,UserPosX,UserPosY,TargetX,TargetY,User_In_Target,AngFacing";
			fprintf(trialLog, formatSpec.c_str(), headerLine);
			String logLine = "";
			for (int i = 0; i < timeStamps.size(); i++)
			{
				logLine = "";
				logLine += String(timeStamps.at(i)) + "," +
					String(user_Traj_X.at(i), 3) + "," +
					String(user_Traj_Y.at(i), 3) + "," +
					String(targetPos_X, 3) + "," +
					String(targetPos_Y, 3) + "," +
					String(series_inTargetBox.at(i)) + "," +
					String(user_angX.at(i), 3);

				fprintf(trialLog, formatSpec.c_str(), logLine);
			}

			user_Traj_X.clear();
			user_Traj_Y.clear();
			timeStamps.clear();
			series_inTargetBox.clear();
			user_angX.clear();

			fclose(trialLog);
		}
	}

	String getCurrentTime()
	{
		time_t rawtime;
		struct tm* timeinfo;
		char buffer[80];

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		strftime(buffer, sizeof(buffer), "%d-%m-%Y %H-%M-%S", timeinfo);
		std::string str(buffer);

		return String(str);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (STS_Sim_2DGuidanceAudioProcessor)
	MapUI* fUI;
	dsp* fDSP;
	float** outputs;
	FaustStrings faustStrings;

	int timer_Interval_MS = 10;
	void hiResTimerCallback();

	double timeDone = 0;
	long cyclesDone = 0;

	int numStrategies = 4;
	int numSliders_Strategywise[100] =
	{
		4,
		1,
		1,
		1
	};

	float sawtooths[8];
	float mapVals[100][8];

	


};
