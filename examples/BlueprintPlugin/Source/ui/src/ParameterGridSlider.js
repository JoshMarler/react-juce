import React, { Component } from 'react';
import { View, Image, Text, NativeMethods } from './Blueprint';


class ParameterGridSlider extends Component {
  constructor(props) {
    super(props);

    this._onMeasure = this._onMeasure.bind(this);
    this._onMouseDown = this._onMouseDown.bind(this);
    this._onMouseDrag = this._onMouseDrag.bind(this);
    this._renderVectorGraphics = this._renderVectorGraphics.bind(this);

    // During a drag, we hold the value at which the drag started here to
    // ensure smooth behavior while the component state is being updated.
    this._valueAtDragStart = 0.0;

    this.state = {
      width: 0,
      height: 0,
      value: 0.0,
    };
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

    this.setState({
      value: value,
    });
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

    return `
      <svg
        width="${width}"
        height="${height}"
        viewBox="0 0 ${width} ${height}"
        version="1.1"
        xmlns="http://www.w3.org/2000/svg">
        <path d="${pathData.join(' ')}" stroke="#66FDCF" stroke-width="2.0" />
        <path d="${pathData2.join(' ')}" stroke="#62E7FD" stroke-width="2.0" />
      </svg>
    `;
  }

  render() {
    const {value, width, height} = this.state;

    return (
      <View {...this.props} onMeasure={this._onMeasure} onMouseDrag={this._onMouseDrag} >
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
