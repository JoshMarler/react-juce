/** Polyfill ES2015 data structures with core-js. */
import 'core-js/es6/set';
import 'core-js/es6/map';

import './lib/polyfill';

import BlueprintBackend from './lib/BlueprintBackend';
import BlueprintRenderer, { BlueprintTracedRenderer } from './lib/BlueprintRenderer';

export { default as NativeMethods } from './lib/NativeMethods';
export { default as EventBridge } from './lib/EventBridge';

export * from './components/View';
export * from './components/ScrollView';
export * from './components/Canvas';
export * from './components/Text';
export * from './components/Image';
export * from './components/Button';
export * from './components/Slider';

export * from './lib/SyntheticEvents';

let __renderStarted = false;
let __preferredRenderer = BlueprintRenderer;

export default {

  getRootContainer() {
    return BlueprintBackend.getRootContainer();
  },

  render(element: any, container: any, callback?: () => void | null | undefined) {
    console.log('Render started...');

    // Create a root Container if it doesnt exist
    if (!container._rootContainer) {
      //TODO: Double check passing false for final param "hydrate correct"
      container._rootContainer = __preferredRenderer.createContainer(container, false, false);
    }

    // Update the root Container
    // TODO: callback in __preferredRenderer.updateContainer is not optional.
    // @ts-ignore
    return __preferredRenderer.updateContainer(element, container._rootContainer, null, callback);
  },

  enableMethodTrace() {
    if (__renderStarted) {
      throw new Error('Cannot enable method trace after initial render.');
    }

    __preferredRenderer = BlueprintTracedRenderer;
  },

};
