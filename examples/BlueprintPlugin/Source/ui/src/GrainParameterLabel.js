import { Colors, ParamIds, ParamLabels } from './Constants';
import React, { Component } from 'react';
import {
  EventBridge,
  Text,
  View,
} from './Blueprint';

import debounce from 'lodash.debounce';


class GrainParameterLabel extends Component {
  constructor(props) {
    super(props);

    this._onParameterValueChange = this._onParameterValueChange.bind(this);
    this._debouncedResetLabel = debounce(this._resetLabel.bind(this), 500);

    EventBridge.addListener('parameterValueChange', this._onParameterValueChange);

    this.state = {
      label: ParamLabels[this.props.g1ParamId],
    };
  }

  shouldComponentUpdate(nextProps, nextState) {
    return this.state.label !== nextState.label ||
      this.props.g1ParamId !== this.props.g1ParamId ||
      this.props.g2ParamId !== this.props.g2ParamId;
  }

  componentWillUnmount() {
    EventBridge.removeListener('parameterValueChange', this._onParameterValueChange);
  }

  _onParameterValueChange(index, paramId, defaultValue, currentValue, stringValue) {
    if (paramId === this.props.g1ParamId || paramId === this.props.g2ParamId) {
      this.setState({
        label: stringValue,
      });

      this._debouncedResetLabel();
    }
  }

  _resetLabel() {
    this.setState({
      label: ParamLabels[this.props.g1ParamId],
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

export default GrainParameterLabel;
