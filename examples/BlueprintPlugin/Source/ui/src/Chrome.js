import Button from './Button';
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
          <Button {...styles.button} onMouseUp={this.props.onToggleHelp}>
            <Image interceptClickEvents={false} {...styles.icon} source={info} />
          </Button>
          <Image {...styles.logo} source={productLogo} />
          <Button {...styles.button} onMouseUp={this.props.onToggleSettings}>
            <Image interceptClickEvents={false} {...styles.icon} source={cog} />
          </Button>
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
    'flex-shrink': 0.0,
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
  button: {
    'flex': 0.0,
    'height': '48%',
    'aspect-ratio': 1.0,
  },
  icon: {
    'height': '100%',
    'width': '100%',
    'placement': Image.PlacementFlags.centred,
  }
};

export default Chrome;
