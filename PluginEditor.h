#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
using namespace juce;

class STS_Sim_2DGuidanceAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Timer, juce::MouseListener, ComboBox::Listener
{
public:
    STS_Sim_2DGuidanceAudioProcessorEditor (STS_Sim_2DGuidanceAudioProcessor&);
    ~STS_Sim_2DGuidanceAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    int interfaceWd = 1300;
    int interfaceHt = 700;
    void mouseDoubleClick(const MouseEvent& event);
    bool isMouseControl = false;
    float mouseStart_X = 0;
    float mouseStart_Y = 0;
    float userPosStart_X = 0;
    float userPosStart_Y = 0;

    bool isTarget_Visible = true;

    void handleMouseDrag_Target()
    {
        // Calculate X and Y increment per pixel of mouse movement
        float boxWidth = 0.5 * interfaceWd;
        float boxHeight = 0.8 * interfaceHt;
        float incrementX = 1;
        float incrementY = 1;
            
        float mousePos_Pres_X = Desktop::getInstance().getMainMouseSource().getScreenPosition().getX();
        float mousePos_Pres_Y = Desktop::getInstance().getMainMouseSource().getScreenPosition().getY();

        if (audioProcessor.guidanceMode == "Cartesian")
        {
            // Update UserPos Variable
            audioProcessor.userPos_X = userPosStart_X + (mousePos_Pres_X - mouseStart_X) * incrementX / boxWidth;
            audioProcessor.userPos_Y = userPosStart_Y - (mousePos_Pres_Y - mouseStart_Y) * incrementY / boxHeight;

            audioProcessor.userPos_X = jlimit(audioProcessor.rangeMin_X, audioProcessor.rangeMax_X, audioProcessor.userPos_X);
            audioProcessor.userPos_Y = jlimit(audioProcessor.rangeMin_Y, audioProcessor.rangeMax_Y, audioProcessor.userPos_Y);
        }

        float cursorPos_X = userPosStart_X + (mousePos_Pres_X - mouseStart_X) * incrementX / boxWidth;
        float cursorPos_Y = userPosStart_Y - (mousePos_Pres_Y - mouseStart_Y) * incrementY / boxHeight;

        if (audioProcessor.guidanceMode == "Polar")
        {
           // if (audioProcessor.userPos_DIST_C < 0.01)
            audioProcessor.userPos_ANG_X = -360 * (cursorPos_X - userPosStart_X) * 0.8;
            audioProcessor.userPos_DIST_C = 2*audioProcessor.targetLine_L * max((cursorPos_Y - userPosStart_Y),0);
        }
    }

    //MouseListener* indicUserPosListener;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    STS_Sim_2DGuidanceAudioProcessor& audioProcessor;

    // Task Interface
    Label showTimeElapsed;
    Label indicUserPos;
    Slider setTarget_X;
    Label indicTarget_X;
    Slider setTarget_Y;
    Label indicTarget_Y;
    Slider setMovBounds_X;
    Label indicMovBounds_X[2];
    Slider setMovBounds_Y;
    Label indicMovBounds_Y[2];
    Label indicStartZone;
    int startZone_SidePx = 30;
    int targetZone_SidePx = 25;
    int userPos_SidePx = 10;
    Label indicTargetZone;
    Label indicState;
    Label indic_targetsFound;
    ComboBox setGuidanceMode;
    ComboBox setStrategyMode;
    void comboBoxChanged(ComboBox* box);
    // Manual Setup
    TextButton set_randomTarget;
    TextButton hide_targetControls;
    TextButton reset_positions;

    // Info Display
    Label trials_Complete;
    TextButton trials_Reset;
    TextButton set_mouseControl;

    // POLAR INDICATOR
    Label angleIndicator[20];
    int angleIndicator_NumBlocks = 8;
    float angleIndicator_Radius = 0.25;

    void angleIndicator_setVisible(bool on)
    {
        for (int i = 0; i < angleIndicator_NumBlocks; i++)
            angleIndicator[i].setVisible(on);
    }

