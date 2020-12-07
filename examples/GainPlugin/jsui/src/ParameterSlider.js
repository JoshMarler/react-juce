import React, { Component } from 'react';
import {
    Slider
} from 'react-juce';

import ParameterValueStore from './ParameterValueStore';

class ParameterSlider extends Component {
    constructor(props) {
      super(props);

      this._onMouseDown = this._onMouseDown.bind(this);
      this._onMouseUp = this._onMouseUp.bind(this);
      this._onSliderValueChange = this._onSliderValueChange.bind(this);
      this._onParameterValueChange = this._onParameterValueChange.bind(this);

      const paramState = ParameterValueStore.getParameterState(this.props.paramId);
      const initialValue = typeof paramState.currentValue === 'number'
                            ? paramState.currentValue
                            : 0.0;

        this.state = {
          value: initialValue
        }
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

    _onMouseDown(e) {
      global.beginParameterChangeGesture(this.props.paramId);
    }

    _onMouseUp(e) {
      global.endParameterChangeGesture(this.props.paramId);
    }

    _onSliderValueChange(value) {
      if (typeof this.props.paramId === 'string' &&
                 this.props.paramId.length > 0) {
        global.setParameterValueNotifyingHost(this.props.paramId, value);
      }
    }

    _onParameterValueChange(paramId) {
      const shouldUpdate = typeof this.props.paramId === 'string' &&
                                  this.props.paramId.length > 0   &&
                                  this.props.paramId === paramId;

      if (shouldUpdate) {
        const state = ParameterValueStore.getParameterState(paramId);

        this.setState({
          defaultValue: state.defaultValue,
          value: state.currentValue,
        });
      }
    }

    render() {
      return (
        <Slider
          {...this.props}
          value={this.state.value}
          onMouseDown={this._onMouseDown}
          onMouseUp={this._onMouseUp}
          onChange={this._onSliderValueChange}
        >
          {this.props.children}
        </Slider>
      )
    }
}

export default ParameterSlider;
