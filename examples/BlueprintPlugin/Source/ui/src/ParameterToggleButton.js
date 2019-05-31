import Button from './Button';
import { Colors } from './Constants';
import React, { Component } from 'react';
import {
  EventBridge,
  Image,
  NativeMethods,
  Text,
  View,
} from './Blueprint';

import throttle from 'lodash.throttle';


class ParameterToggleButton extends Component {
  constructor(props) {
    super(props);

    this._onMouseUp = this._onMouseUp.bind(this);
    this._onParameterValueChange = this._onParameterValueChange.bind(this);
    this._throttleStateUpdate = throttle(this.setState, 32);

    this.state = {
      toggle: false,
    };
  }

  componentDidMount() {
    EventBridge.addListener('parameterValueChange', this._onParameterValueChange);
  }

  componentWillUnmount() {
    EventBridge.removeListener('parameterValueChange', this._onParameterValueChange);
  }

  _onMouseUp(mouseX, mouseY) {
    const newValue = !this.state.toggle;

    if (typeof this.props.paramId === 'string' && this.props.paramId.length > 0) {
      NativeMethods.setParameterValueNotifyingHost(this.props.paramId, newValue);
    }
  }

  _onParameterValueChange(index, paramId, defaultValue, currentValue, stringValue) {
    const shouldUpdate = typeof this.props.paramId === 'string' &&
      this.props.paramId.length > 0 &&
      this.props.paramId === paramId;

    if (shouldUpdate) {
      this._throttleStateUpdate({
        toggle: currentValue > 0.5,
      });
    }
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