    void angleIndicator_UPDATE()
    {
        float side = userPos_SidePx;
        float dist = 0;
        float xPos_C = 0;
        float yPos_C = 0;

        for (int i = 0; i < angleIndicator_NumBlocks; i++)
        {
            dist = angleIndicator_Radius * (i + 1) / (float)angleIndicator_NumBlocks;
            xPos_C = interfaceWd * 0.65 + dist * cos(audioProcessor.userPos_ANG_X * PI / 180.0) * 0.5 * interfaceWd - side/2.0;
            yPos_C = interfaceHt * 0.5 - dist * sin(audioProcessor.userPos_ANG_X * PI / 180.0) * 0.8 * interfaceHt - side/2.0;

            angleIndicator[i].setBounds(xPos_C, yPos_C, side, side);
        }
    }

    // 0 = Practice // 1 = New Target Set // 2 = Trial Ongoing // 3 = Trial Completed
    short trialState = 0;
    void changeTrialState()
    {
        bool tooLong = (audioProcessor.timeElapsed_presentTrial) > 30;
        audioProcessor.timeSinceStateChange = 0;
        audioProcessor.timeElapsed_presentTrial = 0;
        audioProcessor.isTrial_ON = (!audioProcessor.isTrial_ON) ? true : false;

        if (audioProcessor.isTrial_ON)
        {
            if (trialState == 1) trialState = 2;
            audioProcessor.startSound();

            if (isMouseControl)
            {
                mouseStart_X = Desktop::getInstance().getMainMouseSource().getScreenPosition().getX();
                mouseStart_Y = Desktop::getInstance().getMainMouseSource().getScreenPosition().getY();
                userPosStart_X = audioProcessor.userPos_X;
                userPosStart_Y = audioProcessor.userPos_Y;
            }
        }
        else
        {
            if (trialState == 2)
            {
                trialState = 3;
                audioProcessor.trials_Complete++;
                if (audioProcessor.user_inTargetBox && !tooLong) audioProcessor.targetsFound++;
                audioProcessor.timeElapsed_presentTrial = 0;
                //audioProcessor.saveTrialFile();
            }
            audioProcessor.stopSound();
        } 
    }

    // 0 = Free // 1 = Horizontal // 2 = Vertical
    short inTrial_Direction = 0;

    // Trial Configuration
    TextButton recordTrials;
    TextButton startTrial;

    void addAllElements()
    {
        addAndMakeVisible(setTarget_X);
        addAndMakeVisible(setTarget_Y);
        addAndMakeVisible(indicTarget_X);
        addAndMakeVisible(indicTarget_Y);
        addAndMakeVisible(indicUserPos);
        addAndMakeVisible(setMovBounds_X);
        addAndMakeVisible(setMovBounds_Y);
        addAndMakeVisible(set_randomTarget);
        addAndMakeVisible(reset_positions);
        addAndMakeVisible(hide_targetControls);
        addAndMakeVisible(indicStartZone);
        addAndMakeVisible(indicTargetZone);
        addAndMakeVisible(trials_Complete);
        addAndMakeVisible(trials_Reset);
        addAndMakeVisible(recordTrials);
        addAndMakeVisible(set_mouseControl);
        addAndMakeVisible(startTrial);
        addAndMakeVisible(indicState);
        addAndMakeVisible(indic_targetsFound);
        addAndMakeVisible(setGuidanceMode);
        addAndMakeVisible(setStrategyMode);
        addAndMakeVisible(showTimeElapsed);

        for (int i = 0; i < 2; i++)
        {
            addAndMakeVisible(indicMovBounds_X[i]);
            addAndMakeVisible(indicMovBounds_Y[i]);
        }

        for (int i = 0; i < angleIndicator_NumBlocks; i++)
            addAndMakeVisible(angleIndicator[i]);
    }

