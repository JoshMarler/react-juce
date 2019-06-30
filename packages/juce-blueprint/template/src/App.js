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
            Hello world!
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
    'background-color': '272727',
  },
  content: {
    'flex': 1.0,
    'justify-content': 'center',
    'align-items': 'center',
  },
  text: {
    'font-size': 16.0,
    'line-spacing': 1.6,
    'color': 'e7e7e7',
  },
};

export default App;
