import AnimatedFlexBoxExample from './AnimatedFlexBox';
import Label from './Label';
import Meter from './Meter';
import React, { Component } from 'react';
import Slider from './Slider';
import {
  Canvas,
  Image,
  View,
  Text,
} from 'juce-blueprint';


function animatedDraw(ctx) {
  let now = (Date.now() / 10);
  let width = now % 100;
  let red = Math.sqrt(width / 100) * 255;
  let hex = Math.floor(red).toString(16);

  // TODO: Should update the ctx proxy to convert from javascript hex strings, aka
  // #ffffaa to juce's Colour::fromString() API which is ffffffaa.
  ctx.fillStyle = `ff${hex}ffaa`;
  ctx.fillRect(0, 0, width, 2);
}

class App extends Component {
  render() {
    // Uncomment here to watch the animated flex box example in action
    // return (
    //   <View {...styles.container}>
    //     <AnimatedFlexBoxExample />
    //   </View>
    // );

    return (
      <View {...styles.container}>
        <View {...styles.content}>
          <Image source={require('./logo.svg')} {...styles.logo} />
          <Slider paramId="MainGain" {...styles.knob}>
            <Label paramId="MainGain" {...styles.label} />
          </Slider>
          <Meter {...styles.meter} />
          <Canvas {...styles.canvas} animate={true} onDraw={animatedDraw} />
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
  meter: {
    'flex': 0.0,
    'width': 100.0,
    'height': 16.0,
  },
  canvas: {
    'flex': 0.0,
    'width': 100.0,
    'height': 2
  },
};

export default App;
