
import MethodTracer from './MethodTracer';
import ReactReconciler from 'react-reconciler';
import BlueprintBackend from './BlueprintBackend';

const HostConfig = new Proxy({
  /** Time provider. */
  now: Date.now,

  /** Indicates to the reconciler that our DOM tree supports mutating operations
   *  like appendChild, removeChild, etc.
   */
  supportsMutation: true,

  getRootHostContext: (...args) => {
    return {};
  },

  getChildHostContext: (...args) => {
    return {};
  },

  prepareForCommit: (...args) => {},
  resetAfterCommit: (...args) => {},

  /** Called to determine whether or not a new text value can be set on an
   *  existing node, or if a new text node needs to be created.
   *
   *  @param {String} elementType
   *  @param {Object} props
   */
  shouldSetTextContent(elementType, props) {
    return false;
  },

  /** Create a new DOM node.
   *
   *  @param {String} elementType
   *  @param {Object} props
   *  @param {Container} rootContainerInstance
   *  @param {Object} hostContext
   *  @param {Object} internalInstanceHandle
   */
  createInstance(elementType, props, rootContainerInstance, hostContext, internalInstanceHandle) {
    return BlueprintBackend.createViewInstance(elementType, props, rootContainerInstance);
  },

  /** Create a new text node.
   *
   *  @param {String} text
   *  @param {Container} rootContainerInstance
   *  @param {Object} hostContext
   *  @param {Object} internalInstanceHandle
   */
  createTextInstance(text, rootContainerInstance, hostContext, internalInstanceHandle) {
    return {};
    // return document.createTextNode(text);
  },

  /** Mount the child to its container.
   *
   *  @param {Instance} parentInstance
   *  @Param {Instance} child
   */
  appendInitialChild(parentInstance, child) {
    parentInstance.appendChild(child);
  },

  /** For each newly constructed node, once we finish the assignment of children
   *  this method will be called to finalize the node. We take this opportunity
   *  to propagate relevant properties to the node.
   *
   *  @param {Instance} instance
   *  @param {String} elementType
   *  @param {Object} props
   *  @param {Instance} rootContainerInstance
   */
  finalizeInitialChildren(instance, elementType, props, rootContainerInstance) {
    Object.keys(props).forEach(function(propKey) {
      if (propKey !== 'children') {
        instance.setProperty(propKey, props[propKey]);
      }
    });
  },

  /** During a state change, this method will be called to identify the set of
   *  properties that need to be updated. This is more-or-less an opportunity
   *  for us to diff our props before propagating.
   *
   *  @param {Instance} instance
   *  @param {String} elementType
   *  @param {Object} oldProps
   *  @param {Object} newProps
   *  @param {Instance} rootContainerInstance
   *  @param {Object} hostContext
   */
  prepareUpdate(domElement, elementType, oldProps, newProps, rootContainerInstance, hostContext) {
    // The children prop will be handled separately via the tree update.
    let {children: oldChildren, ...op} = oldProps;
    let {children: newChildren, ...np} = newProps;

    // We construct a new payload merging the new properties into the set of
    // remaining properties from the previous state.
    return {
      ...op,
      ...np,
    };
  },

  /** Following from `prepareUpdate` above, this is our opportunity to apply
   *  the update payload to a given instance.
   *
   *  @param {Instance} instance
   *  @param {Object} updatePayload
   *  @param {String} elementType
   *  @param {Object} oldProps
   *  @param {Object} newProps
   *  @param {Object} internalInstanceHandle
   */
  commitUpdate(instance, updatePayload, elementType, oldProps, newProps, internalInstanceHandle) {
    Object.keys(updatePayload).forEach(function(propKey) {
      instance.setProperty(propKey, updatePayload[propKey]);
    });
  },

  /** Similar to the previous method, this is our opportunity to apply text
   *  updates to a given instance.
   *
   *  @param {Instance} instance
   *  @param {String} oldText
   *  @param {String} newText
   */
  commitTextUpdate(instance, oldText, newText) {
    console.log('noop got hit');
    // instance.nodeValue = newText;
  },

  /** TODO
   *
   *  @param {Instance} instance
   *  @param {String} type
   *  @param {Object} newProps
   *  @param {Instance} internalInstanceHandle
   */
  commitMount(instance, type, newProps, internalInstanceHandle) {
    // Noop
  },

  /** Append a child to a parent instance.
   *
   *  @param {Instance} parentInstance
   *  @Param {Instance} child
   */
  appendChild(parentInstance, child) {
    parentInstance.appendChild(child);
  },

  /** Append a child to a parent container.
   *  TODO: Not really sure how this is different from the above.
   *
   *  @param {Container} parentContainer
   *  @Param {Instance} child
   */
  appendChildToContainer(parentContainer, child) {
    parentContainer.appendChild(child);
  },

  /** Remove a child from a parent container.
   *
   *  @param {Container} parentContainer
   *  @Param {Instance} child
   */
  removeChildFromContainer(parentContainer, child) {
    console.log('noop got hit');
    // parentContainer.removeChild(child);
  },

}, MethodTracer);

export default ReactReconciler(HostConfig);
