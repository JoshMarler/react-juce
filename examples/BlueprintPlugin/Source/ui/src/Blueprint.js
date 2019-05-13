import BlueprintBackend from './blueprint/BlueprintBackend';
import BlueprintRenderer, { BlueprintTracedRenderer } from './blueprint/BlueprintRenderer';
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

Image.PlacementFlags = {
  xLeft: 1,
  xRight: 2,
  xMid: 4,
  yTop: 8,
  yBottom: 16,
  yMid: 32,
  stretchToFit: 64,
  fillDestination: 128,
  onlyReduceInSize: 256,
  onlyIncreaseInSize: 512,
  doNotResize: 256 | 512,
  centred: 4 + 32,
};


let __renderStarted = false;
let __preferredRenderer = BlueprintRenderer;

export default {

  getRootContainer() {
    return BlueprintBackend.getRootContainer();
  },

  render(element, container, callback) {
    console.log('Render started...');

    // Create a root Container if it doesnt exist
    if (!container._rootContainer) {
      container._rootContainer = __preferredRenderer.createContainer(container, false);
    }

    // Update the root Container
    return __preferredRenderer.updateContainer(element, container._rootContainer, null, callback);
  },

  enableMethodTrace() {
    if (__renderStarted) {
      throw new Error('Cannot enable method trace after initial render.');
    }

    __preferredRenderer = BlueprintTracedRenderer;
  },

};
