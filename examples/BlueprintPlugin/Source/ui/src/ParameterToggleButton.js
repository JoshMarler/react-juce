import Button from './Button';
import { Colors } from './Constants';
import ParameterValueStore from './ParameterValueStore';
import React, { Component } from 'react';
import {
  Image,
  NativeMethods,
  Text,
  View,
} from 'juce-blueprint';

import throttle from 'lodash.throttle';


class ParameterToggleButton extends Component {
  constructor(props) {
    super(props);

    this._onMouseUp = this._onMouseUp.bind(this);
    this._onParameterValueChange = this._onParameterValueChange.bind(this);
    this._throttleStateUpdate = throttle(this.setState, 32);

    const paramState = ParameterValueStore.getParameterState(this.props.paramId);
    const initialValue = typeof paramState.currentValue === 'number' ?
      paramState.currentValue : 0.0;

    this.state = {
      toggle: initialValue > 0.5,
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

  _onMouseUp(mouseX, mouseY) {
    const newValue = !this.state.toggle;

    if (typeof this.props.paramId === 'string' && this.props.paramId.length > 0) {
      NativeMethods.setParameterValueNotifyingHost(this.props.paramId, newValue);
    }
  }

  _onParameterValueChange(paramId) {
    const shouldUpdate = typeof this.props.paramId === 'string' &&
      this.props.paramId.length > 0 &&
      this.props.paramId === paramId;

    if (shouldUpdate) {
      const state = ParameterValueStore.getParameterState(paramId);

      this._throttleStateUpdate({
        toggle: state.currentValue > 0.5,
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