    void configButtons()
    {
        set_mouseControl.setButtonText("MOUSE DISABLED");
        set_mouseControl.setColour(set_mouseControl.buttonColourId, Colours::red);
        set_mouseControl.onClick = [this]
        {
            isMouseControl = (!isMouseControl) ? true : false;
            if (isMouseControl)
            {
                set_mouseControl.setButtonText("MOUSE ENABLED");
                set_mouseControl.setColour(set_mouseControl.buttonColourId, Colours::green);
            }

            else
            {
                set_mouseControl.setButtonText("MOUSE DISABLED");
                set_mouseControl.setColour(set_mouseControl.buttonColourId, Colours::red);
            }
        };

        set_randomTarget.setButtonText("Set Random Target");
        set_randomTarget.onClick = [this]
        {
            setNextTarget();
        };

        hide_targetControls.setButtonText("Toggle Target Visibility");
        hide_targetControls.setColour(hide_targetControls.buttonColourId, Colours::green);
        hide_targetControls.onClick = [this]
        {
            bool toSet = isTarget_Visible ? false : true;
            if (toSet) trialState = 0;
            isTarget_Visible = toSet;
            setTarget_X.setVisible(toSet);
            setTarget_Y.setVisible(toSet);
            indicTarget_X.setVisible(toSet);
            indicTarget_Y.setVisible(toSet);
            indicTargetZone.setVisible(toSet);
        };

        reset_positions.setButtonText("Reset Interface");
        reset_positions.onClick = [this]
        {
            setMovBounds_X.setMinAndMaxValues(0, 1.0);
            setMovBounds_Y.setMinAndMaxValues(0, 1.0);
            setTarget_X.setValue(0.75);
            setTarget_Y.setValue(0.75);
            audioProcessor.userPos_X = audioProcessor.startPos_X;
            audioProcessor.userPos_Y = audioProcessor.startPos_Y;
            audioProcessor.userPos_DIST_C = 0;
            audioProcessor.userPos_ANG_X = 0;
        };

        trials_Reset.setButtonText("Refresh Count");
        trials_Reset.onClick = [this]
        {
            audioProcessor.trials_Complete = 0;
            audioProcessor.targetsFound = 0;
        };

        recordTrials.setButtonText("Record Trial Data");
        recordTrials.setColour(recordTrials.buttonColourId, Colours::red);
        recordTrials.onClick = [this]
        {
            audioProcessor.isRecording_Trials = (audioProcessor.isRecording_Trials) ? false : true;
            if (indicTargetZone.isVisible()) hide_targetControls.triggerClick();
            if (audioProcessor.isRecording_Trials)
            {
                trials_Reset.triggerClick();
                audioProcessor.createRecFolder();
                recordTrials.setButtonText("Stop Recording");
                recordTrials.setColour(recordTrials.buttonColourId, Colours::blue);
            }
            else
            {
                recordTrials.setButtonText("Record Trial Data");
                recordTrials.setColour(recordTrials.buttonColourId, Colours::red);
            }
        };

        startTrial.setButtonText("Setup Next Trial");
        startTrial.setColour(startTrial.buttonColourId,Colours::white);
        startTrial.setColour(startTrial.textColourOffId,Colours::black);
        startTrial.setColour(startTrial.textColourOnId,Colours::black);
        startTrial.onClick = [this]
        {
            reset_positions.triggerClick();
            if (indicTargetZone.isVisible()) hide_targetControls.triggerClick();
            set_randomTarget.triggerClick();
            trialState = 1;
        };

        for (int i = 0; i < angleIndicator_NumBlocks; i++)
        {
            angleIndicator[i].setColour(angleIndicator[i].backgroundColourId,
                juce::Colour::fromFloatRGBA(0.3f, 0.3f, 1.0f, 1.0f));
        }
    }

