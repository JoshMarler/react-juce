import React, { Component } from 'react';
import { inspect } from 'util';

// TODO: Put this inside blueprint...
function View(props) {
  return React.createElement('View', props, props.children);
}

function Text(props) {
  return React.createElement('Text', props, props.children);
}

function Image(props) {
  return React.createElement('Image', props, props.children);
}


class PatternView extends Component {
  constructor(props) {
    super(props);

    this._onMeasure = this._onMeasure.bind(this);
    this._onMouseDrag = this._onMouseDrag.bind(this);
    this._renderVectorGraphics = this._renderVectorGraphics.bind(this);

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

  _onMouseDrag(mouseX, mouseY, mouseDownX, mouseDownY) {
    // Component vectors
    let dx = mouseX - mouseDownX;
    let dy = mouseDownY - mouseY;

    // Delta
    let dm = dx + dy;
    let sensitivity = (1.0 / 400.0);
    let value = Math.max(0.0, Math.min(1.0, this.state.value + dm * sensitivity));

    // TODO: This "NativeMethods" interface is just a proxy to __BlueprintNative__
    // to check that you've actually registered it before pushing the call.
    // NativeMethods.setParameterValueNotifyingHost(this.props.paramId, value);
    __BlueprintNative__.setParameterValueNotifyingHost("hay", value);

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

    const dashArray = [value*360, (1.0 - value) * 360];

    return `
      <svg
        width="${width}"
        height="${height}"
        viewBox="0 0 ${width} ${height}"
        version="1.1"
        xmlns="http://www.w3.org/2000/svg">
        <rect x="10" y="10" width="80" height="100" stroke="#62E7FD" stroke-dasharray="${dashArray.join(',')}" stroke-width="2.0" fill="none" />
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

export default PatternView;
