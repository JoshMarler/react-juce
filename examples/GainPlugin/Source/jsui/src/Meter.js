import React, { Component } from 'react';
import {
  EventBridge,
  Image,
  Text,
  View,
} from 'juce-blueprint';


class Meter extends Component {
  constructor(props) {
    super(props);

    this._onMeasure = this._onMeasure.bind(this);
    this._onMeterValues = this._onMeterValues.bind(this);

    this.state = {
      width: 0,
      height: 0,
      lcPeak: 0.0,
      rcPeak: 0.0,
    };
  }

  componentDidMount() {
    EventBridge.addListener('gainPeakValues', this._onMeterValues);
  }

  componentWillUnmount() {
    EventBridge.removeListener('gainPeakValues', this._onMeterValues);
  }

  _onMeterValues(lcPeak, rcPeak) {
    this.setState({
      lcPeak,
      rcPeak,
    });
  }

  _onMeasure(width, height) {
    this.setState({
      width: width,
      height: height,
    });
  }

  _renderVectorGraphics(lcPeak, rcPeak, width, height) {
    // Similar to the audio side of this, this is a pretty rudimentary
    // way of drawing a gain meter; we'd get a much nicer response by using
    // a peak envelope follower with instant attack and a smooth release for
    // each channel, but this is just a demo plugin.
    return `
      <svg
        width="${width}"
        height="${height}"
        viewBox="0 0 ${width} ${height}"
        version="1.1"
        xmlns="http://www.w3.org/2000/svg">
        <rect
          x="${0}" y="${0}"
          width="${width}" height="${height * 0.45}"
          fill="#626262" />
        <rect
          x="${0}" y="${0}"
          width="${width * Math.min(1.0, lcPeak)}" height="${height * 0.45}"
          fill="#66FDCF" />
        <rect
          x="${0}" y="${height * 0.5}"
          width="${width}" height="${height * 0.45}"
          fill="#626262" />
        <rect
          x="${0}" y="${height * 0.5}"
          width="${width * Math.min(1.0, rcPeak)}" height="${height * 0.45}"
          fill="#66FDCF" />
      </svg>
    `;
  }

  render() {
    const {lcPeak, rcPeak, width, height} = this.state;

    return (
      <View {...this.props} onMeasure={this._onMeasure}>
        <Image {...styles.canvas} source={this._renderVectorGraphics(lcPeak, rcPeak, width, height)} />
      </View>
    );
  }
}

const styles = {
  canvas: {
    'flex': 1.0,
    'height': '100%',
    'width': '100%',
    'position': 'absolute',
    'left': 0.0,
    'top': 0.0,
    'interceptClickEvents': false,
  },
};

export default Meter;