    void configSliders()
    {
        setTarget_X.setColour(setTarget_X.trackColourId, Colours::green);
        setTarget_X.setColour(setTarget_X.backgroundColourId, Colours::black);
        setTarget_X.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
        setTarget_Y.setColour(setTarget_Y.trackColourId, Colours::green);
        setTarget_Y.setColour(setTarget_Y.backgroundColourId, Colours::black);
        setTarget_Y.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
        setTarget_Y.setSliderStyle(Slider::SliderStyle::LinearVertical);
        setMovBounds_X.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
        setMovBounds_X.setSliderStyle(Slider::SliderStyle::TwoValueHorizontal);
        setMovBounds_Y.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
        setMovBounds_Y.setSliderStyle(Slider::SliderStyle::TwoValueVertical);

        setTarget_X.setRange(audioProcessor.absMin_X, audioProcessor.absMax_X);
        setTarget_X.setValue(audioProcessor.targetPos_X);
        setTarget_Y.setRange(audioProcessor.absMin_Y, audioProcessor.absMax_Y);
        setTarget_Y.setValue(audioProcessor.targetPos_Y);

        setMovBounds_X.setColour(setMovBounds_X.trackColourId, Colours::red);
        setMovBounds_X.setColour(setMovBounds_X.backgroundColourId, Colours::black);
        setMovBounds_X.setRange(audioProcessor.absMin_X, audioProcessor.absMax_X);
        setMovBounds_Y.setColour(setMovBounds_Y.trackColourId, Colours::red);
        setMovBounds_Y.setColour(setMovBounds_Y.backgroundColourId, Colours::black);
        setMovBounds_Y.setRange(audioProcessor.absMin_Y, audioProcessor.absMax_Y);
        setMovBounds_X.setMinAndMaxValues(audioProcessor.rangeMin_X, audioProcessor.rangeMax_X);
        setMovBounds_Y.setMinAndMaxValues(audioProcessor.rangeMin_Y, audioProcessor.rangeMax_Y);

        setGuidanceMode.addItem("Cartesian", 1);
        setGuidanceMode.addItem("Polar", 2);
        setGuidanceMode.setSelectedId(2);
        setGuidanceMode.addListener(this);
        setGuidanceMode.setWantsKeyboardFocus(false);
        setGuidanceMode.setJustificationType(Justification::centred);
        setGuidanceMode.setColour(setGuidanceMode.backgroundColourId,Colours::blue);

        setStrategyMode.addItem("Combined", 1);
        setStrategyMode.addItem("Separate", 2);
        setStrategyMode.setSelectedId(2);
        setStrategyMode.addListener(this);
        setStrategyMode.setWantsKeyboardFocus(false);
        setStrategyMode.setJustificationType(Justification::centred);
        setStrategyMode.setColour(setStrategyMode.backgroundColourId, Colours::darkblue);

        setTarget_X.onValueChange = [this]
        {
            audioProcessor.targetPos_X = setTarget_X.getValue();
            //trialState = 0;
        };

        setTarget_Y.onValueChange = [this]
        {
            audioProcessor.targetPos_Y = setTarget_Y.getValue();
            //trialState = 0;
        };

        setMovBounds_X.onValueChange = [this]
        {
            audioProcessor.rangeMin_X = setMovBounds_X.getMinValue();
            audioProcessor.rangeMax_X = setMovBounds_X.getMaxValue();
        };

        setMovBounds_Y.onValueChange = [this]
        {
            audioProcessor.rangeMin_Y = setMovBounds_Y.getMinValue();
            audioProcessor.rangeMax_Y = setMovBounds_Y.getMaxValue();
        };
    }

    void configIndicators()
    {
        for (int i = 0; i < 2; i++)
        {
            indicMovBounds_X[i].setColour(indicMovBounds_X[i].backgroundColourId, Colours::red);
            indicMovBounds_Y[i].setColour(indicMovBounds_X[i].backgroundColourId, Colours::red);
        }
        indicTarget_X.setColour(indicTarget_X.backgroundColourId, Colours::green);
        indicTarget_Y.setColour(indicTarget_Y.backgroundColourId, Colours::green);

        indicUserPos.setColour(indicUserPos.backgroundColourId, Colours::white);

        indicStartZone.setColour(indicStartZone.backgroundColourId, Colour::fromFloatRGBA(1.0f, 0.0f, 0.0f, 0.4f));
        indicTargetZone.setColour(indicTargetZone.backgroundColourId, Colour::fromFloatRGBA(0.0f, 1.0f, 0.0f, 0.4f));

        trials_Complete.setText("Trials Complete: " + String(audioProcessor.trials_Complete), dontSendNotification);
        trials_Complete.setJustificationType(Justification::centred);

        indicState.setJustificationType(Justification::centred);
        indicState.setFont(juce::Font(24.0f, juce::Font::bold));

        showTimeElapsed.setJustificationType(Justification::centred);
        showTimeElapsed.setFont(juce::Font(24.0f, juce::Font::bold));
    }

