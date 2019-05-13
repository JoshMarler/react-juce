import Button from './Button';
import { Colors } from './Constants';
import React, { Component } from 'react';
import { View, Image, Text } from './Blueprint';


class ParameterToggleButton extends Component {
  constructor(props) {
    super(props);

    this._onMouseUp = this._onMouseUp.bind(this);

    this.state = {
      toggle: false,
    };
  }

  _onMouseUp(mouseX, mouseY) {
    const newValue = !this.state.toggle;

    if (typeof this.props.paramId === 'string' && this.props.paramId.length > 0) {
      __BlueprintNative__.setParameterValueNotifyingHost(this.props.paramId, newValue);
    }

    this.setState({
      toggle: newValue,
    });
  }

  render() {
    const toggleStyles = {
      'background-color': this.state.toggle ? Colors.SOFT_HIGHLIGHT : Colors.TRANSPARENT,
      'border-color': this.state.toggle ? Colors.HIGHLIGHT : Colors.STROKE,
    };

    return (
      <Button {...this.props} {...toggleStyles} onMouseUp={this._onMouseUp}>
        {this.props.children}
      </Button>
    );
  }
}

export default ParameterToggleButton;
