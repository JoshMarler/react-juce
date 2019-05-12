import { Colors } from './Constants';
import React, { Component } from 'react';
import { View, Image, Text } from './Blueprint';


class FloatingGlobalButtons extends Component {
  render() {
    return (
      <View {...this.props}>
        <ButtonCell source={require('./icons/filter.svg')} />
        <ButtonCell source={require('./icons/envelope.svg')} />
      </View>
    );
  }
}

function ButtonCell(props) {
  const {source, ...other} = props;

  return (
    <View {...styles.buttonContainer} {...other}>
      <View {...styles.button}>
        <Image source={source} {...styles.buttonIcon} />
      </View>
    </View>
  );
}

const styles = {
  buttonContainer: {
    'flex': 0.0,
    'width': `${2.0 * 100.0 / 6.0}%`,
    'justify-content': 'center',
  },
  button: {
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
  buttonIcon: {
    'flex': 0.0,
    'width': '72%',
    'aspect-ratio': 1.0,
    'placement': Image.PlacementFlags.centred,
  },
};

export default FloatingGlobalButtons;
