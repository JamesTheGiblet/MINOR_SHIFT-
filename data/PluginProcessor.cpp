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
#endif
{
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
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
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
    buffer.clear(); // This is a MIDI effect, so we clear the audio buffer.

    juce::MidiBuffer processedMidi;
    int time;
    juce::MidiMessage msg;

    // 1. Iterate over incoming MIDI messages
    for (const auto metadata : midiMessages)
    {
        msg = metadata.getMessage();
        time = metadata.samplePosition;

        if (msg.isNoteOn())
        {
            // 2. An input note is played, so trigger the progression
            int rootNote = msg.getNoteNumber();
            auto keyParam = static_cast<juce::AudioParameterChoice*>(apvts.getParameter("KEY"));
            juce::String selectedKey = keyParam->getCurrentChoiceName();

            // 3. Generate the chord progression based on the selected key
            auto progression = generateProgression(selectedKey, rootNote);

            // 4. Create new MIDI messages for the chords
            double samplesPerBeat = getSampleRate() / (getPlayHead()->getPosition()->getBpm().orFallback(120.0) / 60.0);
            int chordLengthInSamples = static_cast<int>(samplesPerBeat); // Each chord lasts one beat

            int chordStartTime = time;
            for (const auto& chord : progression)
            {
                for (int note : chord)
                {
                    // Add Note On message
                    processedMidi.addEvent(juce::MidiMessage::noteOn(msg.getChannel(), note, (juce::uint8)msg.getVelocity()), chordStartTime);
                }
                for (int note : chord)
                {
                    // Add Note Off message for the same chord one beat later
                    processedMidi.addEvent(juce::MidiMessage::noteOff(msg.getChannel(), note), chordStartTime + chordLengthInSamples);
                }
                chordStartTime += chordLengthInSamples; // Move to the next beat
            }
        }
        else
        {
            // Pass through other MIDI messages (Note Off, CC, etc.)
            processedMidi.addEvent(msg, time);
        }
    }

    midiMessages.swapWith(processedMidi);
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