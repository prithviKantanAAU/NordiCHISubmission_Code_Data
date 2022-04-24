#include "PluginProcessor.h"
#include "PluginEditor.h"

STS_Sim_2DGuidanceAudioProcessorEditor::STS_Sim_2DGuidanceAudioProcessorEditor (STS_Sim_2DGuidanceAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (interfaceWd, interfaceHt);
    modKeys = new ModifierKeys();
    setZoneOrder();
    addAllElements();
    configSliders();
    configButtons();
    configIndicators();
    startTimerHz(timerFreqHz);
    
    indicUserPos.addMouseListener(this,false);
}

STS_Sim_2DGuidanceAudioProcessorEditor::~STS_Sim_2DGuidanceAudioProcessorEditor()
{
}

void STS_Sim_2DGuidanceAudioProcessorEditor::mouseDoubleClick(const MouseEvent& event)
{
    if (isMouseControl) changeTrialState();    
}

void STS_Sim_2DGuidanceAudioProcessorEditor::comboBoxChanged(ComboBox *box)
{
    if (box == &setGuidanceMode)
    {
        audioProcessor.guidanceMode = setGuidanceMode.getText();
    }

    if (box == &setStrategyMode)
    {
        audioProcessor.strategyMode = setStrategyMode.getText();
    }
}

void STS_Sim_2DGuidanceAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);

    // Divider b/w controls and graphic
    juce::Line<float> divider(juce::Point<float>(interfaceWd * 0.3, 0),
        juce::Point<float>(interfaceWd * 0.3, interfaceHt));
    g.drawLine(divider, 2.0f);

    // Divider between Manual controls and trial info
    juce::Line<float> divider2(juce::Point<float>(0, interfaceHt * 0.25),
        juce::Point<float>(interfaceWd * 0.3, interfaceHt * 0.25));
    g.drawLine(divider2, 2.0f);

    // Divider between Manual controls and trial controls
    juce::Line<float> divider3(juce::Point<float>(0, interfaceHt * 0.5),
        juce::Point<float>(interfaceWd * 0.3, interfaceHt * 0.5));
    g.drawLine(divider3, 2.0f);

    // Rectangle for 2D task
    juce::Path path;
    juce::Line<float> taskRect_TOP(juce::Point<float>(interfaceWd * 0.4, interfaceHt * 0.1),
        juce::Point<float>(interfaceWd * 0.9, interfaceHt * 0.1));
    juce::Line<float> taskRect_DOWN(juce::Point<float>(interfaceWd * 0.4, interfaceHt * 0.9),
        juce::Point<float>(interfaceWd * 0.9, interfaceHt * 0.9));
    juce::Line<float> taskRect_LEFT(juce::Point<float>(interfaceWd * 0.4, interfaceHt * 0.1),
        juce::Point<float>(interfaceWd * 0.4, interfaceHt * 0.9));
    juce::Line<float> taskRect_RIGHT(juce::Point<float>(interfaceWd * 0.9, interfaceHt * 0.1),
        juce::Point<float>(interfaceWd * 0.9, interfaceHt * 0.9));
    g.drawLine(taskRect_TOP, 2.0f);
    g.drawLine(taskRect_DOWN, 2.0f);
    g.drawLine(taskRect_LEFT, 2.0f);
    g.drawLine(taskRect_RIGHT, 2.0f);
}

void STS_Sim_2DGuidanceAudioProcessorEditor::timerCallback()
{
    simulation_HandleKeyPresses();
    if (isMouseControl && audioProcessor.isTrial_ON) handleMouseDrag_Target();
    updateVisualizer();
    angleIndicator_setVisible(audioProcessor.guidanceMode == "Polar");
    angleIndicator_UPDATE();
    updateBoxLocation();
}

void STS_Sim_2DGuidanceAudioProcessorEditor::resized()
{
    setTarget_X.setBounds(interfaceWd * 0.39, interfaceHt * 0.05, interfaceWd * 0.52, interfaceHt * 0.05);
    setTarget_Y.setBounds(interfaceWd * 0.9, interfaceHt * 0.09, interfaceWd * 0.05, interfaceHt * 0.82);

    //setMovBounds_X.setBounds(interfaceWd * 0.39, interfaceHt * 0.9, interfaceWd * 0.52, interfaceHt * 0.05);
    //setMovBounds_Y.setBounds(interfaceWd * 0.35, interfaceHt * 0.09, interfaceWd * 0.05, interfaceHt * 0.82);

    reset_positions.setBounds(interfaceWd * 0.02, interfaceHt * 0.02, interfaceWd * 0.26, interfaceHt * 0.05);
    hide_targetControls.setBounds(interfaceWd * 0.02, interfaceHt * 0.09, interfaceWd * 0.26, interfaceHt * 0.05);
    set_randomTarget.setBounds(interfaceWd * 0.02, interfaceHt * 0.16, interfaceWd * 0.26, interfaceHt * 0.05);

    trials_Complete.setBounds(0, interfaceHt * 0.27, interfaceWd * 0.3, 20);
    //set_mouseControl.setBounds(interfaceWd * 0.02, interfaceHt * 0.39, interfaceWd * 0.26, interfaceHt * 0.05);
    indic_targetsFound.setBounds(interfaceWd * 0.02, interfaceHt * 0.39, interfaceWd * 0.26, interfaceHt * 0.05);
    trials_Reset.setBounds(interfaceWd * 0.02, interfaceHt * 0.32, interfaceWd * 0.26, interfaceHt * 0.05);

    startTrial.setBounds(interfaceWd * 0.02, interfaceHt * 0.52, interfaceWd * 0.26, interfaceHt * 0.05);
    setGuidanceMode.setBounds(interfaceWd * 0.02, interfaceHt * 0.59, interfaceWd * 0.26, interfaceHt * 0.05);
    setStrategyMode.setBounds(interfaceWd * 0.02, interfaceHt * 0.66, interfaceWd * 0.26, interfaceHt * 0.05);
    recordTrials.setBounds(interfaceWd * 0.02, interfaceHt * 0.92, interfaceWd * 0.26, interfaceHt * 0.05);

    indicState.setBounds(interfaceWd * 0.4 - 50, 0, interfaceWd * 0.6, 30);
    showTimeElapsed.setBounds(interfaceWd * 0.4 - 50, interfaceHt-30, interfaceWd * 0.6, 30);
}
