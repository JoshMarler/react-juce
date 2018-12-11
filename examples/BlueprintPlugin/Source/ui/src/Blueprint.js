
import BlueprintBackend from './blueprint/BlueprintBackend';
import BlueprintRenderer from './blueprint/BlueprintRenderer';

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
