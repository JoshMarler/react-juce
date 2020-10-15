import React, { Component } from 'react';
import {
  EventBridge,
  Text,
  View,
} from 'juce-blueprint';


class Label extends Component {
  constructor(props) {
    super(props);

    this._onMouseEnter = this._onMouseEnter.bind(this);
    this._onMouseExit = this._onMouseExit.bind(this);
    this._onParameterValueChange = this._onParameterValueChange.bind(this);

    this.state = {
      label: '',
      color: 'ff626262'
    };
  }

  componentDidMount() {
    EventBridge.addListener('parameterValueChange', this._onParameterValueChange);
  }

  componentWillUnmount() {
    EventBridge.removeListener('parameterValueChange', this._onParameterValueChange);
  }

  _onParameterValueChange(index, paramId, defaultValue, currentValue, stringValue) {
    if (paramId === this.props.paramId) {
      this.setState({
        label: stringValue,
      });
    }
  }

  _onMouseEnter() {
    console.log('entered')
    this.setState({
      color: 'ffff7777'
    })
  }

  _onMouseExit() {
    console.log('exited')
    this.setState({
      color: 'ff626262'
    })
  }

  render() {
    return (
      <View {...this.props} onMouseEnter={this._onMouseEnter} onMouseExit={this._onMouseExit}>
        <Text {...styles.labelText} color={this.state.color}>
          {this.state.label}
        </Text>
      </View>
    );
  }
}

const styles = {
  labelText: {
    'font-size': 16.0,
    'line-spacing': 1.6,
  },
};

export default Label;
