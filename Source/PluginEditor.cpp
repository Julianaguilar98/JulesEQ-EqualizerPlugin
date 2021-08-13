/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
JulesEQAudioProcessorEditor::JulesEQAudioProcessorEditor (JulesEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),

    peakFreqSliderAttachement(audioProcessor.apvts, "Peak Freq", peakFreqSlider),
    peakGainSliderAttachement(audioProcessor.apvts, "Peak Gain", peakGainSlider),
    peakQualitySliderAttachement(audioProcessor.apvts, "Peak Quality", peakQualitySlider),
    lowCutFreqSliderAttachement(audioProcessor.apvts, "LowCut Freq", lowCutFreqSlider),
    highCutFreqSliderAttachement(audioProcessor.apvts, "HighCut Freq", highCutFreqSlider),
    lowCutSlopeSliderAttachement(audioProcessor.apvts, "LowCut Slope", lowCutSlopeSlider),
    highCutSlopeSliderAttachement(audioProcessor.apvts, "HighCut Slope", highCutSlopeSlider)

{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    for (auto* comp : getComps() )
    {
        addAndMakeVisible(comp);
    }

    setSize (600, 400);
}

JulesEQAudioProcessorEditor::~JulesEQAudioProcessorEditor()
{
}

//==============================================================================
void JulesEQAudioProcessorEditor::paint(juce::Graphics& g)
{
    using namespace juce;
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);

    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);

    auto w = responseArea.getWidth();

    auto& lowcut = monoChain.get<ChainPositions::LowCut>();
    auto& peak = monoChain.get<ChainPositions::Peak>();
    auto& highcut = monoChain.get<ChainPositions::HighCut>();

    auto sampleRate = audioProcessor.getSampleRate();

    std::vector<double> mags;

    mags.resize(w);

    for (int i = 0; i < w; ++i)
    {
        double mag = 1.f;
        auto freq = mapToLog10(double(i) / double(w), 20.0, 20000.0);

        if (!monoChain.isBypassed<ChainPositions::Peak>())
            mag *= peak.coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!lowcut.isBypassed<0>())
            mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowcut.isBypassed<1>())
            mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowcut.isBypassed<2>())
            mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!lowcut.isBypassed<3>())
            mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        if (!highcut.isBypassed<0>())
            mag *= lowcut.get<0>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highcut.isBypassed<1>())
            mag *= lowcut.get<1>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highcut.isBypassed<2>())
            mag *= lowcut.get<2>().coefficients->getMagnitudeForFrequency(freq, sampleRate);
        if (!highcut.isBypassed<3>())
            mag *= lowcut.get<3>().coefficients->getMagnitudeForFrequency(freq, sampleRate);

        mags[i] = Decibels::gainToDecibels(mag);
    }
    Path responseCurve;

    const double outputMin = responseArea.getBottom();
    const double outputMax = responseArea.getY();
    auto map = [outputMin, outputMax](double input)
    {
        return jmap(input, -24.0, 24.0, outputMin, outputMax);
    };

    responseCurve.startNewSubPath(responseArea.getX(), map(mags.front()));

    for (size_t i = 1; i < mags.size(); ++i)
    {
        responseCurve.lineTo(responseArea.getX() + i, map(mags[i]));
    }

    g.setColour(Colours::orange);
    g.drawRoundedRectangle(responseArea.toFloat(), 4.f, 1.f);

    g.setColour(Colours::white);
    g.strokePath(responseCurve, PathStrokeType(2.f));

}

void JulesEQAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight() * 0.33);

    auto lowCutArea = bounds.removeFromLeft(bounds.getWidth() * 0.33);
    auto highCutArea = bounds.removeFromRight(bounds.getWidth() * 0.5);

    lowCutFreqSlider.setBounds(lowCutArea.removeFromTop(lowCutArea.getHeight() * 0.5));
    lowCutSlopeSlider.setBounds(lowCutArea);

    highCutFreqSlider.setBounds(highCutArea.removeFromTop(highCutArea.getHeight() * 0.5));
    highCutSlopeSlider.setBounds(highCutArea);

    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
    peakQualitySlider.setBounds(bounds);

}

std::vector<juce::Component*> JulesEQAudioProcessorEditor::getComps()
{
    return
    {
        &peakFreqSlider,
        &peakGainSlider,
        &peakQualitySlider,
        &lowCutFreqSlider,
        &highCutFreqSlider,
        &lowCutSlopeSlider,
        &highCutSlopeSlider
    };

}