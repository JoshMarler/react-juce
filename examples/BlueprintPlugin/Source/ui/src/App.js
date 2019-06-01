import Chrome from './Chrome';
import { Colors } from './Constants';
import Help from './Help';
import ParameterGrid from './ParameterGrid';
import React, { Component } from 'react';
import { View, Image, Text } from './Blueprint';


const ViewModes = {
  PARAMETERS: 1,
  SETTINGS: 2,
  HELP: 3,
};

class App extends Component {
  constructor(props) {
    super(props);

    this._onToggleSettings = this._onToggleSettings.bind(this);
    this._onToggleHelp = this._onToggleHelp.bind(this);

    this.state = {
      viewMode: ViewModes.PARAMETERS,
    };
  }

  _onToggleSettings(mouseX, mouseY) {
    // If we're already showing the help screen, close it.
    if (this.state.viewMode === ViewModes.SETTINGS) {
      return this.setState({
        viewMode: ViewModes.PARAMETERS,
      });
    }

    this.setState({
      viewMode: ViewModes.SETTINGS,
    });
  }

  _onToggleHelp(mouseX, mouseY) {
    // If we're already showing the help screen, close it.
    if (this.state.viewMode === ViewModes.HELP) {
      return this.setState({
        viewMode: ViewModes.PARAMETERS,
      });
    }

    this.setState({
      viewMode: ViewModes.HELP,
    });
  }

  render() {
    switch (this.state.viewMode) {
      case ViewModes.PARAMETERS:
        return (
          <Chrome
            {...styles.chrome}
            onToggleSettings={this._onToggleSettings}
            onToggleHelp={this._onToggleHelp}>
            <ParameterGrid {...styles.contentContainer} />
          </Chrome>
        );
      case ViewModes.SETTINGS:
        return (
          <Chrome
            {...styles.chrome}
            onToggleSettings={this._onToggleSettings}
            onToggleHelp={this._onToggleHelp}>
            <View {...styles.contentContainer}>
              <Text color={Colors.STROKE}>
                Preset & settings stuff coming soon...
              </Text>
            </View>
          </Chrome>
        );
      case ViewModes.HELP:
        return (
          <Chrome
            {...styles.chrome}
            onToggleSettings={this._onToggleSettings}
            onToggleHelp={this._onToggleHelp}>
            <Help {...styles.contentContainer} />
          </Chrome>
        );
      default:
        return (
          <Chrome
            {...styles.chrome}
            onToggleSettings={this._onToggleSettings}
            onToggleHelp={this._onToggleHelp}>
            <View {...styles.contentContainer}>
              <Text color={Colors.STROKE}>
                Something went wrong... you should not be seeing this!
              </Text>
            </View>
          </Chrome>
        );
    }
  }
}

const styles = {
  chrome: {
    'width': '100%',
    'height': '100%',
    'background-color': Colors.BACKGROUND,
  },
  contentContainer: {
    'flex': 1.0,
  },
};

export default App;
