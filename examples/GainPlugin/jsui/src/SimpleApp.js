import React, { Component } from 'react';
import {
  View,
} from 'juce-blueprint';

export default class SimpleApp extends Component {
  constructor (props) {
    super(props);
    this.state = {
      color: 'ffffffff'
    };
  }

  render () {
    return (
      <View
        { ...styles.container }
        background-color={ this.state.color }
        onMouseDown={ () => this.setState({ color: 'ff22ffff' }) }  // light blue
        onMouseUp={ () => this.setState({ color: 'ffffffff' }) }
        onMouseEnter={ () => this.setState({ color: 'ffff22ff' }) }  // purple
        onMouseExit={ () => this.setState({ color: 'ffffffff' }) }
      />
    );
  }
}

const styles = {
  container: {
    'width': '100%',
    'height': '100%',
  },
};
