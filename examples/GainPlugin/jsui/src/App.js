import AnimatedFlexBoxExample from './AnimatedFlexBox';
import Label from './Label';
import Meter from './Meter';
import React, { Component } from 'react';
import Slider from './Slider';
import ParameterToggleButton from './ParameterToggleButton'
import {
  Canvas,
  Image,
  Text,
  View,
} from 'react-juce';


function animatedDraw(ctx, sizeState) {
  let now = (Date.now() / 10);
  let width = now % 100;
  let red = Math.sqrt(width / 100) * 255;
  let hex = Math.floor(red).toString(16);

  // TODO: Should update the ctx proxy to convert from javascript hex strings, aka
  // #ffffaa to juce's Colour::fromString() API which is ffffffaa.
  ctx.fillStyle = `ff${hex}ffaa`;
  ctx.fillRect(0, 0, width, sizeState.height);
}


 function colorForTouch(touch) {
  const firstBit = (touch.identifier & ( 1 << 0 )) >> 0;
  const secondBit = (touch.identifier & ( 1 << 1 )) >> 1;
  const thirdBit = (touch.identifier & ( 1 << 2 )) >> 2;
  let r = firstBit * 255;
  let g = secondBit * 255;
  let b = thirdBit * 255;
  r = r.toString(16).padStart(2, '0'); // make it a hex digit
  g = g.toString(16).padStart(2, '0'); // make it a hex digit
  b = b.toString(16).padStart(2, '0'); // make it a hex digit
  const color = "ff" + r + g + b;
  return color;
}

 function copyTouch({ identifier, x ,y }) {
  return { identifier, x, y };
}

class App extends Component {
  constructor(props) {
    super(props);
    this._onMuteToggled = this._onMuteToggled.bind(this);
    this._onTouchStart = this._onTouchStart.bind(this);
    this._onTouchMove = this._onTouchMove.bind(this);
    this._onTouchEnd = this._onTouchEnd.bind(this);

    this.state = {
      muted: false
    }

    this.ongoingTouches = [];

  }

  drawingCanvasDraw(ctx, sizeState) {
    if (sizeState.firstStroke) {
      ctx.fillStyle = "ffffffff";
      ctx.fillRect(0, 0, sizeState.width, sizeState.height);
    }

    for (let i = 0; i < this.ongoingTouches.length; i++) {
      const drawingTouch = this.ongoingTouches[i];
      const drawType = drawingTouch["type"];
      const touch = drawingTouch["touch"];
      const color = colorForTouch(touch);
      switch (drawType) {
        case "start":
          ctx.beginPath();
          ctx.moveTo(touch.x, touch.y);
          ctx.arc(touch.x - 3, touch.y - 3, 6, 0, 2 * Math.PI, false);  // a circle at the start
          ctx.fillStyle = color;
          ctx.fill();
          break
        case "move": {
          const previousPosition = drawingTouch["before"];
          ctx.beginPath();
          ctx.moveTo(previousPosition.x, previousPosition.y);
          ctx.lineTo(touch.x, touch.y);
          ctx.lineWidth = 4;
          ctx.strokeStyle = color;
          ctx.stroke();
          drawingTouch["before"] = {"x": touch.x, "y": touch.y};
          break
        }
        case "end": {
          let previousPosition = drawingTouch["before"];
          ctx.lineWidth = 4;
          ctx.fillStyle = color;
          ctx.strokeStyle = color;
          ctx.beginPath();
          ctx.moveTo(previousPosition.x, previousPosition.y);
          ctx.lineTo(touch.x, touch.y);
          ctx.stroke();
          ctx.fillRect(touch.x - 6, touch.y - 6, 12, 12);
          this.ongoingTouches.splice(i, 1);
          break
        }
        default:
          break
      }
    }
  }


  ongoingTouchIndexById(idToFind) {
    for (let i = 0; i < this.ongoingTouches.length; i++) {
      const id = this.ongoingTouches[i]["touch"].identifier;

      if (id === idToFind) {
        return i;
      }
    }
    return -1;    // not found
  }

  _onTouchStart(evt) {
    evt.preventDefault();
    const touches = evt.changedTouches;

    for (let i = 0; i < touches.length; i++) {
      this.ongoingTouches.push({"type": "start", "touch": copyTouch(touches[i]), "before": {"x": touches[i].x, "y": touches[i].y}});
    }
  }

  _onTouchMove(evt) {
    evt.preventDefault();
    const touches = evt.changedTouches;

    for (let i = 0; i < touches.length; i++) {
      const idx = this.ongoingTouchIndexById(touches[i].identifier);
      if (idx >= 0) {
        this.ongoingTouches[idx]["type"] = "move";
        this.ongoingTouches[idx]["touch"] = copyTouch(touches[i]);
      }
    }
  }


  _onTouchEnd(evt) {
    evt.preventDefault();
    const touches = evt.changedTouches;

    for (let i = 0; i < touches.length; i++) {
      const idx = this.ongoingTouchIndexById(touches[i].identifier);
      if (idx >= 0) {
        this.ongoingTouches[idx]["type"] = "end";
        this.ongoingTouches[idx]["touch"] = copyTouch(touches[i]);
      }
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

    return (
      <View {...styles.container}>
        <View {...styles.content}>
          <Image source={require('./logo.png')} {...styles.logo} />
          <Slider paramId="MainGain" {...styles.knob}>
            <Label paramId="MainGain" {...styles.label} />
          </Slider>
          <Meter {...styles.meter} />
          <Canvas {...styles.canvas} autoclear={true} animate={true} onDraw={animatedDraw} />
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
        <Canvas {...styles.drawingCanvas} autoclear={false} animate={true} onDraw={this.drawingCanvasDraw} onTouchStart={this._onTouchStart}
        onTouchMove={this._onTouchMove}
        onTouchEnd={this._onTouchEnd}
        ongoingTouches={this.ongoingTouches}/>
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
    'margin-top': 15,
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
    'height': 2,
    'margin-top': 10,
  },
  mute_button: {
    'justify-content': 'center',
    'align-items': 'center',
    'border-radius': 5.0,
    'border-width': 2.0,
    'border-color': 'ff66FDCF',
    'margin-top': 10,
    'min-width': 30.0,
    'min-height': 30.0,
    'width': '20%',
    'height': '17.5%',
  },
  mute_button_text: {
    'font-size': 20.0,
    'line-spacing': 1.6,
    'font-style': Text.FontStyleFlags.bold,
  },
  drawingCanvas: {
    'flex': 0.0,
    'width': '50%',
    'height': '50%',
    'margin-top': 10,
  },
};

export default App;