    void updateVisualizer()
    {
        trials_Complete.setText("Trials Complete: " + String(audioProcessor.trials_Complete), dontSendNotification);
        showTimeElapsed.setText("Time Elapsed: " + String(audioProcessor.timeElapsed_presentTrial,2) + " sec", dontSendNotification);
        showTimeElapsed.setColour(showTimeElapsed.textColourId, (audioProcessor.timeElapsed_presentTrial > 30) ? Colours::orangered : Colours::white);
        
        startTrial.setVisible(!audioProcessor.isTrial_ON);
        showTimeElapsed.setVisible(audioProcessor.isTrial_ON);
        //reset_positions.setVisible(!audioProcessor.isTrial_ON);
        reset_positions.setVisible(false);
        //set_randomTarget.setVisible(!audioProcessor.isTrial_ON && !audioProcessor.isRecording_Trials);
        set_randomTarget.setVisible(false);
        set_mouseControl.setVisible(!audioProcessor.isTrial_ON);
        hide_targetControls.setVisible(!audioProcessor.isTrial_ON && !audioProcessor.isRecording_Trials);
        setGuidanceMode.setVisible(!audioProcessor.isTrial_ON && !audioProcessor.isRecording_Trials);
        setStrategyMode.setVisible(!audioProcessor.isTrial_ON && !audioProcessor.isRecording_Trials);
        indic_targetsFound.setVisible(!audioProcessor.isTrial_ON && !audioProcessor.isRecording_Trials);
        //reset_positions.setVisible(!audioProcessor.isRecording_Trials);
        trials_Reset.setVisible(!audioProcessor.isRecording_Trials);

        indic_targetsFound.setJustificationType(Justification::centred);
        float targetsPercent = ((double)audioProcessor.targetsFound / (double)audioProcessor.trials_Complete) * 100.0;
        indic_targetsFound.setText(
            "Targets Found: " + String(audioProcessor.targetsFound) + "/" + String(audioProcessor.trials_Complete) + " (" + String(targetsPercent,2) + " %)"
            ,dontSendNotification
        );
        indic_targetsFound.setColour(indic_targetsFound.backgroundColourId, (targetsPercent >= 80 && audioProcessor.trials_Complete >= 10) ? Colours::darkgreen : Colours::red);

        indicTarget_X.setBounds(
            interfaceWd * 0.4 + audioProcessor.targetPos_X * interfaceWd * 0.5,
            interfaceHt * 0.1,
            2,
            interfaceHt * 0.8
        );

        indicTarget_Y.setBounds(
            interfaceWd * 0.4,
            interfaceHt * 0.9 - audioProcessor.targetPos_Y * interfaceHt * 0.8,            
            interfaceWd * 0.5,
            2
        );

        indicTargetZone.setBounds(
            interfaceWd * 0.4 + audioProcessor.targetPos_X * interfaceWd * 0.5 - targetZone_SidePx * 0.5,
            interfaceHt * 0.9 - audioProcessor.targetPos_Y * interfaceHt * 0.8 - targetZone_SidePx * 0.5,
            targetZone_SidePx,
            targetZone_SidePx
        );

        indicMovBounds_X[0].setBounds(
            interfaceWd * 0.4 + audioProcessor.rangeMin_X * interfaceWd * 0.5,
            interfaceHt * 0.1,
            4,
            interfaceHt * 0.8
        );

        indicMovBounds_Y[0].setBounds(
            interfaceWd * 0.4,
            interfaceHt * 0.9 - audioProcessor.rangeMin_Y * interfaceHt * 0.8,
            interfaceWd * 0.5,
            4
        );

        indicStartZone.setBounds(
            interfaceWd * 0.4 + audioProcessor.startPos_X * interfaceWd * 0.5 - startZone_SidePx * 0.5,
            interfaceHt * 0.9 - audioProcessor.startPos_Y * interfaceHt * 0.8 - startZone_SidePx * 0.5,
            startZone_SidePx,
            startZone_SidePx
        );

        indicMovBounds_X[1].setBounds(
            interfaceWd * 0.4 + audioProcessor.rangeMax_X * interfaceWd * 0.5,
            interfaceHt * 0.1,
            4,
            interfaceHt * 0.8
        );

        indicMovBounds_Y[1].setBounds(
            interfaceWd * 0.4,
            interfaceHt * 0.9 - audioProcessor.rangeMax_Y * interfaceHt * 0.8,
            interfaceWd * 0.5,
            4
        );

        indicUserPos.setBounds(
            interfaceWd * 0.4 + audioProcessor.userPos_X * interfaceWd * 0.5 - 5,
            interfaceHt * 0.9 - audioProcessor.userPos_Y * interfaceHt * 0.8 - 5,
            10,
            10
        );

        indicStartZone.toBack();
        indicTargetZone.toBack();
        indicUserPos.toFront(false);

        switch (trialState)
        {
        case 0:
            indicState.setText("Practice Mode", dontSendNotification);
            break;
        case 1:
            indicState.setText("New Target Set", dontSendNotification);
            break;
        case 2:
            indicState.setText("Trial Ongoing", dontSendNotification);
            break;
        case 3:
            indicState.setText("Trial Complete", dontSendNotification);
            break;
        }
    } 

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////

