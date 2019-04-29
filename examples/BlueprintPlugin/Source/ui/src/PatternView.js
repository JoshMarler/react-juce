import React, { Component } from 'react';
import { inspect } from 'util';

// TODO: Put this inside blueprint...
function View(props) {
  return React.createElement('View', props, props.children);
}

function Text(props) {
  return React.createElement('Text', props, props.children);
}

class Image extends Component {
  render() {
    return React.createElement('Image', this.props, this.props.children);
  }
}

class PatternView extends Component {
  constructor(props) {
    super(props);

    this._onMeasure = this._onMeasure.bind(this);
    this._renderVectorGraphics = this._renderVectorGraphics.bind(this);
    this._ref = React.createRef();

    this.state = {
      source: this._renderVectorGraphics(0, 0),
    };
  }

  _onMeasure(width, height) {
    this.setState({
      source: this._renderVectorGraphics(width, height),
    });
  }

  _renderVectorGraphics(width, height) {
    let pathData = [];
    let pathData2 = [];

    let cy = height * 0.5;
    pathData.push(`M 0 ${cy}`);
    pathData2.push(`M 0 ${cy}`);

    for (let x = 0; x < width; x++) {
      let y1 = cy + 30 * Math.sin(0.05 * width * Math.PI * (x / width));
      let y2 = cy + 30 * Math.sin(0.05 * width * Math.PI * (0.5 + (x / width)));

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
    return (
      <Image ref={this._ref} source={this.state.source} {...this.props} onMeasure={this._onMeasure} />
    );
  }
}

export default PatternView;
