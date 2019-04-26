import React, { Component } from 'react';


// TODO: Put this inside blueprint...
function View(props) {
  return React.createElement('View', props, props.children);
}

function Text(props) {
  return React.createElement('Text', props, props.children);
}

function Image(props) {
  return React.createElement('Image', props, props.children);
}

class TestLayout01 extends Component {
  render() {
    return (
      <View {...styles.container} {...this.props}>
        <View {...styles.inner}>
        </View>
      </View>
    );
  }
}

const styles = {
  container: {
    'flex-direction': 'row',
    'justify-content': 'center',
    'background-color': 'ff884848',
    // FIXME: There's something wrong with percentage padding on flex children.
    // This test layout shows uneven padding distribution for the same given values,
    // but only for the bottom edge.
    'padding-left': '2%',
    'padding-top': '2%',
    'padding-right': '2%',
    'padding-bottom': '2%',
  },
  inner: {
    'flex': 1.0,
    'justify-content': 'space-between',
    'background-color': 'ff488848',
  },
};

export default TestLayout01;
