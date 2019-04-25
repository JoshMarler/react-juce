import React, { Component } from 'react';

import productLogo from './icons/product-logo.svg';
import cog from './icons/cog.svg';
import info from './icons/info.svg';


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

class Header extends Component {
  render() {
    return (
      <View {...styles.container} {...this.props}>
        <View {...styles.inner}>
          <Image {...styles.icon} source={info} />
          <Image {...styles.logo} source={productLogo} />
          <Image {...styles.icon} source={cog} />
        </View>
      </View>
    );
  }
}

const styles = {
  container: {
    'flex-direction': 'row',
    'align-items': 'center',
    'justify-content': 'center',
  },
  inner: {
    'flex': 0.0,
    'width': '92%',
    'height': '26%',
    'justify-content': 'space-between',
  },
  logo: {
    'flex': 0.0,
    'height': '100%',
    'aspect-ratio': 405.0 / 28.0,
  },
  icon: {
    'flex': 0.0,
    'height': '100%',
    'aspect-ratio': 1.0,
  }
};

export default Header;
