import AnimatedFlexBoxExample from './AnimatedFlexBox';
import Label from './Label';
import Meter from './Meter';
import ParameterToggleButton from './ParameterToggleButton'
import ParameterSlider from './ParameterSlider'

import React, { Component } from 'react';

import {
  Canvas,
  Image,
  Text,
  View,
  Slider,
} from 'react-juce';


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
  constructor(props) {
    super(props);
    this._onMuteToggled = this._onMuteToggled.bind(this);

    this.state = {
      muted: false
    }
  }

  _onMuteToggled(toggled) {
    this.setState({
      muted: toggled
    });
  }

  render() {
    // Uncomment here to watch the animated flex box example in action
    // return (
    //   <View {...styles.container}>
    //     <AnimatedFlexBoxExample />
    //   </View>
    // );

    const muteBackgroundColor = this.state.muted ? 'ff66FDCF' : 'ff17191f';
    const muteTextColor = this.state.muted ? 'ff17191f' : 'ff66FDCF';

    const sliderFillColor  = 'ff66FDCF';
    const sliderTrackColor = 'ff626262';

    const logo_url = "https://raw.githubusercontent.com/nick-thompson/blueprint/master/examples/GainPlugin/jsui/src/logo.png";
    //const logo_png = require('./logo.png');
    //const logo_svg = require('./logo.svg');

    return (
      <View {...styles.container}>
        <View {...styles.content}>
          <Image source={logo_url} {...styles.logo} />
          <ParameterSlider
            paramId="MainGain"
            onDraw={Slider.drawRotary(sliderTrackColor, sliderFillColor)}
            mapDragGestureToValue={Slider.rotaryGestureMap}
            {...styles.knob}
          >
            <Label paramId="MainGain" {...styles.label} />
          </ParameterSlider>
          <Meter {...styles.meter} />	
          <Canvas {...styles.canvas} animate={true} onDraw={animatedDraw} />
          <ParameterToggleButton
            paramId="MainMute"
            onToggled={this._onMuteToggled}
            background-color={muteBackgroundColor}
            {...styles.mute_button}
          >
            <Text color={muteTextColor} {...styles.mute_button_text}>
              MUTE
            </Text>
          </ParameterToggleButton>
        </View>
      </View>
    );
  }
}

const styles = {
  container: {
    width: '100%',
    height: '100%',
    backgroundColor: 'ff17191f',
    justifyContent: 'center',
    alignItems: 'center',
  },
  content: {
    flex: 1.0,
    flexDirection: 'column',
    justifyContent: 'space-around',
    alignItems: 'center',
    padding: 24.0,
    maxWidth: 600,
    aspectRatio: 400.0 / 240.0,
  },
  logo: {
    flex: 0.0,
    width: '80%',
    aspectRatio: 281.6 / 35.0,
    placement: Image.PlacementFlags.centred,
  },
  knob: {
    minWidth: 100.0,
    minHeight: 100.0,
    width: '55%',
    height: '55%',
    marginTop: 15,
    marginBottom: 15,
    justifyContent: 'center',
    alignItems: 'center',
  },
  label: {
    flex: 1.0,
    justifyContent: 'center',
    alignItems: 'center',
    interceptClickEvents: false,
  },
  meter: {
    flex: 0.0,
    width: 100.0,
    height: 16.0,
  },
  canvas: {
    flex: 0.0,
    width: 100.0,
    height: 2,
    marginTop: 10,
  },
  mute_button: {
    justifyContent: 'center',
    alignItems: 'center',
    borderRadius: 5.0,
    borderWidth: 2.0,
    borderColor: 'ff66FDCF',
    marginTop: 10,
    minWidth: 30.0,
    minHeight: 30.0,
    width: '20%',
    height: '17.5%',
  },
  mute_button_text: {
    fontSize: 20.0,
    lineSpacing: 1.6,
    fontStyle: Text.FontStyleFlags.bold,
  }
};

export default App;
