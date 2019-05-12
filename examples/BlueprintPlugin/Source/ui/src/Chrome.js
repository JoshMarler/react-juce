import { Colors } from './Constants';
import React, { Component } from 'react';
import { View, Image, Text } from './Blueprint';

import productLogo from './icons/product-logo.svg';
import cog from './icons/cog.svg';
import info from './icons/info.svg';


class Chrome extends Component {
  render() {
    return (
      <View {...styles.container} {...this.props}>
        <View {...styles.header}>
          <Image {...styles.icon} source={info} />
          <Image {...styles.logo} source={productLogo} />
          <Image {...styles.icon} source={cog} />
        </View>
        {this.props.children}
      </View>
    );
  }
}

const styles = {
  container: {
    'flex-direction': 'column',
  },
  header: {
    'height': '10%',
    'flex': 0.0,
    'justify-content': 'space-between',
    'align-items': 'center',
    'padding-left': 16.0,
    'padding-right': 16.0,
  },
  logo: {
    'flex': 0.0,
    'height': '48%',
    'aspect-ratio': 405.0 / 28.0,
    'placement': Image.PlacementFlags.centred,
  },
  icon: {
    'flex': 0.0,
    'height': '48%',
    'aspect-ratio': 1.0,
    'placement': Image.PlacementFlags.centred,
  }
};

export default Chrome;
