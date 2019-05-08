import { Colors } from './Constants';
import React, { Component } from 'react';
import { View, Image, Text } from './Blueprint';


class ParameterGrid extends Component {
  render() {
    return (
      <View {...styles.container} {...this.props}>
        <View {...styles.contentHeader}>
          <GridLabel {...styles.shiftLeft}>DELAY</GridLabel>
          <GridLabel {...styles.shiftLeft}>FILTER</GridLabel>
          <GridLabel {...styles.shiftLeft}>WARP</GridLabel>
          <GridLabel {...styles.shiftLeft}>ENV</GridLabel>
          <GridLabel {...styles.shiftLeft}>MIX</GridLabel>
        </View>
        <View {...styles.grid}>
          <View {...styles.row}>
            <Gutter direction={-1.0}>GRAIN 1</Gutter>
            <View {...styles.cell} {...styles.notchTopRight} />
            <View {...styles.cell} {...styles.notchTopLeft} {...styles.notchTopRight} />
            <View {...styles.cell} {...styles.notchTopRight} {...styles.notchTopLeft} />
            <View {...styles.cell} {...styles.notchTopLeft} {...styles.notchTopRight} />
            <View {...styles.cell} {...styles.notchTopRight} {...styles.notchTopLeft} />
            <View {...styles.cell} {...styles.notchTopLeft} />
            <Gutter direction={1.0}>GRAIN 1</Gutter>
          </View>
          <View {...styles.row}>
            <Gutter direction={-1.0}>GRAIN 2</Gutter>
            <View {...styles.cell} />
            <View {...styles.cell} {...styles.notchBottomRight} />
            <View {...styles.cell} {...styles.notchBottomLeft} />
            <View {...styles.cell} {...styles.notchBottomRight} />
            <View {...styles.cell} {...styles.notchBottomLeft} />
            <View {...styles.cell} />
            <Gutter direction={1.0}>GRAIN 2</Gutter>
          </View>
        </View>
        <View {...styles.contentFooter}>
          <GridLabel>FREQUENCY</GridLabel>
          <GridLabel>SPRAY</GridLabel>
          <GridLabel>SPREAD</GridLabel>
          <GridLabel>PITCH</GridLabel>
          <GridLabel>REVERSE</GridLabel>
          <GridLabel>FEEDBACK</GridLabel>
        </View>
      </View>
    );
  }
}

function GridLabel(props) {
  return (
    <View {...styles.labelContainer} {...props}>
      <Text color={Colors.STROKE}>{props.children}</Text>
    </View>
  );
}

function Gutter(props) {
  return (
    <View {...styles.gutter}>
      <Text color={Colors.STROKE} {...{'transform-rotate': Math.PI * 0.5 * props.direction}}>
        {props.children}
      </Text>
    </View>
  );
}

const styles = {
  container: {
    'flex-direction': 'column',
  },
  contentHeader: {
    'height': `${100.0 / 9.0}%`, // Equivalent to the Chrome's header row height
    'flex': 0.0,
    'justify-content': 'center',
    'padding-left': 32.0,
    'padding-right': 32.0,
  },
  contentFooter: {
    'height': `${100.0 / 9.0}%`, // Equivalent to the Chrome's header row height
    'flex': 0.0,
    'justify-content': 'center',
    'padding-left': 32.0,
    'padding-right': 32.0,
  },
  labelContainer: {
    'flex': 0.0,
    'width': `${100.0 / 6.0}%`,
    'justify-content': 'center',
  },
  shiftLeft: {
    'position': 'relative',
    'left': '-8%',
  },
  grid: {
    'flex': 1.0,
    'flex-direction': 'column',
  },
  gutter: {
    'flex': 0.0,
    'width': 32.0,
    'justify-content': 'center',
    'align-items': 'center',
  },
  row: {
    'flex': 1.0,
    'flex-direction': 'row',
  },
  cell: {
    'flex': 1.0,
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

export default ParameterGrid;