    int numZones = 12;

    float zoneStartEnds_X[20][2] =
    {
        {0.75, 1},
        {0.75, 1},
        {0.5, 0.75},
        {0.25, 0.5},
        {0, 0.25},
        {0, 0.25},
        {0, 0.25},
        {0, 0.25},
        {0.25, 0.5},
        {0.5, 0.75},
        {0.75, 1},
        {0.75, 1},
    };

    float zoneStartEnds_Y[20][2] =
    {
        {0.5, 0.75},
        {0.75, 1},
        {0.75, 1},
        {0.75, 1},
        {0.75, 1},
        {0.5, 0.75},
        {0.25, 0.5},
        {0, 0.25},
        {0, 0.25},
        {0, 0.25},
        {0, 0.25},
        {0.25, 0.5}
    };

    Random RAN_zoneSelector;
    Random RAN_withinZone;

    int zoneOrder[20];

    void setZoneOrder()
    {

        for (int a = 0; a < numZones; a++) zoneOrder[a] = -1;
        int randomIndex = RAN_zoneSelector.nextInt(numZones);
        bool alreadyDone = false;

        for (int k = 0; k < numZones; k++)
        {
            for (int i = 0; i < numZones; i++)
            {
                if (randomIndex == zoneOrder[i])
                    alreadyDone = true;
            }

            while (alreadyDone)
            {
                randomIndex = RAN_zoneSelector.nextInt(numZones);
                alreadyDone = false;
                for (int i = 0; i < numZones; i++)
                {
                    if (randomIndex == zoneOrder[i])
                        alreadyDone = true;
                }
            }

            zoneOrder[k] = randomIndex;
        }
    }

    void setNextTarget()
    {
        int trialNum_MOD = audioProcessor.trials_Complete % numZones;
        int currentZone = zoneOrder[trialNum_MOD];

        float zonePerc_X = (double)RAN_withinZone.nextDouble() * 0.95;
        float zonePerc_Y = (double)RAN_withinZone.nextDouble() * 0.95;

        float targetPos_X = zoneStartEnds_X[currentZone][0] + zonePerc_X *
            (zoneStartEnds_X[currentZone][1] - zoneStartEnds_X[currentZone][0]);

        float targetPos_Y = zoneStartEnds_Y[currentZone][0] + zonePerc_Y *
            (zoneStartEnds_Y[currentZone][1] - zoneStartEnds_Y[currentZone][0]);

        setTarget_X.setValue(targetPos_X);
        setTarget_Y.setValue(targetPos_Y);
    }

    void updateBoxLocation()
    {
        bool user_inBounds_StartX = false;
        bool user_inBounds_StartY = false;
        bool user_inBounds_TargetX = false;
        bool user_inBounds_TargetY = false;

        user_inBounds_StartX = indicUserPos.getX() > indicStartZone.getX() &&
            (indicUserPos.getX() + userPos_SidePx) < (indicStartZone.getX() + startZone_SidePx);

        user_inBounds_TargetX = indicUserPos.getX() > indicTargetZone.getX() &&
            (indicUserPos.getX() + userPos_SidePx) < (indicTargetZone.getX() + targetZone_SidePx);

        user_inBounds_StartY = indicUserPos.getY() > indicStartZone.getY() &&
            (indicUserPos.getY() + userPos_SidePx) < (indicStartZone.getY() + startZone_SidePx);

        user_inBounds_TargetY = indicUserPos.getY() > indicTargetZone.getY() &&
            (indicUserPos.getY() + userPos_SidePx) < (indicTargetZone.getY() + targetZone_SidePx);

        // ADD TO OTHERS
        audioProcessor.user_inTargetX = user_inBounds_TargetX;
        audioProcessor.user_inTargetY = user_inBounds_TargetY;
        // ADD TO OTHERS

        audioProcessor.user_inTargetBox = user_inBounds_TargetX && user_inBounds_TargetY;
        audioProcessor.user_inStartBox = user_inBounds_StartX && user_inBounds_StartY;

        

        //bool colourChangeZone = user_inTargetBox || user_inStartBox;
        //indicUserPos.setColour(indicUserPos.backgroundColourId, colourChangeZone ? Colours::black : Colours::white);
    }

