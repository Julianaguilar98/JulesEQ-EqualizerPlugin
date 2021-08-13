/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
struct CustomRotarySlider : juce::Slider
{
    CustomRotarySlider() : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                                        juce::Slider::TextEntryBoxPosition::NoTextBox)
    {

    }

};

//==============================================================================
/**
*/
class JulesEQAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    JulesEQAudioProcessorEditor (JulesEQAudioProcessor&);
    ~JulesEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    JulesEQAudioProcessor& audioProcessor;

    CustomRotarySlider peakFreqSlider, peakGainSlider, peakQualitySlider, lowCutFreqSlider, highCutFreqSlider, lowCutSlopeSlider, highCutSlopeSlider;

    using APVTS = juce::AudioProcessorValueTreeState;
    using Attachement = APVTS::SliderAttachment;

    Attachement peakFreqSliderAttachement, peakGainSliderAttachement, peakQualitySliderAttachement,
                                           lowCutFreqSliderAttachement, highCutFreqSliderAttachement,
                                           lowCutSlopeSliderAttachement, highCutSlopeSliderAttachement;

    std::vector<juce::Component*> getComps();

    MonoChain monoChain;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JulesEQAudioProcessorEditor)
};
