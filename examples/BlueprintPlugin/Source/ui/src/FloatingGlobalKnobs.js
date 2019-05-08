import { Colors } from './Constants';
import React, { Component } from 'react';
import { View, Image, Text } from './Blueprint';


class FloatingGlobalKnobs extends Component {
  render() {
    return (
      <View {...this.props}>
        <KnobCell source={require('./icons/delay.svg')} />
        <KnobCell source={require('./icons/filter.svg')} />
        <KnobCell source={require('./icons/wave.svg')} />
        <KnobCell source={require('./icons/envelope.svg')} />
        <KnobCell source={require('./icons/delay.svg')} />
      </View>
    );
  }
}

function KnobCell(props) {
  const {source, ...other} = props;

  return (
    <View {...styles.knobContainer} {...other}>
      <View {...styles.knob}>
        <Image source={source} {...styles.knobIcon} />
      </View>
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
  },
};

export default FloatingGlobalKnobs;
