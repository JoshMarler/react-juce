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

  shouldComponentUpdate(nextProps, nextState) {
    return nextState.width !== this.state.width ||
      nextState.height !== this.state.height ||
      nextState.value !== this.state.value;
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
    let strokeWidth = 2.0;
    let halfStrokeWidth = 1.0;
    let inverseCornerRadius = -0.26 * Math.min(width, height);
    let cornerRadius = 0.1 * Math.min(width, height);

    // These props denote "notch top left," "notch top right," etc.
    // Note this little bounds trick. When a Path is stroked, the line width extends
    // outwards in both directions from the coordinate line. If the coordinate
    // line is the exact bounding box then the component clipping makes the corners
    // appear to have different radii on the interior and exterior of the box.
    let {border, length} = drawBorderPath(
      0 + halfStrokeWidth,
      0 + halfStrokeWidth,
      width - strokeWidth,
      height - strokeWidth,
      this.props.ntl ? inverseCornerRadius : cornerRadius,
      this.props.ntr ? inverseCornerRadius : cornerRadius,
      this.props.nbr ? inverseCornerRadius : cornerRadius,
      this.props.nbl ? inverseCornerRadius : cornerRadius,
    );

    // Animate the border by stroke-dasharray, where the length of the dash is
    // related to the value property and the length of the space takes up the
    // rest of the circumference.
    const dashArray = [0.999 * value * length, length];

    return `
      <svg
        width="${width}"
        height="${height}"
        viewBox="0 0 ${width} ${height}"
        version="1.1"
        xmlns="http://www.w3.org/2000/svg">
        ${this.props.drawFunction(value, width, height)}
        <path d="${border.join(' ')}" stroke="#626262" stroke-width="${strokeWidth}" fill="none" />
        <path d="${border.join(' ')}" stroke="#62E7FD" stroke-width="${strokeWidth}" fill="none" stroke-dasharray="${dashArray.join(',')}" />
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
