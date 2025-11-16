/*
  ==============================================================================

    PluginProcessor.cpp
    Created: 16 Nov 2025 8:45:04pm
    Author:  Gemini Code Assist

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Minor_shiftAudioProcessor::Minor_shiftAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
    , apvts(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
    // Add our synth sound and voices
    synth.addSound(new SynthSound());
    for (int i = 0; i < 4; ++i) // 4 voices for polyphony
        synth.addVoice(new SynthVoice());
}

Minor_shiftAudioProcessor::~Minor_shiftAudioProcessor()
{
}

//==============================================================================
const juce::String Minor_shiftAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Minor_shiftAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Minor_shiftAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Minor_shiftAudioProcessor::isMidiEffect() const
{
   // #if JucePlugin_IsMidiEffect
   //  return true;
   // #else
    return false;
   // #endif
}

double Minor_shiftAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Minor_shiftAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Minor_shiftAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Minor_shiftAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Minor_shiftAudioProcessor::getProgramName (int index)
{
    return {};
}

void Minor_shiftAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Minor_shiftAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    synth.setCurrentPlaybackSampleRate(sampleRate);
}

void Minor_shiftAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Minor_shiftAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
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

void Minor_shiftAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    // This is now a synth, so we don't clear the audio buffer at the start.
    // We will render our synth output into it.
    buffer.clear();

    // We still want to output MIDI for chaining instruments
    juce::MidiBuffer processedMidi; 
    int time;
    juce::MidiMessage msg;

    for (const auto metadata : midiMessages)
    {
        msg = metadata.getMessage();
        time = metadata.samplePosition;

        if (msg.isNoteOn())
        {
            int rootNote = msg.getNoteNumber();
            auto keyParam = static_cast<juce::AudioParameterChoice*>(apvts.getParameter("KEY"));
            juce::String selectedKey = keyParam->getCurrentChoiceName();

            auto progression = generateProgression(selectedKey, rootNote);

            auto melody = generateMelody(progression, msg.getVelocity());

            double samplesPerBeat = getSampleRate() / (getPlayHead()->getPosition()->getBpm().orFallback(120.0) / 60.0);
            int chordLengthInSamples = static_cast<int>(samplesPerBeat);

            int chordStartTime = time;
            for (const auto& chord : progression)
            {
                for (int note : chord)
                {
                    // Add Note On for MIDI output
                    processedMidi.addEvent(juce::MidiMessage::noteOn(msg.getChannel(), note, (juce::uint8)msg.getVelocity()), chordStartTime);
                }
                for (int note : chord)
                {
                    // Add Note Off for MIDI output
                    processedMidi.addEvent(juce::MidiMessage::noteOff(msg.getChannel(), note), chordStartTime + chordLengthInSamples);
                }
                chordStartTime += chordLengthInSamples;
            }

            // Add generated melody to the MIDI output
            for (const auto& melodyMsg : melody)
            {
                processedMidi.addEvent(melodyMsg.getMessage(), melodyMsg.getSamplePosition());
            }
        }
        else
        {
            processedMidi.addEvent(msg, time);
        }
    }

    // The synth now renders audio from our *processed* MIDI buffer, which contains the chords and melody
    synth.renderNextBlock(buffer, processedMidi, 0, buffer.getNumSamples());

    // We swap our generated MIDI (chords + melody) to be the final output
    midiMessages.swapWith(processedMidi);
}

//==============================================================================
// Synth Voice Implementation
//==============================================================================

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    currentAngle = 0.0;
    level = velocity * 0.15;
    auto cyclesPerSecond = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    auto cyclesPerSample = cyclesPerSecond / getSampleRate();
    angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;

    adsrParams = { 0.1f, 0.1f, 1.0f, 0.3f }; // Attack, Decay, Sustain, Release
    adsr.setParameters(adsrParams);
    adsr.noteOn();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    adsr.noteOff();
    if (!allowTailOff)
    {
        clearCurrentNote();
        angleDelta = 0.0;
    }
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (angleDelta != 0.0)
    {
        while (--numSamples >= 0)
        {
            auto currentSample = (float)(std::sin(currentAngle) * level * adsr.getNextSample());

            for (int i = outputBuffer.getNumChannels(); --i >= 0;)
                outputBuffer.addSample(i, startSample, currentSample);

            currentAngle += angleDelta;
            ++startSample;
        }
    }
}

//==============================================================================
// MINOR_SHIFT Core Logic Implementation
//==============================================================================

std::vector<int> Minor_shiftAudioProcessor::getChordNotes(const juce::String& chordName, int octave)
{
    static const std::map<juce::String, int> noteMap = {
        {"C", 0}, {"C#", 1}, {"Db", 1}, {"D", 2}, {"D#", 3}, {"Eb", 3}, {"E", 4}, {"F", 5},
        {"F#", 6}, {"Gb", 6}, {"G", 7}, {"G#", 8}, {"Ab", 8}, {"A", 9}, {"A#", 10}, {"Bb", 10}, {"B", 11}
    };

    bool isMinor = chordName.endsWith("m");
    juce::String rootName = isMinor ? chordName.upToLastOccurrenceOf("m", false, true) : chordName;
    int rootNote = 12 * octave + noteMap.at(rootName);

    if (isMinor)
        return { rootNote, rootNote + 3, rootNote + 7 }; // Minor triad
    else
        return { rootNote, rootNote + 4, rootNote + 7 }; // Major triad
}

std::vector<std::vector<int>> Minor_shiftAudioProcessor::generateProgression(const juce::String& key, int rootNote)
{
    static const std::map<juce::String, std::vector<juce::String>> progressionMap = {
        {"Bm",  {"Bm", "A", "G"}},
        {"C#m", {"C#m", "B", "A"}},
        {"F#m", {"F#m", "E", "D"}},
        {"Gm",  {"Gm", "F", "Eb"}},
        {"Dm",  {"Dm", "C", "Bb"}}
    };

    auto it = progressionMap.find(key);
    const auto& chordNames = (it != progressionMap.end()) ? it->second : progressionMap.at("Bm");

    std::vector<std::vector<int>> progression;
    int octave = rootNote / 12;

    for (const auto& chordName : chordNames)
    {
        progression.push_back(getChordNotes(chordName, octave));
    }
    return progression;
}

std::vector<juce::MidiMessageMetadata> Minor_shiftAudioProcessor::generateMelody(const std::vector<std::vector<int>>& chordProgression, juce::uint8 velocity)
{
    std::vector<juce::MidiMessageMetadata> melody;
    int channel = 1;

    double samplesPerBeat = getSampleRate() / (getPlayHead()->getPosition()->getBpm().orFallback(120.0) / 60.0);
    int quarterNoteLength = static_cast<int>(samplesPerBeat);
    int eighthNoteLength = quarterNoteLength / 2;

    int melodyStartTime = 0;

    for (const auto& chord : chordProgression)
    {
        if (chord.size() < 3) continue; // Need at least a triad

        // Simple arpeggio: Root, 3rd, 5th, 3rd (as eighth notes)
        int note1 = chord[0] + 12; // Play melody an octave higher
        int note2 = chord[1] + 12;
        int note3 = chord[2] + 12;

        // Note 1 (Root)
        melody.emplace_back(juce::MidiMessage::noteOn(channel, note1, velocity), melodyStartTime);
        melody.emplace_back(juce::MidiMessage::noteOff(channel, note1), melodyStartTime + eighthNoteLength);
        // Note 2 (3rd)
        melody.emplace_back(juce::MidiMessage::noteOn(channel, note2, velocity), melodyStartTime + eighthNoteLength);
        melody.emplace_back(juce::MidiMessage::noteOff(channel, note2), melodyStartTime + 2 * eighthNoteLength);
        // Note 3 (5th)
        melody.emplace_back(juce::MidiMessage::noteOn(channel, note3, velocity), melodyStartTime + 2 * eighthNoteLength);
        melody.emplace_back(juce::MidiMessage::noteOff(channel, note3), melodyStartTime + 3 * eighthNoteLength);
        // Note 4 (3rd again)
        melody.emplace_back(juce::MidiMessage::noteOn(channel, note2, velocity), melodyStartTime + 3 * eighthNoteLength);
        melody.emplace_back(juce::MidiMessage::noteOff(channel, note2), melodyStartTime + 4 * eighthNoteLength);

        melodyStartTime += quarterNoteLength; // Move to the next beat/chord
    }

    return melody;
}

//==============================================================================
bool Minor_shiftAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Minor_shiftAudioProcessor::createEditor()
{
    return new Minor_shiftAudioProcessorEditor (*this);
}

//==============================================================================
void Minor_shiftAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void Minor_shiftAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout Minor_shiftAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    juce::StringArray keys = { "Bm", "C#m", "F#m", "Gm", "Dm" };
    params.push_back(std::make_unique<juce::AudioParameterChoice>("KEY", "Key", keys, 0));

    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Minor_shiftAudioProcessor();
}