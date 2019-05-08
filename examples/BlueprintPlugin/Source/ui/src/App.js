import Chrome from './Chrome';
import { Colors } from './Constants';
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

    this.state = {
      viewMode: ViewModes.PARAMETERS,
    };
  }

  render() {
    switch (this.state.viewMode) {
      case ViewModes.PARAMETERS:
        return (
          <Chrome {...styles.chrome}>
            <ParameterGrid {...styles.contentContainer} />
          </Chrome>
        );
      case ViewModes.SETTINGS:
      case ViewModes.HELP:
      default:
        return (
          <View />
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
