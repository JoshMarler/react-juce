import BlueprintBackend from './lib/BlueprintBackend';
import BlueprintRenderer, { BlueprintTracedRenderer } from './lib/BlueprintRenderer';
import React, { Component } from 'react';

import invariant from 'invariant';

export { default as NativeMethods } from './lib/NativeMethods';
export { default as EventBridge } from './lib/EventBridge';

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

export function Canvas(props) {
  if (props.hasOwnProperty('onDraw')) {
    let userOnDraw = props.onDraw;
    let wrappedDraw = function(ctx) {
      Object.defineProperty(ctx, 'fillStyle', {
        enumerable: false,
        configurable: false,
        get: function() {
          return 'Not Supported';
        },
        set: function(value) {
          this.__setFillStyle(value);
        }
      });

      Object.defineProperty(ctx, 'strokeStyle', {
        enumerable: false,
        configurable: false,
        get: function() {
          return 'Not Supported';
        },
        set: function(value) {
          this.__setStrokeStyle(value);
        }
      });

      Object.defineProperty(ctx, 'lineWidth', {
        enumerable: false,
        configurable: false,
        get: function() {
          return 'Not Supported';
        },
        set: function(value) {
          this.__setLineWidth(value);
        }
      });

      return userOnDraw(ctx);
    };

    return React.createElement('CanvasView', Object.assign({}, props, {
      onDraw: wrappedDraw,
    }), props.children);
  }

  return React.createElement('CanvasView', props, props.children);
}

function ScrollViewContentView(props) {
  return React.createElement('ScrollViewContentView', props, props.children);
}

export function ScrollView(props) {
  const child = React.Children.only(props.children);

  invariant(
    child.type === ScrollViewContentView,
    'ScrollView must have only one child, and that child must be a ScrollView.ContentView.'
  );

  return React.createElement('ScrollView', props, child);
}

ScrollView.ContentView = ScrollViewContentView;

View.ClickEventFlags = {
  disableClickEvents: 0,
  allowClickEvents: 1,
  allowClickEventsExcludingChildren: 2,
  allowClickEventsOnlyOnChildren: 3,
};

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

Text.WordWrap = {
  none: 0,
  byWord: 1,
  byChar: 2,
};

Text.FontStyleFlags = {
  plain: 0,
  bold: 1,
  italic: 2,
  underlined: 4,
};

Text.JustificationFlags = {
  left: 1,
  right: 2,
  horizontallyCentred: 4,
  top: 8,
  bottom: 16,
  verticallyCentred: 32,
  horizontallyJustified: 64,
  centred: 36,
  centredLeft: 33,
  centredRight: 34,
  centredTop: 12,
  centredBottom: 20,
  topLeft: 9,
  topRight: 10,
  bottomLeft: 17,
  bottomRight: 18,
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
