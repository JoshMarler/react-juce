import React, { Component } from 'react';

import logo from './logo.svg';

console.log(logo);


// We'll need to wrap the default native components in stuff like this so that
// you can use <View> in your JSX. Otherwise we need the dynamic friendliness
// of the createElement call (note that the type is a string...);
function View(props) {
  return React.createElement('View', props, props.children);
}

function Text(props) {
  return React.createElement('Text', props, props.children);
}


class App extends Component {
  constructor(props) {
    super(props);

    this._timerId = null;

    this.state = {
      date: new Date(),
    };
  }

  tick() {
    // this.setState({
    //   date: new Date(),
    // });
  }

  componentDidMount() {
    // this._timerId = setInterval(
    //   () => this.tick(),
    //   1000
    // );
  }

  componentWillUnmount() {
    // clearInterval(this._timerId);
  }

  render() {
    const styleObj = {
      'background-color': 'ff272777',
      'align-items': 'center',
      'flex': 1.0,
      'padding': 20.0,
      ...this.props.style,
    };

    const childStyleObj = {
      'background-color': '77772727',
      'flex': 1.0,
      'height': 200.0,
      'margin-horizontal': 80.0,
      'justify-content': 'center',
    };

    const styles = {
      text: {
        'background-color': '77aeaeae',
        'align-self': 'auto',
      },
    };

    return (
      <View debug="1" {...styleObj}>
        <View {...childStyleObj}>
          <Text {...styles.text}>
            The date today is: {'\n'}{this.state.date.toLocaleDateString()}
          </Text>
        </View>
      </View>
    );
  }
}

export default App;
