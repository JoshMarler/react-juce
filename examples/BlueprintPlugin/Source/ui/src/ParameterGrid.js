import { Colors, ParamIds } from './Constants';
import FloatingGlobalButtons from './FloatingGlobalButtons';
import FloatingGlobalKnobs from './FloatingGlobalKnobs';
import ParameterGridSlider from './ParameterGridSlider';
import ParameterLabel from './ParameterLabel';
import GrainParameterLabel from './GrainParameterLabel';
import React, { Component } from 'react';
import {
  Image,
  Text,
  View,
} from './Blueprint';

import {
  drawGrainFrequencyPattern,
  drawGrainSprayPattern,
  drawGrainSpreadPattern,
  drawGrainPitchPattern,
  drawGrainFeedbackPattern
} from './Drawing';


class ParameterGrid extends Component {
  render() {
    return (
      <View {...styles.container} {...this.props}>
        <View {...styles.contentHeader}>
          <ParameterLabel paramId={ParamIds.DELAY} {...styles.shiftLeft} {...styles.labelContainer} />
          <ParameterLabel paramId={ParamIds.FILTER_CUTOFF} {...styles.shiftLeft} {...styles.labelContainer} />
          <ParameterLabel paramId={ParamIds.WARP} {...styles.shiftLeft} {...styles.labelContainer} />
          <ParameterLabel paramId={ParamIds.ENVELOPE_THRESHOLD} {...styles.shiftLeft} {...styles.labelContainer} />
          <ParameterLabel paramId={ParamIds.MIX} {...styles.shiftLeft} {...styles.labelContainer} />
        </View>
        <View {...styles.grid}>
          <View {...styles.row}>
            <Gutter direction={-1.0}>GRAIN 1</Gutter>
            <ParameterGridSlider ntr paramId={ParamIds.GRAIN_FREQUENCY} {...styles.cell} drawFunction={drawGrainFrequencyPattern} />
            <ParameterGridSlider ntl ntr paramId={ParamIds.SPRAY} {...styles.cell} drawFunction={drawGrainSprayPattern} />
            <ParameterGridSlider ntl ntr paramId={ParamIds.SPREAD} {...styles.cell} drawFunction={drawGrainSpreadPattern} />
            <ParameterGridSlider ntl ntr paramId={ParamIds.PITCH} {...styles.cell} drawFunction={drawGrainPitchPattern} />
            <ParameterGridSlider ntl ntr paramId={ParamIds.REVERSE} {...styles.cell} drawFunction={drawGrainFrequencyPattern} />
            <ParameterGridSlider ntl paramId={ParamIds.FEEDBACK_G1G2} {...styles.cell} drawFunction={drawGrainFeedbackPattern} />
            <Gutter direction={1.0}>GRAIN 1</Gutter>
          </View>
          <View {...styles.row}>
            <Gutter direction={-1.0}>GRAIN 2</Gutter>
            <ParameterGridSlider paramId={ParamIds.GRAIN_FREQUENCY_G2} {...styles.cell} drawFunction={drawGrainFrequencyPattern} />
            <ParameterGridSlider nbr paramId={ParamIds.SPRAY_G2} {...styles.cell} drawFunction={drawGrainSprayPattern} />
            <ParameterGridSlider nbl paramId={ParamIds.SPREAD_G2} {...styles.cell} drawFunction={drawGrainSpreadPattern} />
            <ParameterGridSlider nbr paramId={ParamIds.PITCH_G2} {...styles.cell} drawFunction={drawGrainPitchPattern} />
            <ParameterGridSlider nbl paramId={ParamIds.REVERSE_G2} {...styles.cell} drawFunction={drawGrainFrequencyPattern} />
            <ParameterGridSlider paramId={ParamIds.FEEDBACK_G2G1} {...styles.cell} drawFunction={drawGrainFeedbackPattern} />
            <Gutter direction={1.0}>GRAIN 2</Gutter>
          </View>
        </View>
        <View {...styles.contentFooter}>
          <GrainParameterLabel g1ParamId={ParamIds.GRAIN_FREQUENCY} g2ParamId={ParamIds.GRAIN_FREQUENCY_G2} {...styles.labelContainer}>FREQUENCY</GrainParameterLabel>
          <GrainParameterLabel g1ParamId={ParamIds.SPRAY} g2ParamId={ParamIds.SPRAY_G2} {...styles.labelContainer}>SPRAY</GrainParameterLabel>
          <GrainParameterLabel g1ParamId={ParamIds.SPREAD} g2ParamId={ParamIds.SPREAD_G2} {...styles.labelContainer}>SPREAD</GrainParameterLabel>
          <GrainParameterLabel g1ParamId={ParamIds.PITCH} g2ParamId={ParamIds.PITCH_G2} {...styles.labelContainer}>PITCH</GrainParameterLabel>
          <GrainParameterLabel g1ParamId={ParamIds.REVERSE} g2ParamId={ParamIds.REVERSE_G2} {...styles.labelContainer}>REVERSE</GrainParameterLabel>
          <GrainParameterLabel g1ParamId={ParamIds.FEEDBACK_G1G2} g2ParamId={ParamIds.FEEDBACK_G2G1} {...styles.labelContainer}>FEEDBACK</GrainParameterLabel>
        </View>
        <FloatingGlobalKnobs {...styles.contentHeader} {...styles.floatingHeader} />
        <FloatingGlobalButtons {...styles.contentFooter} {...styles.floatingFooter} />
      </View>
    );
  }
}

