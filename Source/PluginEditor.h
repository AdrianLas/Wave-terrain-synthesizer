/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class GeneratorADSRAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer, private juce::Slider:: Listener
{
public:
    GeneratorADSRAudioProcessorEditor(GeneratorADSRAudioProcessor&);
    ~GeneratorADSRAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GeneratorADSRAudioProcessor& audioProcessor;
    void timerCallback() override
    {
        keyboardComponent.grabKeyboardFocus();
        stopTimer();
    }
    juce::MidiKeyboardComponent keyboardComponent;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GeneratorADSRAudioProcessorEditor)
        void sliderValueChanged(juce::Slider* slider) override;
    void submitClicked();
    void readListChanged();
    void orbitListChanged();
    void openFileButtonClicked();

    juce::Slider sliderA;
    juce::Slider sliderD;
    juce::Slider sliderS;
    juce::Slider sliderR;
    juce::Label labelA;
    juce::Label labelD;
    juce::Label labelS;
    juce::Label labelR;


    juce::ComboBox orbitList;
    juce::Slider sliderDetune;
    juce::Slider ceilN;
    juce::Slider floorN;
    juce::Label orbitListLabel;
    juce::Label sliderDetuneLabel;
    juce::Label ceilNLabel;
    juce::Label floorNLabel;


    juce::ComboBox readList;
    juce::TextButton openFileButton;
    juce::TextButton submit;
    juce::Label readListLabel;
    juce::Label openFileButtonLabel;
    juce::Label submitLabel;

    juce::Slider ceilfreq1;
    juce::Slider ceilfreq2;
    juce::Slider floorfreq1;
    juce::Slider floorfreq2;
    juce::Label ceilfreq1Label;
    juce::Label ceilfreq2Label;
    juce::Label floorfreq1Label;
    juce::Label floorfreq2Label;
    juce::Label freq1Label;
    juce::Label freq2Label;




};
