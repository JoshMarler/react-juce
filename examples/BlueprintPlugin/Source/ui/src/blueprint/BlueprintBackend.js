/* global __BlueprintNative__:false */

let __rootViewInstance = null;

if (typeof window !== 'undefined') {
  // This is just a little shim so that I can build for web and run my renderer
  // in the browser, which can be helpful for debugging my renderer implementation.
  window.__BlueprintNative__ = {
    appendChild(parent, child) {
      // noop
    },
    getRootInstance() {
      return 'rootinstanceid';
    },
    createViewInstance() {
      return 'someviewinstanceid';
    },
  };
}

class ViewInstance {
  constructor(id, type) {
    this._id = id;
    this._type = type;
    this._children = [];
  }

  appendChild(childInstance) {
    this._children.push(childInstance);
    return __BlueprintNative__.appendChild(this._id, childInstance._id);
  }

  setProperty(propKey, value) {
    return __BlueprintNative__.setViewProperty(this._id, propKey, value);
  }
}

export default {

  getRootContainer() {
    if (__rootViewInstance !== null)
      return __rootViewInstance;

    const id = __BlueprintNative__.getRootInstanceId();
    __rootViewInstance = new ViewInstance(id, 'View');

    return __rootViewInstance;
  },

  createViewInstance(viewType, props, parentInstance) {
    const id = __BlueprintNative__.createViewInstance(viewType);
    return new ViewInstance(id, viewType);
  }

};
