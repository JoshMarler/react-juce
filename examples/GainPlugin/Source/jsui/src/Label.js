import React, { Component } from 'react';
import {
  EventBridge,
  Text,
  View,
} from 'juce-blueprint';


class Label extends Component {
  constructor(props) {
    super(props);

    this._onParameterValueChange = this._onParameterValueChange.bind(this);

    this.state = {
      label: '',
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

  render() {
    return (
      <View {...this.props}>
        <Text {...styles.labelText}>
          {this.state.label}
        </Text>
      </View>
    );
  }
}

const styles = {
  labelText: {
    'color': 'ff626262',
    'font-size': 16.0,
    'line-spacing': 1.6,
  },
};

export default Label;
