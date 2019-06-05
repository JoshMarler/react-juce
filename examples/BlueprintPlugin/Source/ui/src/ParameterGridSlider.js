import ParameterValueStore from './ParameterValueStore';
import React, { Component } from 'react';
import {
  Image,
  NativeMethods,
  Text,
  View,
} from './Blueprint';

import throttle from 'lodash.throttle';
import { drawBorderPath } from './Drawing';


class ParameterGridSlider extends Component {
  constructor(props) {
    super(props);

    this._onMeasure = this._onMeasure.bind(this);
    this._onMouseDown = this._onMouseDown.bind(this);
    this._onMouseDrag = this._onMouseDrag.bind(this);
    this._renderVectorGraphics = this._renderVectorGraphics.bind(this);
    this._onParameterValueChange = this._onParameterValueChange.bind(this);
    this._throttleStateUpdate = throttle(this.setState, 32);

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
      NativeMethods.setParameterValueNotifyingHost(this.props.paramId, value);
    }
  }

  _onParameterValueChange(paramId) {
    const shouldUpdate = typeof this.props.paramId === 'string' &&
      this.props.paramId.length > 0 &&
      this.props.paramId === paramId;

    if (shouldUpdate) {
      const state = ParameterValueStore.getParameterState(paramId);

      this._throttleStateUpdate({
        value: state.currentValue,
      });
    }
  }

  _renderVectorGraphics(value, width, height) {
    let pathData = [];
    let pathData2 = [];

    let cy = height * 0.5;
    pathData.push(`M 0 ${cy}`);
    pathData2.push(`M 0 ${cy}`);

    for (let x = 0; x < width; x++) {
      let y1 = cy + 30 * Math.sin(4.0 * value * Math.PI * (x / width));
      let y2 = cy + 30 * Math.sin(4.0 * value * Math.PI * (0.5 + (x / width)));

      pathData.push(`L ${x} ${y1}`);
      pathData2.push(`L ${x} ${y2}`);
    }

    let inverseCornerRadius = -0.26 * Math.min(width, height);
    let cornerRadius = 0.1 * Math.min(width, height);

    // These props denote "notch top left," "notch top right," etc.
    let border = drawBorderPath(
      width,
      height,
      this.props.ntl ? inverseCornerRadius : cornerRadius,
      this.props.ntr ? inverseCornerRadius : cornerRadius,
      this.props.nbr ? inverseCornerRadius : cornerRadius,
      this.props.nbl ? inverseCornerRadius : cornerRadius,
    );

    return `
      <svg
        width="${width}"
        height="${height}"
        viewBox="0 0 ${width} ${height}"
        version="1.1"
        xmlns="http://www.w3.org/2000/svg">
        <path d="${pathData.join(' ')}" stroke="#66FDCF" stroke-width="2.0" />
        <path d="${pathData2.join(' ')}" stroke="#62E7FD" stroke-width="2.0" />
        <path d="${border.join(' ')}" stroke="#626262" stroke-width="2.0" fill="none" />
      </svg>
    `;
  }

  render() {
    const {value, width, height} = this.state;

    return (
      <View {...this.props} onMeasure={this._onMeasure} onMouseDown={this._onMouseDown} onMouseDrag={this._onMouseDrag} >
        <Image
          flex={1.0}
          height="100%"
          interceptClickEvents={false}
          source={this._renderVectorGraphics(value, width, height)} />
      </View>
    );
  }
}

export default ParameterGridSlider;
