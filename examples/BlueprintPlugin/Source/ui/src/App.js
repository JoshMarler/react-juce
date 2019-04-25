import Header from './Header';
import React, { Component } from 'react';

import logo from './icons/logo.svg';


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
      ...this.props.style,
    };

    const childStyleObj = {
      'background-color': '77772727',
      'border-color': 'ffa7a7a7',
      'border-width': 2.0,
      'border-top-left-radius': '50%',
      'border-top-right-radius': '50%',
      'border-bottom-right-radius': '50%',
      'border-bottom-left-radius': '50%',
      'flex': 0.0,
      'flex-direction': 'column-reverse',
      'height': '40.0%',
      'width': '15.0%',
      'margin': '0.4%',
      'justify-content': 'center',
      'align-items': 'center',
    };

    return (
      <View {...styles.container}>
        <Header {...styles.header}/>
        <View {...styles.content}>
          <View {...styles.grid}>
            <View {...styles.row}>
              <View {...styles.cell} {...styles.notchTopRight} />
              <View {...styles.cell} {...styles.notchTopLeft} {...styles.notchTopRight} />
              <View {...styles.cell} {...styles.notchTopRight} {...styles.notchTopLeft} />
              <View {...styles.cell} {...styles.notchTopLeft} {...styles.notchTopRight} />
              <View {...styles.cell} {...styles.notchTopRight} {...styles.notchTopLeft} />
              <View {...styles.cell} {...styles.notchTopLeft} />
            </View>
            <View {...styles.row}>
              <View {...styles.cell} />
              <View {...styles.cell} {...styles.notchBottomRight} />
              <View {...styles.cell} {...styles.notchBottomLeft} />
              <View {...styles.cell} {...styles.notchBottomRight} />
              <View {...styles.cell} {...styles.notchBottomLeft} />
              <View {...styles.cell} />
            </View>
          </View>
        </View>
      </View>
    );
  }
}

const Colors = {
  BACKGROUND: 'ff17191f',
  STROKE: 'ff626262',
};

const styles = {
  container: {
    'width': '100%',
    'height': '100%',
    'flex-direction': 'column',
    'background-color': Colors.BACKGROUND,
  },
  header: {
    'flex': 0.0,
    'height': '13.6%',
  },
  content: {
    'flex': 1.0,
    'padding': '3.4%',
  },
  grid: {
    'flex': 1.0,
    'flex-direction': 'column',
  },
  row: {
    'flex': 1.0,
    'flex-direction': 'row',
  },
  cell: {
    'flex': 1.0,
    'margin': '0.4%',
    'border-width': 2.0,
    'border-color': Colors.STROKE,
    'border-top-left-radius': '10%',
    'border-top-right-radius': '10%',
    'border-bottom-left-radius': '10%',
    'border-bottom-right-radius': '10%',
  },
  notchTopLeft: { 'border-top-left-radius': '-26%' },
  notchTopRight: { 'border-top-right-radius': '-26%' },
  notchBottomLeft: { 'border-bottom-left-radius': '-26%' },
  notchBottomRight: { 'border-bottom-right-radius': '-26%' },
};

export default App;
