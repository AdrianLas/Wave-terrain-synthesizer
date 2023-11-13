/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GeneratorADSRAudioProcessorEditor::GeneratorADSRAudioProcessorEditor(GeneratorADSRAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), keyboardComponent(audioProcessor.keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    addAndMakeVisible(keyboardComponent);
    setSize(600, 580);
    startTimer(400);

    //zmienic z 0.001 na 1 itp. (sekundy na mili)
    sliderA.setRange(1, 250, 1);
    sliderA.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    sliderA.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderA.setPopupDisplayEnabled(true, false, this);
    sliderA.setTextValueSuffix("ms Attack");
    sliderA.setValue(25);
    addAndMakeVisible(&sliderA);
    sliderA.addListener(this);

    sliderD.setRange(1, 500, 1);
    sliderD.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    sliderD.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderD.setPopupDisplayEnabled(true, false, this);
    sliderD.setTextValueSuffix("ms Decay");
    sliderD.setValue(100);
    addAndMakeVisible(&sliderD);
    sliderD.addListener(this);

    sliderS.setRange(0, 100, 1);
    sliderS.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    sliderS.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderS.setPopupDisplayEnabled(true, false, this);
    sliderS.setTextValueSuffix("% Sustain");
    sliderS.setValue(30);
    addAndMakeVisible(&sliderS);
    sliderS.addListener(this);

    sliderR.setRange(10, 4000, 10);
    sliderR.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    sliderR.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sliderR.setPopupDisplayEnabled(true, false, this);
    sliderR.setTextValueSuffix("ms Release");
    sliderR.setValue(500);
    addAndMakeVisible(&sliderR);
    sliderR.addListener(this);

    sliderDetune.setRange(0, 10, 1);
    sliderDetune.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    sliderDetune.setPopupDisplayEnabled(true, false, this);
    sliderDetune.setTextValueSuffix("Detune");
    sliderDetune.setValue(0);
    addAndMakeVisible(&sliderDetune);
    sliderDetune.addListener(this);


    ceilN.setRange(0, 10, 1);
    ceilN.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    ceilN.setPopupDisplayEnabled(true, false, this);
    ceilN.setTextValueSuffix("N ceil");
    ceilN.setValue(0);
    addAndMakeVisible(&ceilN);
    ceilN.addListener(this);

    floorN.setRange(0, 100, 1);
    floorN.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    floorN.setPopupDisplayEnabled(true, false, this);
    floorN.setTextValueSuffix("N floor");
    floorN.setValue(0);
    addAndMakeVisible(&floorN);
    floorN.addListener(this);

    submit.setButtonText("Generate");
    submit.onClick = [this] {GeneratorADSRAudioProcessorEditor::submitClicked(); };
    addAndMakeVisible(&submit);

    addAndMakeVisible(&readList);
    readList.addItem("Random", 1);
    readList.addItem("Periodic", 2);
    readList.addItem("Image", 3);
    readList.setSelectedId(2);
    readList.onChange = [this] { readListChanged(); };

    addAndMakeVisible(&orbitList);
    orbitList.addItem("Sinus", 1);
    orbitList.addItem("Rose Curve", 2);
    orbitList.addItem("Butterfly Curve", 3);
    orbitList.setSelectedId(1);
    orbitList.onChange = [this] { orbitListChanged(); };

    ceilfreq1.setRange(0, 10, 1);
    ceilfreq1.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    ceilfreq1.setPopupDisplayEnabled(true, false, this);
    ceilfreq1.setTextValueSuffix("Freq1 ceil");
    ceilfreq1.setValue(1);
    addAndMakeVisible(&ceilfreq1);

    ceilfreq2.setRange(0, 10, 1);
    ceilfreq2.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    ceilfreq2.setPopupDisplayEnabled(true, false, this);
    ceilfreq2.setTextValueSuffix("Freq2 ceil");
    ceilfreq2.setValue(1);
    addAndMakeVisible(&ceilfreq2);
    
    floorfreq1.setRange(0, 99, 1);
    floorfreq1.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    floorfreq1.setPopupDisplayEnabled(true, false, this);
    floorfreq1.setTextValueSuffix("Freq1 floor");
    floorfreq1.setValue(0);
    addAndMakeVisible(&floorfreq1);

    floorfreq2.setRange(0, 99, 1);
    floorfreq2.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    floorfreq2.setPopupDisplayEnabled(true, false, this);
    floorfreq2.setTextValueSuffix("Freq2 floor");
    floorfreq2.setValue(0);
    addAndMakeVisible(&floorfreq2);

    openFileButton.setButtonText("Choose File:");
    openFileButton.onClick = [this] {openFileButtonClicked(); };
    addAndMakeVisible(&openFileButton);

    //labels
    //ADSR
    labelA.setText("Attack", juce::NotificationType::dontSendNotification);
    labelA.setJustificationType(juce::Justification::centredBottom);
    labelD.setText("Decay",juce::NotificationType::dontSendNotification);
    labelD.setJustificationType(juce::Justification::centredBottom);
    labelS.setText("Sustain", juce::NotificationType::dontSendNotification);
    labelS.setJustificationType(juce::Justification::centredBottom);
    labelR.setText("Release", juce::NotificationType::dontSendNotification);
    labelR.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&labelA);
    addAndMakeVisible(&labelD);
    addAndMakeVisible(&labelS);
    addAndMakeVisible(&labelR);

    //Orbit
    orbitListLabel.setText("Orbit Type", juce::NotificationType::dontSendNotification);
    orbitListLabel.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&orbitListLabel);
    sliderDetuneLabel.setText("Sinus Detune", juce::NotificationType::dontSendNotification);
    sliderDetuneLabel.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&sliderDetuneLabel);
    ceilNLabel.setText("N Integer Value", juce::NotificationType::dontSendNotification);
    ceilNLabel.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&ceilNLabel);
    floorNLabel.setText("N Mantissa Value", juce::NotificationType::dontSendNotification);
    floorNLabel.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&floorNLabel);


    readListLabel.setText("Wavetable Type", juce::NotificationType::dontSendNotification);
    readListLabel.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&readListLabel);
    openFileButtonLabel.setText("File Path:", juce::NotificationType::dontSendNotification);
    openFileButtonLabel.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&openFileButtonLabel);
    submitLabel.setText("Confirm changes", juce::NotificationType::dontSendNotification);
    submitLabel.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&submitLabel);

    ceilfreq1Label.setText("Freq Integer Value", juce::NotificationType::dontSendNotification);
    ceilfreq1Label.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&ceilfreq1Label);
    floorfreq1Label.setText("Freq Mantissa Value", juce::NotificationType::dontSendNotification);
    floorfreq1Label.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&floorfreq1Label);
    ceilfreq2Label.setText("Freq Integer Value", juce::NotificationType::dontSendNotification);
    ceilfreq2Label.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&ceilfreq2Label);
    floorfreq2Label.setText("Freq Mantissa Value", juce::NotificationType::dontSendNotification);
    floorfreq2Label.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&floorfreq2Label);
    freq1Label.setText("X-axis", juce::NotificationType::dontSendNotification);
    freq1Label.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&freq1Label);
    freq2Label.setText("Y-axis", juce::NotificationType::dontSendNotification);
    freq2Label.setJustificationType(juce::Justification::centredBottom);
    addAndMakeVisible(&freq2Label);
}

