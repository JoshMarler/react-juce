import Label from './Label';
import React, { Component } from 'react';
import Slider from './Slider';
import {
  Image,
  View,
  Text,
} from 'juce-blueprint';


class App extends Component {
  render() {
    return (
      <View {...styles.container}>
        <View {...styles.content}>
          <Image source={require('./logo.svg')} {...styles.logo} />
          <Slider paramId="MainGain" {...styles.knob}>
            <Label paramId="MainGain" {...styles.label} />
          </Slider>
        </View>
      </View>
    );
  }
}

const styles = {
  container: {
    'width': '100%',
    'height': '100%',
    'background-color': 'ff17191f',
    'justify-content': 'center',
    'align-items': 'center',
  },
  content: {
    'flex': 1.0,
    'flex-direction': 'column',
    'justify-content': 'space-around',
    'align-items': 'center',
    'padding': 24.0,
    'max-width': 600,
    'aspect-ratio': 400.0 / 240.0,
  },
  logo: {
    'flex': 0.0,
    'width': '80%',
    'aspect-ratio': 281.6 / 35.0,
    'placement': Image.PlacementFlags.centred,
  },
  knob: {
    'min-width': 100.0,
    'min-height': 100.0,
    'width': '50%',
    'height': '50%',
  },
  label: {
    'flex': 1.0,
    'justify-content': 'center',
    'align-items': 'center',
    'interceptClickEvents': false,
  },
};

export default App;
