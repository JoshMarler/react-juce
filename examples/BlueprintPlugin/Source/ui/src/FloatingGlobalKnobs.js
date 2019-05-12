import { Colors } from './Constants';
import React, { Component } from 'react';
import RotarySlider from './RotarySlider';
import { View, Image, Text } from './Blueprint';


class FloatingGlobalKnobs extends Component {
  render() {
    return (
      <View {...this.props}>
        <KnobCell paramId="delay" source={require('./icons/delay.svg')} />
        <KnobCell paramId="filter" source={require('./icons/filter.svg')} />
        <KnobCell paramId="warp" source={require('./icons/wave.svg')} />
        <KnobCell paramId="env" source={require('./icons/envelope.svg')} />
        <KnobCell paramId="mix" source={require('./icons/delay.svg')} />
      </View>
    );
  }
}

function KnobCell(props) {
  const {source, paramId, ...other} = props;

  return (
    <View {...styles.knobContainer} {...other}>
      <RotarySlider paramId={paramId} {...styles.knob}>
        <Image source={source} interceptClickEvents={false} {...styles.knobIcon} />
      </RotarySlider>
    </View>
  );
}

const styles = {
  knobContainer: {
    'flex': 0.0,
    'width': `${100.0 / 6.0}%`,
    'justify-content': 'center',
  },
  knob: {
    'flex': 0.0,
    'height': '100%',
    'justify-content': 'center',
    'align-items': 'center',
    'aspect-ratio': 1.0,
    'border-width': 2.0,
    'border-color': Colors.STROKE,
    'border-top-left-radius': '50%',
    'border-top-right-radius': '50%',
    'border-bottom-left-radius': '50%',
    'border-bottom-right-radius': '50%',
  },
  knobIcon: {
    'flex': 0.0,
    'width': '72%',
    'aspect-ratio': 1.0,
    'placement': Image.PlacementFlags.centred,
  },
};

export default FloatingGlobalKnobs;
