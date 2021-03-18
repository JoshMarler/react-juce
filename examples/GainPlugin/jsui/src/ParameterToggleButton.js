import ParameterValueStore from "./ParameterValueStore";
import React, { Component } from "react";
import { Button } from "react-juce";
import {
  beginParameterChangeGesture,
  endParameterChangeGesture,
  setParameterValueNotifyingHost,
} from "./nativeMethods";

class ParameterToggleButton extends Component {
  constructor(props) {
    super(props);

    this._handleClick = this._handleClick.bind(this);
    this._handleEnter = this._handleEnter.bind(this);
    this._handleLeave = this._handleLeave.bind(this);
    this._onParameterValueChange = this._onParameterValueChange.bind(this);

    const paramState = ParameterValueStore.getParameterState(
      this.props.paramId
    );

    const initialDefaultValue =
      typeof paramState.defaultValue === "number"
        ? paramState.defaultValue
        : 0.0;

    const initialValue =
      typeof paramState.currentValue === "number"
        ? paramState.currentValue
        : 0.0;

    this.defaultBorderColor = "#66FDCF";
    this.hoverBorderColor = "#66CFFD";

    this.state = {
      defaultValue: initialDefaultValue,
      value: initialValue,
      borderColor: this.defaultBorderColor,
    };

    if (typeof this.props.onToggled === "function") {
      this.props.onToggled(initialValue !== 0.0);
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

  _handleClick(e) {
    const newValue = this.state.value === 0.0 ? 1.0 : 0.0;

    this.setState({
      value: newValue,
    });

    if (
      typeof this.props.paramId === "string" &&
      this.props.paramId.length > 0
    ) {
      setParameterValueNotifyingHost(this.props.paramId, newValue);
    }

    if (typeof this.props.onToggled === "function") {
      this.props.onToggled(newValue !== 0.0);
    }
  }

  _handleEnter(e) {
    beginParameterChangeGesture(this.props.paramId);
    this.setState({
      borderColor: this.hoverBorderColor,
    });
  }

  _handleLeave(e) {
    endParameterChangeGesture(this.props.paramId);
    this.setState({
      borderColor: this.defaultBorderColor,
    });
  }

  _onParameterValueChange(paramId) {
    const shouldUpdate =
      typeof this.props.paramId === "string" &&
      this.props.paramId.length > 0 &&
      this.props.paramId === paramId;

    if (shouldUpdate) {
      const state = ParameterValueStore.getParameterState(paramId);

      const newDefaultValue = state.defaultValue;
      const newValue = state.currentValue;

      this.setState({
        defaultValue: newDefaultValue,
        value: newValue,
      });

      if (typeof this.props.onToggled === "function") {
        this.props.onToggled(newValue !== 0.0);
      }
    }
  }

  render() {
    const { parameterId, onToggled, ...other } = this.props;

    return (
      <Button
        {...other}
        borderColor={this.state.borderColor}
        onClick={this._handleClick}
        onMouseEnter={this._handleEnter}
        onMouseLeave={this._handleLeave}
      >
        {this.props.children}
      </Button>
    );
  }
}

export default ParameterToggleButton;