    // SIMULATION KEYPRESSES
    KeyPress TrunkThigh_PlusMinus_RLUD[7];
    ModifierKeys *modKeys;

    void simulation_HandleKeyPresses()
    {
        bool isKeyDown[6] = { false, false, false, false, false, false };
        isKeyDown[0] = TrunkThigh_PlusMinus_RLUD[0].isKeyCurrentlyDown(KeyPress::rightKey);
        isKeyDown[1] = TrunkThigh_PlusMinus_RLUD[1].isKeyCurrentlyDown(KeyPress::leftKey);
        isKeyDown[2] = TrunkThigh_PlusMinus_RLUD[2].isKeyCurrentlyDown(KeyPress::upKey);
        isKeyDown[3] = TrunkThigh_PlusMinus_RLUD[3].isKeyCurrentlyDown(KeyPress::downKey);
        isKeyDown[4] = TrunkThigh_PlusMinus_RLUD[4].isKeyCurrentlyDown(KeyPress::spaceKey);
        isKeyDown[5] = TrunkThigh_PlusMinus_RLUD[5].isKeyCurrentlyDown(KeyPress::deleteKey);
        isKeyDown[6] = TrunkThigh_PlusMinus_RLUD[6].isKeyCurrentlyDown(KeyPress::pageDownKey);

        
        bool turbo = ModifierKeys::getCurrentModifiersRealtime().isShiftDown();

        short mode = 0;
        if (audioProcessor.guidanceMode == "Cartesian")     mode = 1;
        if (audioProcessor.guidanceMode == "Polar")     mode = 2;

        for (int i = 0; i < 7; i++)
        {
            if (isKeyDown[i])
            {
                switch (i)
                {
                case 0:
                    if (mode == 1) audioProcessor.userPos_X += (turbo) ? 0.02 : 0.0035;
                    //if (mode == 2 && (audioProcessor.userPos_DIST_C < 0.03)) 
                    if (mode == 2) 
                        audioProcessor.userPos_ANG_X -= (turbo) ? 5.625 : 180.0 / (40.0 * 6.0);
                    break;
                case 1:
                    if (mode == 1) audioProcessor.userPos_X -= (turbo) ? 0.02 : 0.0035;
                    //if (mode == 2 && (audioProcessor.userPos_DIST_C < 0.03)) 
                    if (mode == 2) 
                        audioProcessor.userPos_ANG_X += (turbo) ? 5.625 : 180.0 / (40.0 * 6.0);
                    break;
                case 2:
                    if (mode == 1) audioProcessor.userPos_Y += (turbo) ? 0.02 : 0.0035;
                    if (mode == 2) audioProcessor.userPos_DIST_C += (turbo) ? 0.02 : 0.0035;
                    break;
                case 3:
                    audioProcessor.userPos_Y -= (turbo) ? 0.02 : 0.0035;
                    if (mode == 2) audioProcessor.userPos_DIST_C -= (turbo) ? 0.02 : 0.0035;
                    break;
                case 4:
                    if (audioProcessor.timeSinceStateChange > 0.5 && !isMouseControl)
                    {
                        changeTrialState();
                    }
                    break;
                /*case 5:
                    if (mode == 2 && (audioProcessor.userPos_DIST_C < 0.03)) 
                        audioProcessor.userPos_ANG_X += 5.625;
                    break;
                case 6:
                    if (mode == 2 && (audioProcessor.userPos_DIST_C < 0.03))
                        audioProcessor.userPos_ANG_X -= 5.625;
                    break;*/
                }
            }

            audioProcessor.userPos_X = jlimit((double)audioProcessor.rangeMin_X, (double)audioProcessor.rangeMax_X, (double)audioProcessor.userPos_X);
            audioProcessor.userPos_Y = jlimit((double)audioProcessor.rangeMin_Y, (double)audioProcessor.rangeMax_Y, (double)audioProcessor.userPos_Y);
        }
    }

    int timerFreqHz = 40;
    void timerCallback();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (STS_Sim_2DGuidanceAudioProcessorEditor)
};
