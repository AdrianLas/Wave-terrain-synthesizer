#pragma once
#include "JuceHeader.h"

//==============================================================================
struct SineWaveSound : public juce::SynthesiserSound
{
	SineWaveSound() {}
	bool appliesToNote(int) override { return true; }
	bool appliesToChannel(int) override { return true; }
};
//==============================================================================
struct SineWaveVoice : public juce::SynthesiserVoice
{
	
	SineWaveVoice(const juce::AudioSampleBuffer& wavetableToUse, unsigned int voices,
		float* ptrA, float* ptrD, float* ptrS, float* ptrR, float* det, float* n, float* orbit) : wavetable(wavetableToUse)
	{
		globalLevel = 0.5 / (float)voices;
		jassert(wavetable.getNumChannels() == 1);
		voiceA = ptrA;
		voiceD = ptrD;
		voiceS = ptrS;
		voiceR = ptrR;
		detune = det;
		nvalue = n;
		orbitType = orbit;
	}

	void setFrequency(float frequency, float sampleRate)
	{
		float det =* detune;
		auto tableSizeOverSampleRate = wavetable.getNumSamples() / sampleRate;
		tableDelta = frequency * tableSizeOverSampleRate;
		tableDelta2 = (frequency*pow(2,(* detune/1200))) * tableSizeOverSampleRate;
		angleDelta = juce::MathConstants<float>::twoPi * frequency / sampleRate;
	}

	forcedinline float getNextSample() noexcept
	{
		auto tableSize = wavetable.getNumSamples();
		int switcher = (int)* orbitType;
		float currentSample = 0.0;
		//switch (switcher)
		//{
		//default:
		//	break;
		//}

		if (switcher == 1)
		{
			//sinus
			auto index0 = (unsigned int)currentIndex;
			auto index1 = index0 == (tableSize - 1) ? (unsigned int)0 : index0 + 1;
			auto frac = currentIndex - (float)index0;
			auto channel0 = (unsigned int)currentIndex2;
			auto channel1 = channel0 == (tableSize - 1) ? (unsigned int)0 : channel0 + 1;
			auto frac2 = currentIndex2 - (float)channel0;
			auto* table = wavetable.getReadPointer(channel0);
			auto* table2 = wavetable.getReadPointer(channel1);
			auto value0 = table[index0];
			auto value1 = table[index1];
			auto value2 = table2[index0];
			auto value3 = table2[index1];
			auto Sample = value0 + frac * (value1 - value0);
			auto Sample2 = value2 + frac * (value3 - value2);
			currentSample = Sample + frac * (Sample2 - Sample);
			if ((currentIndex += tableDelta) > tableSize)
				currentIndex -= tableSize;
			if ((currentIndex2 += tableDelta2) > tableSize)
			currentIndex2 -= tableSize;
		}
		else if (switcher == 2)
		{
			//rose
			float multiplier = *nvalue; // podmienić na wskaźnik do suwaka
			auto r = (tableSize / 2) * std::cos(multiplier * currentAngle);
			auto channel = std::round((tableSize / 2) + (r * sin(currentAngle)));
			if (channel == tableSize)
				channel = tableSize - 1;
			auto samplePos = std::round((tableSize / 2) + (r * cos(currentAngle)));
			if (samplePos == tableSize)
				samplePos = tableSize - 1;
			auto* table = wavetable.getReadPointer((unsigned int)channel);
			currentSample = table[(unsigned int)samplePos];
			if ((currentAngle += angleDelta) > 2 * juce::MathConstants<float>::pi)
				currentAngle -= 2 * juce::MathConstants<float>::pi;
		}
		else if (switcher == 3)
		{
			//butterfly
			auto r = (tableSize / 8) * (pow(juce::MathConstants<float>::euler,cos(currentAngle)) - (2*cos(4*currentAngle)) + (pow(sin(currentAngle/12),5)));
			auto channel = std::round((tableSize / 2) + (r * sin(currentAngle)));
			if (channel >= tableSize)
				channel = tableSize - 1;
			auto samplePos = std::round((tableSize / 2) + (r * cos(currentAngle)));
			if (samplePos >= tableSize)
				samplePos = tableSize - 1;
			auto* table = wavetable.getReadPointer((unsigned int)channel);
			currentSample = table[(unsigned int)samplePos];
			if ((currentAngle += angleDelta) > 2 * juce::MathConstants<float>::pi)
				currentAngle -= 2 * juce::MathConstants<float>::pi;
		}
		return currentSample;

		//{
		//auto index0 = (unsigned int)currentIndex;
		//auto index1 = index0 == (tableSize - 1) ? (unsigned int)0 : index0 + 1;
		//auto frac = currentIndex - (float)index0;
		//auto channel0 = (unsigned int)currentIndex2;
		//auto channel1 = channel0 == (tableSize - 1) ? (unsigned int)0 : channel0 + 1;
		//auto frac2 = currentIndex2 - (float)channel0;
		//auto* table = wavetable.getReadPointer(channel0);
		//auto* table2 = wavetable.getReadPointer(channel1);
		//auto value0 = table[index0];
		//auto value1 = table[index1];
		//auto value2 = table2[index0];
		//auto value3 = table2[index1];
		//auto Sample = value0 + frac * (value1 - value0);
		//auto Sample2 = value2 + frac * (value3 - value2);
		//auto currentSample = Sample + frac * (Sample2 - Sample);
		//if ((currentIndex += tableDelta) > tableSize)
		//	currentIndex -= tableSize;
		//if ((currentIndex2 += tableDelta2) > tableSize)
		//	currentIndex2 -= tableSize;
		//return currentSample;
		//}
		//else
		//{
		//	float temp = *nvalue; // podmienić na wskaźnik do suwaka
		//	auto r = (tableSize/2) * std::cos(temp * currentAngle);
		//	auto channel = std::round((tableSize/2) + (r * sin(currentAngle)));
		//	if (channel == tableSize)
		//		channel = tableSize - 1;
		//	auto samplePos = std::round((tableSize / 2) + (r * cos(currentAngle)));
		//	if (samplePos == tableSize)
		//		samplePos = tableSize - 1;
		//	auto* table = wavetable.getReadPointer((unsigned int)channel);
		//	float currentSample;
		//	currentSample = table[(unsigned int)samplePos];
		//	if ((currentAngle += angleDelta) > 2 * juce::MathConstants<float>::pi)
		//		currentAngle -= 2 * juce::MathConstants<float>::pi;
		//	return currentSample;
		//}
		//else
		//{
		//	float temp = *nvalue; // podmienić na wskaźnik do suwaka
		//	auto r = (tableSize / 8) * (pow(juce::MathConstants<float>::euler,cos(currentAngle)) - (2*cos(4*currentAngle)) + (pow(sin(currentAngle/12),5)));
		//	auto channel = std::round((tableSize / 2) + (r * sin(currentAngle)));
		//	if (channel >= tableSize)
		//		channel = tableSize - 1;
		//	auto samplePos = std::round((tableSize / 2) + (r * cos(currentAngle)));
		//	if (samplePos >= tableSize)
		//		samplePos = tableSize - 1;
		//	auto* table = wavetable.getReadPointer((unsigned int)channel);
		//	float currentSample;
		//	currentSample = table[(unsigned int)samplePos];
		//	if ((currentAngle += angleDelta) > 2 * juce::MathConstants<float>::pi)
		//		currentAngle -= 2 * juce::MathConstants<float>::pi;
		//	return currentSample;
		//}

	}