function Gutter(props) {
  return (
    <View {...styles.gutter}>
      <Text {...styles.gutterText} {...{'transform-rotate': Math.PI * 0.5 * props.direction}}>
        {props.children}
      </Text>
    </View>
  );
}

const styles = {
  container: {
    'flex-direction': 'column',
  },
  contentHeader: {
    'height': `${100.0 / 9.0}%`, // Equivalent to the Chrome's header row height
    'flex': 0.0,
    'justify-content': 'center',
    'padding-left': 32.0,
    'padding-right': 32.0,
  },
  floatingHeader: {
    'position': 'absolute',
    'top': `${100.0 / 9.0 / 2.0}%`, // Half the Chrome's header row height
    'width': '100%',
  },
  contentFooter: {
    'height': `${100.0 / 9.0}%`, // Equivalent to the Chrome's header row height
    'flex': 0.0,
    'justify-content': 'center',
    'padding-left': 32.0,
    'padding-right': 32.0,
  },
  floatingFooter: {
    'position': 'absolute',
    'bottom': `${100.0 / 9.0 / 2.0}%`, // Half the Chrome's header row height
    'width': '100%',
  },
  labelContainer: {
    'flex': 0.0,
    'width': `${100.0 / 6.0}%`,
    'justify-content': 'center',
  },
  shiftLeft: {
    'position': 'relative',
    'left': '-7%',
  },
  grid: {
    'flex': 1.0,
    'flex-direction': 'column',
  },
  gutter: {
    'flex': 0.0,
    'width': 32.0,
    'justify-content': 'center',
    'align-items': 'center',
  },
  gutterText: {
    // The internal text measureFunc doesn't care about the rotation and will
    // clip the width of the text to the width of the parent if flex-shrink
    // is allowed. This 0 value guarantees that our text element width will be
    // the size of the text, then the rotation will ensure we avoid clipping.
    'flex-shrink': 0,
    'color': Colors.STROKE,
    'word-wrap': Text.WordWrap.none,
    'font-size': 16.0,
    'font-family': 'open-sans',
    'line-spacing': 1.6,
  },
  row: {
    'flex': 1.0,
    'flex-direction': 'row',
  },
  cell: {
    'flex': 1.0,
    'margin': 2.0,
  },
};

export default ParameterGrid;
