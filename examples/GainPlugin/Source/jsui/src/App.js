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
  },
  content: {
    'flex': 1.0,
    'flex-direction': 'column',
    'justify-content': 'space-around',
    'align-items': 'center',
    'padding': 24.0,
  },
  logo: {
    'width': 281.6,
    'height': 35.0,
    'flex': 0.0,
    'flex-shrink': 0.0,
  },
  knob: {
    'width': 100.0,
    'height': 100.0,
  },
  label: {
    'flex': 1.0,
    'justify-content': 'center',
    'align-items': 'center',
    'interceptClickEvents': false,
  },
};

export default App;
