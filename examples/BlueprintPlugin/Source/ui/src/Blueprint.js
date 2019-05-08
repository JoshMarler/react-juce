import BlueprintBackend from './blueprint/BlueprintBackend';
import BlueprintRenderer from './blueprint/BlueprintRenderer';
import React, { Component } from 'react';


// We'll need to wrap the default native components in stuff like this so that
// you can use <View> in your JSX. Otherwise we need the dynamic friendliness
// of the createElement call (note that the type is a string...);
export function View(props) {
  return React.createElement('View', props, props.children);
}

export function Text(props) {
  return React.createElement('Text', props, props.children);
}

export function Image(props) {
  return React.createElement('Image', props, props.children);
}


export default {

  getRootContainer() {
    return BlueprintBackend.getRootContainer();
  },

  render(element, container, callback) {
    console.log('Render started...');

    // Create a root Container if it doesnt exist
    if (!container._rootContainer) {
      container._rootContainer = BlueprintRenderer.createContainer(container, false);
    }

    // Update the root Container
    return BlueprintRenderer.updateContainer(element, container._rootContainer, null, callback);
  },

};
