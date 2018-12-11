/* global __BlueprintNative__:false */

let __rootViewInstance = null;

if (typeof window !== 'undefined') {
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
    return __BlueprintNative__.appendChild(this._id, childInstance._id);
  }
}

export default {

  getRootContainer() {
    if (__rootViewInstance !== null)
      return __rootViewInstance;

    const id = __BlueprintNative__.getRootInstance();
    __rootViewInstance = new ViewInstance(id, 'View');

    return __rootViewInstance;
  },

  createViewInstance(viewType, props, parentInstance) {
    const id = __BlueprintNative__.createViewInstance(viewType);
    return new ViewInstance(id, viewType);
  }

};