GeneratorADSRAudioProcessorEditor::~GeneratorADSRAudioProcessorEditor()
{
}

//==============================================================================
void GeneratorADSRAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    //g.drawFittedText("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void GeneratorADSRAudioProcessorEditor::resized()
{
    keyboardComponent.setBounds(10, 10, 580, 140);
    //ADSR components
    sliderA.setBounds(40, 180, 100, 100);
    sliderD.setBounds(180, 180, 100, 100);
    sliderS.setBounds(320, 180, 100, 100);
    sliderR.setBounds(460, 180, 100, 100);
    labelA.setBounds(40, 145, 100, 40);
    labelD.setBounds(180, 145, 100, 40);
    labelS.setBounds(320, 145, 100, 40);
    labelR.setBounds(460, 145, 100, 40);

    //orbits components
    orbitList.setBounds(getWidth() / 2 - 50, 300, 100, 50);
    sliderDetune.setBounds(getWidth()/2 - 70, 355, 140, 70);
    ceilN.setBounds(100, 355, 140, 70);
    floorN.setBounds(360, 355, 140, 70);
    orbitListLabel.setBounds(getWidth() / 2 - 50, 277, 100, 20);
    sliderDetuneLabel.setBounds(getWidth() / 2 - 70, 365, 140, 20);
    ceilNLabel.setBounds(100, 365, 140, 20);
    floorNLabel.setBounds(360, 365, 140, 20);

    //wavetable components
    submit.setBounds(getWidth() - 130, 475, 100, 50);
    readList.setBounds(30, 475, 100, 50);
    openFileButton.setBounds(getWidth() / 2 - 75, 500, 150, 50);
    submitLabel.setBounds(getWidth() - 130, 435, 100, 40);
    readListLabel.setBounds(30, 435, 100, 40);
    openFileButtonLabel.setBounds(getWidth() / 2 - 75, 460, 150, 40);

    ceilfreq1.setBounds(150, 445, 140, 70);
    ceilfreq2.setBounds(310, 445, 140, 70);
    floorfreq1.setBounds(150, 490, 140, 70);
    floorfreq2.setBounds(310, 490, 140, 70);
    ceilfreq1Label.setBounds(150, 450, 140, 20);
    ceilfreq2Label.setBounds(310, 450, 140, 20);
    floorfreq1Label.setBounds(150, 495, 140, 20);
    floorfreq2Label.setBounds(310, 495, 140, 20);
    freq1Label.setBounds(150, 430, 140, 20);
    freq2Label.setBounds(310, 430, 140, 20);

    

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void GeneratorADSRAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    float sr = (float)audioProcessor.getSampleRate();
    audioProcessor.procA = sr * (sliderA.getValue() / 1000.0f);
    audioProcessor.procD = sr * (sliderD.getValue() / 1000.0f);
    audioProcessor.procS = sliderS.getValue() / 100.0f;
    audioProcessor.procR = sr * (sliderR.getValue() / 1000.0f);
    audioProcessor.procDetune = sliderDetune.getValue();
    audioProcessor.procN = ceilN.getValue() + floorN.getValue() /100;
}
void GeneratorADSRAudioProcessorEditor::submitClicked()
{
    //if (sliderDetune.getValue() == 0)
    //    sliderDetune.setValue(10);
    //else
    //    sliderDetune.setValue(0);
    audioProcessor.procFreq1 = ceilfreq1.getValue() + floorfreq1.getValue() / 100;
    audioProcessor.procFreq2 = ceilfreq2.getValue() + floorfreq2.getValue() / 100;
    //freq1.setText(std::to_string(audioProcessor.procFreq1), juce::dontSendNotification);
    audioProcessor.modifyWavetable(audioProcessor.synthAudioSource.bleble, audioProcessor.isPeriodic);
    keyboardComponent.grabKeyboardFocus();
}
void GeneratorADSRAudioProcessorEditor::readListChanged()
{
    switch(readList.getSelectedId())
    {
    case 1: 
        openFileButton.setEnabled(false);
        openFileButtonLabel.setEnabled(false);
        ceilfreq1.setEnabled(false); 
        ceilfreq2.setEnabled(false);
        floorfreq1.setEnabled(false);
        floorfreq2.setEnabled(false);
        ceilfreq1Label.setEnabled(false);
        ceilfreq2Label.setEnabled(false);
        floorfreq1Label.setEnabled(false);
        floorfreq2Label.setEnabled(false);
        freq1Label.setEnabled(false);
        freq2Label.setEnabled(false);

        openFileButton.setVisible(false);
        openFileButtonLabel.setVisible(false);
        ceilfreq1.setVisible(false);
        ceilfreq2.setVisible(false);
        floorfreq1.setVisible(false);
        floorfreq2.setVisible(false);
        ceilfreq1Label.setVisible(false);
        ceilfreq2Label.setVisible(false);
        floorfreq1Label.setVisible(false);
        floorfreq2Label.setVisible(false);
        freq1Label.setVisible(false);
        freq2Label.setVisible(false);


        audioProcessor.isPeriodic = false;
        audioProcessor.isImage = false;
        audioProcessor.isRandom = true;
        break;
    case 2:         
        openFileButton.setEnabled(false);
        openFileButtonLabel.setEnabled(false);
        ceilfreq1.setEnabled(true);
        ceilfreq2.setEnabled(true);
        floorfreq1.setEnabled(true);
        floorfreq2.setEnabled(true);
        ceilfreq1Label.setEnabled(true);
        ceilfreq2Label.setEnabled(true);
        floorfreq1Label.setEnabled(true);
        floorfreq2Label.setEnabled(true);
        freq1Label.setEnabled(true);
        freq2Label.setEnabled(true);

        openFileButton.setVisible(false);
        openFileButtonLabel.setVisible(false);
        ceilfreq1.setVisible(true);
        ceilfreq2.setVisible(true);
        floorfreq1.setVisible(true);
        floorfreq2.setVisible(true);
        ceilfreq1Label.setVisible(true);
        ceilfreq2Label.setVisible(true);
        floorfreq1Label.setVisible(true);
        floorfreq2Label.setVisible(true);
        freq1Label.setVisible(true);
        freq2Label.setVisible(true);

        audioProcessor.isPeriodic = true;
        audioProcessor.isImage = false;
        audioProcessor.isRandom = false;
        break;
    case 3:
        openFileButton.setEnabled(true);
        openFileButtonLabel.setEnabled(true);
        ceilfreq1.setEnabled(false);
        ceilfreq2.setEnabled(false);
        floorfreq1.setEnabled(false);
        floorfreq2.setEnabled(false);
        ceilfreq1Label.setEnabled(false);
        ceilfreq2Label.setEnabled(false);
        floorfreq1Label.setEnabled(false);
        floorfreq2Label.setEnabled(false);
        freq1Label.setEnabled(false);
        freq2Label.setEnabled(false);

        openFileButton.setVisible(true);
        openFileButtonLabel.setVisible(true);
        ceilfreq1.setVisible(false);
        ceilfreq2.setVisible(false);
        floorfreq1.setVisible(false);
        floorfreq2.setVisible(false);
        ceilfreq1Label.setVisible(false);
        ceilfreq2Label.setVisible(false);
        floorfreq1Label.setVisible(false);
        floorfreq2Label.setVisible(false);
        freq1Label.setVisible(false);
        freq2Label.setVisible(false);

        audioProcessor.isPeriodic = false;
        audioProcessor.isImage = true;
        audioProcessor.isRandom = false;
        break;
    default:
        break;
    }
    
}

