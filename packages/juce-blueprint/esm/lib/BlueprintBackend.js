/* global __BlueprintNative__:false */

let __rootViewInstance = null;
let __viewRegistry = {};

if (typeof window !== 'undefined') {
  // This is just a little shim so that I can build for web and run my renderer
  // in the browser, which can be helpful for debugging my renderer implementation.
  window.__BlueprintNative__ = {
    appendChild(parent, child) {
      // noop
    },
    getRootInstanceId() {
      return 'rootinstanceid';
    },
    createViewInstance() {
      return 'someviewinstanceid';
    },
    createTextViewInstance() {
      return 'sometextviewinstanceid';
    },
    setViewProperty() {
      // Noop
    },
  };
}

class ViewInstance {
  constructor(id, type, props) {
    this._id = id;
    this._type = type;
    this._children = [];
    this._props = props;
  }

  getChildIndex(childInstance) {
    for (let i = 0; i < this._children.length; ++i) {
      if (this._children[i] === childInstance) {
        return i;
      }
    }

    return -1;
  }

  appendChild(childInstance) {
    this._children.push(childInstance);
    return __BlueprintNative__.addChild(this._id, childInstance._id);
  }

  insertChild(childInstance, index) {
    this._children.splice(index, 0, childInstance);
    return __BlueprintNative__.addChild(this._id, childInstance._id, index);
  }

  removeChild(childInstance) {
    const index = this._children.indexOf(childInstance);

    if (index >= 0) {
      this._children.splice(index, 1);
      return __BlueprintNative__.removeChild(this._id, childInstance._id);
    }
  }

  setProperty(propKey, value) {
    this._props = Object.assign({}, this._props, {
      [propKey]: value,
    });

    if (typeof value === 'number' || typeof value === 'string' || typeof value === 'boolean')
      return __BlueprintNative__.setViewProperty(this._id, propKey, value);

    return void 0;
  }
}

class RawTextViewInstance {
  constructor(id, text) {
    this._id = id;
    this._text = text;
  }

  setTextValue(text) {
    this._text = text;
    return __BlueprintNative__.setRawTextValue(this._id, text);
  }
}

__BlueprintNative__.dispatchViewEvent = function dispatchEvent(viewId, eventType, ...args) {
  if (__viewRegistry.hasOwnProperty(viewId)) {
    let instance = __viewRegistry[viewId];
    let eventHandler = instance._props[`on${eventType}`];

    // TODO: Could do manual event bubbling here. Form an "event" object, give it to the
    // handler, and then walk up the parent chain giving the same object to every parent
    // callback. Would require that ViewInstance carry pointers to parents but that should
    // be trivial...
    if (typeof eventHandler === 'function') {
      eventHandler.call(null, ...args);
    }
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
    const instance = new ViewInstance(id, viewType, props);

    __viewRegistry[id] = instance;
    return instance;
  },

  createTextViewInstance(text) {
    const id = __BlueprintNative__.createTextViewInstance(text);
    return new RawTextViewInstance(id, text);
  },

};