	bool canPlaySound(juce::SynthesiserSound* sound) override
	{
		return dynamic_cast<SineWaveSound*> (sound) != nullptr;
	}
	void startNote(int midiNoteNumber, float velocity,
		juce::SynthesiserSound*, int /*currentPitchWheelPosition*/) override
	{
		currentIndex = 0.0f;
		currentIndex2 = 0.0f;
		currentSegment = 1;
		currentPosInSegment = 0;
		level = velocity * globalLevel;
		tmpLevel = 0.0f;
		ampIncrease = level / (*voiceA);
		setFrequency(juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber), getSampleRate());
	}
	void stopNote(float /*velocity*/, bool allowTailOff) override
	{
		currentPosInSegment = 0;
		currentSegment = 4;
		ampIncrease = -1.0 * (tmpLevel / (*voiceR));

	}
	void pitchWheelMoved(int) override {}
	void controllerMoved(int, int) override {}
	void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample, int numSamples) override
	{
		if (currentSegment > 0)
		{
			while (--numSamples >= 0)
			{
				tmpLevel += ampIncrease;
				auto currentSample = getNextSample() * tmpLevel;
				for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
					outputBuffer.addSample(i, startSample, currentSample);
				++startSample;
				++currentPosInSegment;
				// A -> D
				if ((currentSegment == 1) && (currentPosInSegment > juce::roundToInt(*voiceA)))
				{
					currentPosInSegment = 0;
					currentSegment++;
					ampIncrease = -1.0 * (level * (1.0 - (*voiceS))) / (*voiceD);
				}
				// D -> S
				if ((currentSegment == 2) && (currentPosInSegment > juce::roundToInt(*voiceD)))
				{
					//
					// ...
					currentPosInSegment = 0;
					currentSegment++;
					ampIncrease = 0;

					tmpLevel = level * (*voiceS);
				}
				// R -> stop
				if ((currentSegment == 4) && (currentPosInSegment > juce::roundToInt(*voiceR)))
				{
					// ...
					currentPosInSegment = 0;
					currentSegment=0;
					//ampIncrease = -1.0 * (level * (*voiceS)) / (*voiceR);

					clearCurrentNote();
					tableDelta = 0.0f;
					break;
				}
			}
		}
	}
