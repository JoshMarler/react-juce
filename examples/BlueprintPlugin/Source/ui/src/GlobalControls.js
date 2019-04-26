import React, { Component } from 'react';

import productLogo from './icons/product-logo.svg';
import cog from './icons/cog.svg';
import info from './icons/info.svg';


// TODO: Put this inside blueprint...
function View(props) {
  return React.createElement('View', props, props.children);
}

function Text(props) {
  return React.createElement('Text', props, props.children);
}

function Image(props) {
  return React.createElement('Image', props, props.children);
}

class GlobalControls extends Component {
  render() {
    return (
      <View {...styles.container} {...this.props}>
        <View {...styles.inner}>
          <View {...styles.knobContainer}>
            <View {...styles.knob}>
              <Image source={require('./icons/delay.svg')} {...styles.knobIcon} />
            </View>
          </View>
          <View {...styles.knobContainer}>
            <View {...styles.knob}>
              <Image source={require('./icons/filter.svg')} {...styles.knobIcon} />
            </View>
          </View>
          <View {...styles.knobContainer}>
            <View {...styles.knob}>
              <Image source={require('./icons/wave.svg')} {...styles.knobIcon} />
            </View>
          </View>
          <View {...styles.knobContainer}>
            <View {...styles.knob}>
              <Image source={require('./icons/envelope.svg')} {...styles.knobIcon} />
            </View>
          </View>
          <View {...styles.knobContainer}>
            <View {...styles.knob}>
              <Image source={require('./icons/delay.svg')} {...styles.knobIcon} />
            </View>
          </View>
        </View>
      </View>
    );
  }
}

const Colors = {
  BACKGROUND: 'ff17191f',
  STROKE: 'ff626262',
  DBG_RED: 'ff884848',
  DBG_GREEN: 'ff488848',
};

const styles = {
  container: {
    'flex-direction': 'row',
    'justify-content': 'center',
    'position': 'relative',
    'top': '3.6%',
  },
  inner: {
    'flex': 1.0,
    'justify-content': 'center',
  },
  knobContainer: {
    'flex': 0.0,
    'width': '16.86%',
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

export default GlobalControls;
