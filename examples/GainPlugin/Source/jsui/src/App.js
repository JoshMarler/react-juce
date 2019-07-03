import React, { Component } from 'react';
import {
  View,
  Text,
} from 'juce-blueprint';


class App extends Component {
  render() {
    return (
      <View {...styles.container}>
        <View {...styles.content}>
          <Text {...styles.text}>
            Hello from React.js!
          </Text>
        </View>
      </View>
    );
  }
}

const styles = {
  container: {
    'width': '100%',
    'height': '100%',
    'background-color': 'ff17191f',
  },
  content: {
    'flex': 1.0,
    'justify-content': 'center',
    'align-items': 'center',
  },
  text: {
    'font-size': 18.0,
    'line-spacing': 1.6,
    'color': 'ff626262',
  },
};

export default App;
