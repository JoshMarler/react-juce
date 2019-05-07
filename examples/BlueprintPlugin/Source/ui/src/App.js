import GlobalControls from './GlobalControls';
import Header from './Header';
import PatternView from './PatternView';
import React, { Component } from 'react';
import TestLayout01 from './TestLayout01';

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

    this._onMeasure = this._onMeasure.bind(this);
  }

  _onMeasure(width, height) {
    console.log(width, height);
  }

  render() {
    if (false) {
      return (
        <View {...styles.container}>
          <TestLayout01 {...styles.header} />
        </View>
      );
    }

    return (
      <View {...styles.container}>
        <Header {...styles.header}/>
        <View {...styles.content}>
          <GlobalControls {...styles.globals} />
          <View {...styles.gridContainer}>
            <View {...styles.gutter}>
              <Text color={Colors.STROKE} {...{'transform-rotate': Math.PI * -0.5}}>
                GRAIN 1
              </Text>
              <Text color={Colors.STROKE} {...{'transform-rotate': Math.PI * -0.5}}>
                GRAIN 2
              </Text>
            </View>
            <View {...styles.grid}>
              <View {...styles.row}>
                <PatternView {...styles.cell} {...styles.notchTopRight} {...{'transform-rotate': Math.PI * 0.25}} />
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
                <View {...styles.cell} {...{'transform-rotate': Math.PI * 0.25}} />
              </View>
            </View>
            <View {...styles.gutter}>
              <Text color={Colors.STROKE} {...{'transform-rotate': Math.PI * 0.5}}>
                GRAIN 1
              </Text>
              <Text color={Colors.STROKE} {...{'transform-rotate': Math.PI * 0.5}}>
                GRAIN 2
              </Text>
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
  DBG_RED: 'ff884848',
  DBG_GREEN: 'ff488848',
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
    'flex-direction': 'column',
    'padding-left': 16,
    'padding-right': 16,
  },
  globals: {
    'flex': 0.0,
    'height': '15%',
  },
  gutter: {
    'flex-direction': 'column',
    'justify-content': 'space-around',
    'align-items': 'center',
    'flex': 0.0,
    'width': 32,
  },
  gridContainer: {
    'flex': 1.0,
  },
  grid: {
    'flex': 1.0,
    'flex-direction': 'column',
    // Ensures that the cell margin doesn't create "padding" at the grid level
    'margin': '-0.4%',
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
