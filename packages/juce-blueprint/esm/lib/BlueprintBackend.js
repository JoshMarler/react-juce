/* global __BlueprintNative__:false */

let __rootViewInstance = null;

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

function isMouseEventHandler(key) {
  const k = key.toLowerCase();

  return k === 'onmousedown' ||
    k == 'onmouseup' ||
    k == 'onmousedrag' ||
    k == 'onmouseclick';
}

function isKeyboardEventHandler(key) {
  const k = key.toLowerCase();

  return k === 'onkeydown' ||
    k == 'onkeyup' ||
    k == 'onkeypress';
}

class SyntheticEvent {
  constructor(props) {
    this.bubbles = true;
    this.defaultPrevented = false;

    this._internal = props;
  }

  stopPropagation() {
    this.bubbles = false;
  }

  preventDefault() {
    this.defaultPrevented = true;
  }
}

class SyntheticMouseEvent extends SyntheticEvent {
  constructor(props) {
    super(props);

    this.x = this.clientX = props.x;
    this.y = this.clientY = props.y;
    this.screenX = props.screenX;
    this.screenY = props.screenY;
  }
}

class SyntheticKeyboardEvent extends SyntheticEvent {
  constructor(props) {
    super(props);

    this.keyCode = props.keyCode;
    this.key = props.key;
  }
}

function noop() {}

class ViewInstance {
  constructor(id, type, props, parent) {
    this._id = id;
    this._type = type;
    this._children = [];
    this._props = props;
    this._parent = parent;

    this.setProperty('onMouseDown', noop);
    this.setProperty('onMouseUp', noop);
    this.setProperty('onMouseDrag', noop);
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
    childInstance._parent = this;

    this._children.push(childInstance);
    return __BlueprintNative__.addChild(this._id, childInstance._id);
  }

  insertChild(childInstance, index) {
    childInstance._parent = this;

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

    if (isMouseEventHandler(propKey)) {
      return __BlueprintNative__.setViewProperty(this._id, propKey, (evt) => {
        this.bubbleViewEvent(propKey, new SyntheticMouseEvent(evt));
      });
    }

    if (isKeyboardEventHandler(propKey)) {
      return __BlueprintNative__.setViewProperty(this._id, propKey, (evt) => {
        this.bubbleViewEvent(propKey, new SyntheticKeyboardEvent(evt));
      });
    }

    return __BlueprintNative__.setViewProperty(this._id, propKey, value);
  }

  bubbleViewEvent(propKey, evt) {
    let instance = this;

    while (instance && evt.bubbles) {
      const hasHandler = instance._props.hasOwnProperty(propKey) &&
        typeof instance._props[propKey] === 'function';

      if (hasHandler) {
        instance._props[propKey](evt);
      }

      instance = instance._parent;
    }
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
    const instance = new ViewInstance(id, viewType, props, parentInstance);
    return instance;
  },

  createTextViewInstance(text) {
    const id = __BlueprintNative__.createTextViewInstance(text);
    return new RawTextViewInstance(id, text);
  },

};
