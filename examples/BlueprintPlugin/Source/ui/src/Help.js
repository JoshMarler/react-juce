import { Colors } from './Constants';
import React, { Component } from 'react';
import { View, Image, Text, ScrollView } from 'juce-blueprint';


function SectionHeader(props) {
  return (
    <View {...styles.sectionHeader} {...props}>
      <Text {...styles.sectionHeaderText}>
        {props.children}
      </Text>
    </View>
  );
}

function SectionTitle(props) {
  return (
    <View {...styles.sectionTitle} {...props}>
      <Text {...styles.sectionTitleText}>
        {props.children}
      </Text>
    </View>
  );
}

class Help extends Component {
  render() {
    return (
      <View {...styles.container} {...this.props}>
        <ScrollView {...styles.scrollView}>
          <ScrollView.ContentView {...styles.scrollViewContent}>
            <Text {...styles.sectionBodyText}>{"v" + global.__VERSION__}</Text>
            <SectionTitle>[SIGNAL FLOW]</SectionTitle>
            <Text {...styles.sectionBodyText}>
              Remnant consists of two independent grain engines that read from two distinct delay lines. The input signal feeds
              into each delay line after going through independent filters, then the grain engines produce grains from the signal
              in its respective delay line. The output of each grain engine feeds back into itself, and feeds back into the other
              grain engine an amount decided by the CrossTalk parameter.
            </Text>
            <SectionTitle {...{'margin-top': 16.0}}>[GLOBAL PARAMETERS]</SectionTitle>
            <SectionHeader>DELAY</SectionHeader>
            <Text {...styles.sectionBodyText}>
              Sets the delay time from input to output. The larger the delay time, the larger individual grains can be,
              and the more opportunity for randomness within each grain engine.
            </Text>
            <SectionHeader>CUTOFF</SectionHeader>
            <Text {...styles.sectionBodyText}>
              Both the direct input and the feedback signal run through a filter before entering the grain stage. This parameter
              sets the cutoff frequency of that filter. The FILTER TYPE button beneath it toggles between Lowpass and Highpass mode.
            </Text>
            <SectionHeader>WARP</SectionHeader>
            <Text {...styles.sectionBodyText}>
              Controls a classic pitch warp applied to the grain engine output. Can be used to dial in subtle detune effects or
              the nostalgic pitch wobble of a classic tape machine.
            </Text>
            <SectionHeader>THRESHOLD</SectionHeader>
            <Text {...styles.sectionBodyText}>
              Only takes effect when FREEZE is enabled. While the delay buffer is frozen, the value of the input signal can
              be used to temporarily disable the freeze, allowing you to capture a new frozen state. This temporary unlock
              happens as long as the input signal is above the THRESHOLD value set here.
            </Text>
            <SectionHeader>MIX</SectionHeader>
            <Text {...styles.sectionBodyText}>
              Your standard wet/dry control. A value of 0% is just the dry input, a value of 100% is just the wet output.
            </Text>
            <SectionTitle {...{'margin-top': 16.0}}>[GRAIN PARAMETERS]</SectionTitle>
            <SectionHeader>FREQUENCY</SectionHeader>
            <Text {...styles.sectionBodyText}>
              Sets how frequently grains are emitted, and proportionally sets the length of an individual grain. Lower frequency
              values mean longer grains, but the grain length will automatically be limited to the size of the DELAY parameter.
            </Text>
            <SectionHeader>SPRAY</SectionHeader>
            <Text {...styles.sectionBodyText}>
              Introduces randomness when deciding where from within the delay buffer to pick the next grain.
            </Text>
            <SectionHeader>SPREAD</SectionHeader>
            <Text {...styles.sectionBodyText}>
              Randomly pans each grain to the left or the right. The SPREAD value sets the maximum pan amount to either side.
            </Text>
            <SectionHeader>PITCH</SectionHeader>
            <Text {...styles.sectionBodyText}>
              Sets the playback pitch for each grain.
            </Text>
            <SectionHeader>REVERSE</SectionHeader>
            <Text {...styles.sectionBodyText}>
              Sets the probability of playing a grain in reverse. At 100%, all grains will be played in reverse.
            </Text>
            <SectionHeader>FEEDBACK</SectionHeader>
            <Text {...styles.sectionBodyText}>
              Sets the feedback amount from one grain engine back into itself.
            </Text>
          </ScrollView.ContentView>
        </ScrollView>
      </View>
    );
  }
}

const styles = {
  container: {
    'padding-left': 16.0,
    'padding-right': 16.0,
  },
  scrollView: {
    'flex-direction': 'column',
    'flex': 1.0,
    'height': '100%',
  },
  scrollViewContent: {
    'flex-direction': 'column',
    // This is important. We want the scroll view content to stretch vertically
    // to contain its children but we need to enforce that it does not get shrunk,
    // otherwise Yoga will attempt to squeeze it into the size of its container.
    'flex': 1.0,
    'flex-shrink': 0.0,
    'padding-top': 16.0,
    'padding-bottom': 16.0,
    'padding-right': 16.0,
  },
  sectionHeader: {
    'margin-top': 8.0,
    'margin-bottom': 4.0,
    'flex-shrink': 0.0,
  },
  sectionHeaderText: {
    'color': Colors.STROKE,
    'font-size': 20.0,
    'font-family': 'open-sans',
    'font-style': Text.FontStyleFlags.bold,
  },
  sectionTitle: {
    'margin-top': 4.0,
    'margin-bottom': 4.0,
    'flex-shrink': 0.0,
  },
  sectionTitleText: {
    'color': Colors.SOFT_HIGHLIGHT,
    'font-size': 22.0,
    'font-family': 'open-sans',
  },
  sectionBodyText: {
    'color': Colors.STROKE,
    'font-size': 18.0,
    'font-family': 'open-sans',
    'line-spacing': 1.6,
  },
};

export default Help;
