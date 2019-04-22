import React, { Component } from 'react';

import logo from './logo.svg';


// We'll need to wrap the default native components in stuff like this so that
// you can use <View> in your JSX. Otherwise we need the dynamic friendliness
// of the createElement call (note that the type is a string...);
function View(props) {
  return React.createElement('View', props, props.children);
}

function Text(props) {
  return React.createElement('Text', props, props.children);
}

function Image(props) {
  return React.createElement('Image', props, props.children);
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
      'background-color': 'ff141414',
      'justify-content': 'center',
      'align-items': 'center',
      'flex': 1.0,
      'padding': 20.0,
      'flex-wrap': 'wrap',
      ...this.props.style,
    };

    const childStyleObj = {
      'background-color': '77772727',
      'border-color': 'ffa7a7a7',
      'border-width': 2.0,
      'border-radius': 20.0,
      'flex': 0.0,
      'flex-direction': 'column-reverse',
      'height': '40.0%',
      'width': '25.0%',
      'justify-content': 'center',
      'align-items': 'center',
    };

    const styles = {
      text: {
        'background-color': '77aeaeae',
        'align-self': 'auto',
      },
      img: {
        'background-color': '77e7e7e7',
        'width': 52.0,
        'height': 34.0,
      },
    };

    return (
      <View debug="1" {...styleObj}>
        <View {...childStyleObj}>
          <Text {...styles.text}>
            The date today is: {'\n'}{this.state.date.toLocaleDateString()}
          </Text>
          <Image {...styles.img} source={logo} />
        </View>
        <View {...childStyleObj}>
          <Text {...styles.text}>
            The date today is: {'\n'}{this.state.date.toLocaleDateString()}
          </Text>
          <Image {...styles.img} source={logo} />
        </View>
        <View {...childStyleObj}>
          <Text {...styles.text}>
            The date today is: {'\n'}{this.state.date.toLocaleDateString()}
          </Text>
          <Image {...styles.img} source={logo} />
        </View>
      </View>
    );
  }
}

export default App;
