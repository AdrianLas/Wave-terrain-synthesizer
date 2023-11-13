/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GeneratorADSRAudioProcessor::GeneratorADSRAudioProcessor() : synthAudioSource(keyboardState, &procA, &procD, &procS, &procR, &procDetune, &procN, &orbitT)
#ifndef JucePlugin_PreferredChannelConfigurations
, AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
    .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
    .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
)

#endif
{
}

GeneratorADSRAudioProcessor::~GeneratorADSRAudioProcessor()
{
}

//==============================================================================
const juce::String GeneratorADSRAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GeneratorADSRAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool GeneratorADSRAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool GeneratorADSRAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double GeneratorADSRAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GeneratorADSRAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int GeneratorADSRAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GeneratorADSRAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String GeneratorADSRAudioProcessor::getProgramName(int index)
{
    return {};
}

void GeneratorADSRAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void GeneratorADSRAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synthAudioSource.prepareToPlay(samplesPerBlock, sampleRate);
    synthAudioSource.midiCollector.reset(sampleRate);
}

void GeneratorADSRAudioProcessor::releaseResources()
{
    synthAudioSource.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GeneratorADSRAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void GeneratorADSRAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::MidiMessage m;
    int time;
    for (juce::MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);)
        synthAudioSource.midiCollector.addMessageToQueue(m);
    juce::AudioSourceChannelInfo channelInfo = juce::AudioSourceChannelInfo(buffer);
    synthAudioSource.getNextAudioBlock(channelInfo);
}

//==============================================================================
bool GeneratorADSRAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GeneratorADSRAudioProcessor::createEditor()
{
    return new GeneratorADSRAudioProcessorEditor(*this);
}

//==============================================================================
void GeneratorADSRAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void GeneratorADSRAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GeneratorADSRAudioProcessor();
}
void GeneratorADSRAudioProcessor::modifyWavetable(float** writePointers, bool isPeriodic) 
{   
    auto tableSize = 1024;
    auto** tablePointers = writePointers;
    //signalTable.setSize(1, tableSize2);
    //auto* samples = signalTable.getWritePointer(0);
    auto angleDelta = juce::MathConstants<double>::twoPi / (double)(tableSize - 1);
    auto angleDelta2 = juce::MathConstants<double>::twoPi / (double)(tableSize - 1);
    auto currentAngle2 = 0.0;
    if (isPeriodic)
    {
        for (auto b = 0; b < tableSize; ++b)
        {
            auto* samples = tablePointers[b];
            auto currentAngle = 0.0;
            for (auto i = 0; i < tableSize; ++i)
            {
                //auto sample = std::sin(currentAngle);
                //auto sample = std::sin(currentAngle) * std::sin(currentAngle2);
                auto sample = std::sin(juce::MathConstants<double>::twoPi * procFreq1 * i / tableSize) * std::sin(juce::MathConstants<double>::twoPi * procFreq2 * b / tableSize);
                samples[i] = (float)sample;
                currentAngle += angleDelta;
            }
            currentAngle2 += angleDelta2;
        }
    }
    else if (isRandom)
    {
        srand(time(NULL));
        for (auto b = 0; b < tableSize; ++b)
        {
            auto* samples = tablePointers[b];
            for (auto i = 0; i < tableSize; ++i)
            {
                auto sample = rand() % 1000 / 1000.0;
                if (b != 0 && b != tableSize - 1 && i != 0 && i != tableSize - 1)
                    samples[i] = (float)sample;
                else
                    samples[i] = 0;
            }
        }
    }
    else if (isImage)
    {
        float normalizer = 0.0;
        for (auto b = 0; b < tableSize; ++b)
        {
            auto* samples = tablePointers[b];
            for (auto i = 0; i < tableSize; ++i)
            {
                auto color = imageHolder.getPixelAt(i, b);
                auto sample = color.getBrightness() *2.0 - 1;
                if (std::abs(sample) > normalizer)
                    normalizer = sample;
                samples[i] = (float)sample;
            }
        }
        for (auto b = 0; b < tableSize; ++b)
        {
            auto* samples = tablePointers[b];
            for (auto i = 0; i < tableSize; ++i)
            {
                samples[i] = samples[i] / normalizer;
            }
        }
    }

}
