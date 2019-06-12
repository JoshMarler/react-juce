import { Colors, ParamIds, ParamLabels } from './Constants';
import React, { Component } from 'react';
import {
  EventBridge,
  Text,
  View,
} from 'juce-blueprint';

import debounce from 'lodash.debounce';


class ParameterLabel extends Component {
  constructor(props) {
    super(props);

    this._onParameterValueChange = this._onParameterValueChange.bind(this);
    this._debouncedResetLabel = debounce(this._resetLabel.bind(this), 500);

    this.state = {
      label: ParamLabels[this.props.paramId],
    };
  }

  shouldComponentUpdate(nextProps, nextState) {
    // TODO: Check props too...
    return this.state.label !== nextState.label;
  }

  componentDidMount() {
    EventBridge.addListener('parameterValueChange', this._onParameterValueChange);
  }

  componentWillUnmount() {
    EventBridge.removeListener('parameterValueChange', this._onParameterValueChange);
    this._debouncedResetLabel.cancel();
  }

  _onParameterValueChange(index, paramId, defaultValue, currentValue, stringValue) {
    if (paramId === this.props.paramId) {
      this.setState({
        label: stringValue,
      });

      this._debouncedResetLabel();
    }
  }

  _resetLabel() {
    this.setState({
      label: ParamLabels[this.props.paramId],
    });
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
    'color': Colors.STROKE,
    'font-size': 16.0,
    'font-family': 'open-sans',
    'line-spacing': 1.6,
  },
};

export default ParameterLabel;
