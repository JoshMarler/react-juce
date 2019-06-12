import { Colors, ParamIds } from './Constants';
import ParameterToggleButton from './ParameterToggleButton';
import React, { Component } from 'react';
import { View, Image, Text } from 'juce-blueprint';


class FloatingGlobalButtons extends Component {
  render() {
    return (
      <View {...this.props}>
        <ButtonCell paramId={ParamIds.FILTER_TYPE} source={require('./icons/filter.svg')} />
        <ButtonCell paramId={ParamIds.FREEZE} source={require('./icons/envelope.svg')} />
      </View>
    );
  }
}

function ButtonCell(props) {
  const {source, paramId, ...other} = props;

  return (
    <View {...styles.buttonContainer} {...other}>
      <ParameterToggleButton paramId={paramId} {...styles.button}>
        <Image interceptClickEvents={false} source={source} {...styles.buttonIcon} />
      </ParameterToggleButton>
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
    'border-radius': '50%',
  },
  buttonIcon: {
    'flex': 0.0,
    'width': '72%',
    'aspect-ratio': 1.0,
    'placement': Image.PlacementFlags.centred,
  },
};

export default FloatingGlobalButtons;
