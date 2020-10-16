import React, { Component } from 'react';
import {
  EventBridge,
  Text,
  View,
} from 'juce-blueprint';

const grey = 'ff626262';
const cyan = 'ff66fdcf';

class Label extends Component {
  constructor(props) {
    super(props);

    this._onParameterValueChange = this._onParameterValueChange.bind(this);

    this.state = {
      label: '',
      color: grey
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
        <Text
          {...styles.labelText}
          color={this.state.color}
          onMouseEnter={() => this.setState({ color: cyan })}
          onMouseExit={() => this.setState({ color: grey })}
        >
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
