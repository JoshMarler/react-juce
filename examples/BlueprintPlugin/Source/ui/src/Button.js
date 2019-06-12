import React, { Component } from 'react';
import { View, Image, Text } from 'juce-blueprint';


class Button extends Component {
  constructor(props) {
    super(props);

    this._onMouseDown = this._onMouseDown.bind(this);
    this._onMouseUp = this._onMouseUp.bind(this);

    this.state = {
      depressed: false,
    };
  }

  _onMouseDown(mouseX, mouseY) {
    if (typeof this.props.onMouseDown === 'function')
      this.props.onMouseDown.call(null, mouseX, mouseY);

    this.setState({
      depressed: true,
    });
  }

  _onMouseUp(mouseX, mouseY) {
    if (typeof this.props.onMouseUp === 'function')
      this.props.onMouseUp.call(null, mouseX, mouseY);

    this.setState({
      depressed: false,
    });
  }

  render() {
    const {onMouseUp, onMouseDown, ...other} = this.props;
    const opacity = this.state.depressed ? 0.5 : 1.0;

    return (
      <View
        onMouseDown={this._onMouseDown}
        onMouseUp={this._onMouseUp}
        opacity={opacity}
        {...other}>
        {this.props.children}
      </View>
    );
  }
}

export default Button;