private:
	// *** double currentAngle = 0.0, angleDelta = 0.0
	//float tailOff = 0.0;

	int currentPosInSegment = 0;
	int currentSegment = 0;
	float ampIncrease = 0.0f, tmpLevel = 0.0f;
	const juce::AudioSampleBuffer& wavetable;
	float currentIndex = 0.0f, currentIndex2 = 0.0f, tableDelta = 0.0f, tableDelta2 = 0.0f, level = 0.0f, globalLevel = 0.0f, angleDelta = 0.0f, currentAngle = 0.0f;
	float* voiceA, * voiceD, * voiceS, * voiceR;
	float* detune;
	float* nvalue;
	float* orbitType;
};
//==============================================================================
class SynthAudioSource : public juce::AudioSource
{
public:
	
	SynthAudioSource(juce::MidiKeyboardState& keyState,
		float* ptrA, float* ptrD, float* ptrS, float* ptrR, float* ptrDetune, float* ptrN, float* orbit) : keyboardState(keyState)
	{
		for (auto i = 0; i < numVoices; ++i)
			synth.addVoice(new SineWaveVoice(signalTable, numVoices, ptrA, ptrD, ptrS, ptrR, ptrDetune, ptrN, orbit));
		synth.addSound(new SineWaveSound());
	}
	void setUsingSineWaveSound()
	{
		synth.clearSounds();
	}
	void prepareToPlay(int /*samplesPerBlockExpected*/, double sampleRate) override
	{
		synth.setCurrentPlaybackSampleRate(sampleRate);
		createWavetable(); // ***
	}
	void releaseResources() override {}
	void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
	{
		bufferToFill.clearActiveBufferRegion();
		juce::MidiBuffer incomingMidi;
		midiCollector.removeNextBlockOfMessages(incomingMidi, bufferToFill.numSamples);
		keyboardState.processNextMidiBuffer(incomingMidi, bufferToFill.startSample, bufferToFill.numSamples, true);
		synth.renderNextBlock(*bufferToFill.buffer, incomingMidi, bufferToFill.startSample, bufferToFill.numSamples);
	}
	juce::MidiMessageCollector* getMidiCollector()
	{
		return &midiCollector;
	}
	void createWavetable()
	{
		signalTable.setSize(tableSize, tableSize2);
		bleble = signalTable.getArrayOfWritePointers(); // dodane
		//signalTable.setSize(1, tableSize2);
		//auto* samples = signalTable.getWritePointer(0);
		auto angleDelta = juce::MathConstants<double>::twoPi / (double)(tableSize - 1);
		auto angleDelta2 = juce::MathConstants<double>::twoPi / (double)(tableSize2 - 1);
		auto currentAngle2 = 0.0;
		for (auto b = 0; b < tableSize; ++b)
		{
			auto* samples = signalTable.getWritePointer(b);
			auto currentAngle = 0.0;
			for (auto i = 0; i < tableSize; ++i)
			{
				//auto sample = std::sin(currentAngle);
				//auto sample = std::sin(currentAngle) * std::sin(currentAngle2);
				auto sample = std::sin(juce::MathConstants<double>::twoPi * 3 * i / tableSize) * std::sin(juce::MathConstants<double>::twoPi * 5 * b / tableSize);
				samples[i] = (float)sample;
				currentAngle += angleDelta;
			}
			currentAngle2 += angleDelta2;
		}
	}
	juce::MidiMessageCollector midiCollector;
	float** bleble; 
private:
	juce::MidiKeyboardState& keyboardState;
	juce::Synthesiser synth;
	int tableSize = 4096;// zmienione z 1024
	int tableSize2 = 4096;
	int numVoices = 4;
	juce::AudioSampleBuffer signalTable;

	//juce::AudioSampleBuffer signalTable;
};