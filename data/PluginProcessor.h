/*
  ==============================================================================

    PluginProcessor.h
    Created: 16 Nov 2025 8:45:04pm
    Author:  Gemini Code Assist

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <map>
#include <string>
#include <vector>

//==============================================================================
// A simple sine wave oscillator voice for our synth
class SynthVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound (juce::SynthesiserSound* sound) override { return true; }

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff) override;
    void pitchWheelMoved (int newPitchWheelValue) override {}
    void controllerMoved (int controllerNumber, int newControllerValue) override {}
    void renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

private:
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;
    double currentAngle = 0.0, angleDelta = 0.0, level = 0.0;
};

// A simple sound class that our voice can play
class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int midiNoteNumber) override { return true; }
    bool appliesToChannel (int midiChannel) override { return true; }
};


//==============================================================================
/**
*/
class Minor_shiftAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    Minor_shiftAudioProcessor();
    ~Minor_shiftAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // Add APVTS for parameter management
    juce::AudioProcessorValueTreeState apvts;

private:
    juce::Synthesiser synth;

    //==============================================================================
    // MINOR_SHIFT Core Logic Ported to C++
    //==============================================================================
    std::vector<int> getChordNotes(const juce::String& chordName, int octave);
    std::vector<std::vector<int>> generateProgression(const juce::String& key, int rootNote);
    std::vector<juce::MidiMessageMetadata> generateMelody(const std::vector<std::vector<int>>& chordProgression, juce::uint8 velocity);

    // Helper function to create the layout of parameters
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Minor_shiftAudioProcessor)
};