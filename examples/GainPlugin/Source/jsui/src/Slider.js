import ParameterValueStore from './ParameterValueStore';
import React, { Component } from 'react';
import {
  Image,
  Text,
  View,
} from 'juce-blueprint';


class Slider extends Component {
  constructor(props) {
    super(props);

    this._onMeasure = this._onMeasure.bind(this);
    this._onMouseDown = this._onMouseDown.bind(this);
    this._onMouseUp = this._onMouseUp.bind(this);
    this._onMouseDrag = this._onMouseDrag.bind(this);
    this._renderVectorGraphics = this._renderVectorGraphics.bind(this);
    this._onParameterValueChange = this._onParameterValueChange.bind(this);

    // During a drag, we hold the value at which the drag started here to
    // ensure smooth behavior while the component state is being updated.
    this._valueAtDragStart = 0.0;

    const paramState = ParameterValueStore.getParameterState(this.props.paramId);
    const initialValue = typeof paramState.currentValue === 'number' ?
      paramState.currentValue : 0.0;

    this.state = {
      width: 0,
      height: 0,
      value: initialValue,
    };
  }

  componentDidMount() {
    ParameterValueStore.addListener(
      ParameterValueStore.CHANGE_EVENT,
      this._onParameterValueChange
    );
  }

  componentWillUnmount() {
    ParameterValueStore.removeListener(
      ParameterValueStore.CHANGE_EVENT,
      this._onParameterValueChange
    );
  }

  _onMeasure(width, height) {
    this.setState({
      width: width,
      height: height,
    });
  }

  _onMouseDown(mouseX, mouseY) {
    this._valueAtDragStart = this.state.value;
    global.beginParameterChangeGesture(this.props.paramId);
  }

  _onMouseUp(mouseX, mouseY) {
    global.endParameterChangeGesture(this.props.paramId);
  }

  _onMouseDrag(mouseX, mouseY, mouseDownX, mouseDownY) {
    // Component vectors
    let dx = mouseX - mouseDownX;
    let dy = mouseDownY - mouseY;

    // Delta
    let dm = dx + dy;
    let sensitivity = (1.0 / 200.0);
    let value = Math.max(0.0, Math.min(1.0, this._valueAtDragStart + dm * sensitivity));

    if (typeof this.props.paramId === 'string' && this.props.paramId.length > 0) {
      global.setParameterValueNotifyingHost(this.props.paramId, value);
    }
  }

  _onParameterValueChange(paramId) {
    const shouldUpdate = typeof this.props.paramId === 'string' &&
      this.props.paramId.length > 0 &&
      this.props.paramId === paramId;

    if (shouldUpdate) {
      const state = ParameterValueStore.getParameterState(paramId);

      this.setState({
        defaultValue: state.defaultValue,
        value: state.currentValue,
      });
    }
  }

  _renderVectorGraphics(value, width, height) {
    const cx = width * 0.5;
    const cy = height * 0.5;
    const strokeWidth = 2.0;

    // Note that we nudge the radius by half the stroke width; this is because
    // the stroke will extend outwards in both directions from the given coordinates,
    // which gets clipped if we try to draw the circle perfectly on the edge of the
    // image. We nudge it in so that no part of the path gets clipped.
    const radius = (Math.min(width, height) * 0.5) - (strokeWidth / 2);

    // Animate the arc by stroke-dasharray, where the length of the dash is
    // related to the value property and the length of the space takes up the
    // rest of the circle.
    const arcCircumference = 1.5 * Math.PI * radius;
    const dashArray = [value * arcCircumference, 2.0 * Math.PI * radius];

    return `
      <svg
        width="${width}"
        height="${height}"
        viewBox="0 0 ${width} ${height}"
        version="1.1"
        xmlns="http://www.w3.org/2000/svg">
        <circle
          cx="${cx}"
          cy="${cy}"
          r="${radius}"
          stroke="#626262"
          stroke-width="${strokeWidth}"
          stroke-dasharray="${[arcCircumference, arcCircumference].join(',')}"
          fill="none" />
        <circle
          cx="${cx}"
          cy="${cy}"
          r="${radius}"
          stroke="#66FDCF"
          stroke-width="${strokeWidth}"
          stroke-dasharray="${dashArray.join(',')}"
          fill="none" />
      </svg>
    `;
  }

  render() {
    const {value, width, height} = this.state;

    return (
      <View
        {...this.props}
        onMeasure={this._onMeasure}
        onMouseDown={this._onMouseDown}
        onMouseUp={this._onMouseUp}
        onMouseDrag={this._onMouseDrag}>
        <Image {...styles.canvas} source={this._renderVectorGraphics(value, width, height)} />
        {this.props.children}
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
    'transform-rotate': Math.PI * 1.25,
  },
};

export default Slider;
