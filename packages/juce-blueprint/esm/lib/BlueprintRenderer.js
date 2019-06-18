import MethodTracer from './MethodTracer';
import ReactReconciler from 'react-reconciler';
import BlueprintBackend from './BlueprintBackend';

import invariant from 'invariant';


const HostConfig = {
  /** Time provider. */
  now: Date.now,

  /** Indicates to the reconciler that our DOM tree supports mutating operations
   *  like appendChild, removeChild, etc.
   */
  supportsMutation: true,

  /** Provides the context for rendering the root level element.
   *
   *  Really only using this and `getChildHostContext` for enforcing nesting
   *  constraints, such as that raw text content must be a child of a <Text>
   *  element.
   *
   *  @param {Container} rootContainerInstance
   */
  getRootHostContext(rootContainerInstance) {
    return {
      isInTextParent: false,
    };
  },

  /** Provides the context for rendering a child element.
   *
   *  @param {Object} parentHostContext
   *  @param {String} elementType
   *  @param {Container} rootContainerInstance
   */
  getChildHostContext(parentHostContext, elementType, rootContainerInstance) {
    const isInTextParent = parentHostContext.isInTextParent ||
      elementType === 'Text';

    return {isInTextParent};
  },

  prepareForCommit: (...args) => {},
  resetAfterCommit: (...args) => {},

  /** Called to determine whether or not a new text value can be set on an
   *  existing node, or if a new text node needs to be created.
   *
   *  This is essentially born from the fact in that in a Web DOM, there are certain
   *  nodes, such as <textarea>, that support a `textContent` property. Setting the
   *  node's `textContent` property is different from creating a TextNode and appending
   *  it to the node's children array. This method signals which option to take.
   *
   *  In our case, we return `false` always because we have no nodes in the JUCE
   *  backend that support this kind of behavior. All text nodes must be created as
   *  RawTextViewInstances as children of a TextViewInstance.
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
    invariant(
      !hostContext.isInTextParent,
      'Nesting elements inside of <Text> is currently not supported.'
    );

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
    invariant(
      hostContext.isInTextParent,
      'Raw text strings must be rendered within a <Text> element.'
    );

    return BlueprintBackend.createTextViewInstance(text);
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
    if (typeof newText === 'string' && oldText !== newText) {
      instance.setTextValue(newText);
    }
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

  /** Inserts a child node into a parent's children array, just before the
   *  second given child node.
   *
   *  @param {Instance} parentInstance
   *  @Param {Instance} child
   *  @Param {Instance} beforeChild
   */
  insertBefore(parentInstance, child, beforeChild) {
    console.log('TODO');
  },

  /** Remove a child from a parent instance.
   *
   *  @param {Instance} parentInstance
   *  @Param {Instance} child
   */
  removeChild(parentInstance, child) {
    parentInstance.removeChild(child);
  },

  /** Remove a child from a parent container.
   *
   *  @param {Container} parentContainer
   *  @Param {Instance} child
   */
  removeChildFromContainer(parentContainer, child) {
    parentContainer.removeChild(child);
  },

};

export default ReactReconciler(HostConfig);
export const BlueprintTracedRenderer = ReactReconciler(new Proxy(HostConfig, MethodTracer));
