import { Colors, ParamIds, ParamLabels } from './Constants';
import React, { Component } from 'react';
import {
  EventBridge,
  Text,
  View,
} from './Blueprint';

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
        <Text color={Colors.STROKE}>
          {this.state.label}
        </Text>
      </View>
    );
  }
}

export default ParameterLabel;