void GeneratorADSRAudioProcessorEditor::orbitListChanged()
{
    switch (orbitList.getSelectedId())
    {
    case 1://sinus
        audioProcessor.orbitT = orbitList.getSelectedId();

        sliderDetune.setEnabled(true);
        floorN.setEnabled(false);
        ceilN.setEnabled(false);
        sliderDetuneLabel.setEnabled(true);
        floorNLabel.setEnabled(false);
        ceilNLabel.setEnabled(false);

        sliderDetune.setVisible(true);
        floorN.setVisible(false);
        ceilN.setVisible(false);
        sliderDetuneLabel.setVisible(true);
        floorNLabel.setVisible(false);
        ceilNLabel.setVisible(false);

        break;
    case 2://rose
        audioProcessor.orbitT = orbitList.getSelectedId();

        sliderDetune.setEnabled(false);
        floorN.setEnabled(true);
        ceilN.setEnabled(true);
        sliderDetuneLabel.setEnabled(false);
        floorNLabel.setEnabled(true);
        ceilNLabel.setEnabled(true);

        sliderDetune.setVisible(false);
        floorN.setVisible(true);
        ceilN.setVisible(true);
        sliderDetuneLabel.setVisible(false);
        floorNLabel.setVisible(true);
        ceilNLabel.setVisible(true);

        break;
    case 3://butterfly
        audioProcessor.orbitT = orbitList.getSelectedId();

        sliderDetune.setEnabled(false);
        floorN.setEnabled(false);
        ceilN.setEnabled(false);
        sliderDetuneLabel.setEnabled(false);
        floorNLabel.setEnabled(false);
        ceilNLabel.setEnabled(false);

        sliderDetune.setVisible(false);
        floorN.setVisible(false);
        ceilN.setVisible(false);
        sliderDetuneLabel.setVisible(false);
        floorNLabel.setVisible(false);
        ceilNLabel.setVisible(false);

        break;
    default:
        break;
    }
    keyboardComponent.grabKeyboardFocus();
}

void GeneratorADSRAudioProcessorEditor::openFileButtonClicked()
{
    juce::FileChooser chooser("Choose image to open", juce::File::getSpecialLocation(juce::File::userDesktopDirectory), "*.png","*.jpg", true, false);
    if(chooser.browseForFileToOpen())
    { 
        juce::File myFile;
        juce::Image image = juce::ImageFileFormat::loadFrom(chooser.getResult());
        image = image.rescaled(4096, 4096);//zmieniono z 1024
        audioProcessor.imageHolder = image;

    }

}