import SyntheticEvents, 
       { SyntheticEvent, 
         SyntheticMouseEvent, 
         SyntheticKeyboardEvent } from './SyntheticEvents' 

/* global __BlueprintNative__:false */

let __rootViewInstance: ViewInstance | null = null;

if (typeof window !== 'undefined') {
  // This is just a little shim so that I can build for web and run my renderer
  // in the browser, which can be helpful for debugging my renderer implementation.

  //@ts-ignore
  window.__BlueprintNative__ = {
    appendChild(parent: ViewInstance, child: ViewInstance) {
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

function noop(): void {}

export class ViewInstance {
  private _id: string;
  private _type: string;
  private _children: ViewInstance[];
  private _props: any;
  private _parent: any;

  constructor(id: string, type: string, props?: any, parent?: ViewInstance) {
    this._id = id;
    this._type = type;
    this._children = [];
    this._props = props;
    this._parent = parent;

    this.setProperty('onMouseDown', noop);
    this.setProperty('onMouseUp', noop);
    this.setProperty('onMouseDrag', noop);
  }

  getChildIndex(childInstance: ViewInstance): number {
    for (let i = 0; i < this._children.length; ++i) {
      if (this._children[i] === childInstance) {
        return i;
      }
    }

    return -1;
  }

  appendChild(childInstance: ViewInstance): any {
    childInstance._parent = this;

    this._children.push(childInstance);

    //@ts-ignore
    return __BlueprintNative__.addChild(this._id, childInstance._id);
  }

  insertChild(childInstance: ViewInstance, index: number): any {
    childInstance._parent = this;

    this._children.splice(index, 0, childInstance);

    //@ts-ignore
    return __BlueprintNative__.addChild(this._id, childInstance._id, index);
  }

  removeChild(childInstance: ViewInstance): any {
    const index = this._children.indexOf(childInstance);

    if (index >= 0) {
      this._children.splice(index, 1);

      //@ts-ignore
      return __BlueprintNative__.removeChild(this._id, childInstance._id);
    }
  }

  setProperty(propKey: string, value: any): any {
    this._props = Object.assign({}, this._props, {
      [propKey]: value,
    });

    if (SyntheticEvents.isMouseEventHandler(propKey)) {
      //@ts-ignore
      return __BlueprintNative__.setViewProperty(this._id, propKey, (evt: SyntheticMouseEvent) => {
        this.bubbleViewEvent(propKey, new SyntheticMouseEvent(evt));
      });
    }

    if (SyntheticEvents.isKeyboardEventHandler(propKey)) {
      //@ts-ignore
      return __BlueprintNative__.setViewProperty(this._id, propKey, (evt: SyntheticKeyboardEvent) => {
        this.bubbleViewEvent(propKey, new SyntheticKeyboardEvent(evt));
      });
    }

    //@ts-ignore
    return __BlueprintNative__.setViewProperty(this._id, propKey, value);
  }

  bubbleViewEvent(propKey: string, evt: SyntheticEvent): void {
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

export class RawTextViewInstance {
  private _id: string;
  private _text: string;

  constructor(id: string, text: string) {
    this._id = id;
    this._text = text;
  }

  setTextValue(text: string): any {
    this._text = text;
    //@ts-ignore
    return __BlueprintNative__.setRawTextValue(this._id, text);
  }
}

export default {

  getRootContainer(): ViewInstance {
    if (__rootViewInstance !== null)
      return __rootViewInstance;

    //@ts-ignore
    const id = __BlueprintNative__.getRootInstanceId();
    __rootViewInstance = new ViewInstance(id, 'View');

    return __rootViewInstance;
  },

  createViewInstance(viewType: string, props: any, parentInstance: ViewInstance): ViewInstance {
    //@ts-ignore
    const id = __BlueprintNative__.createViewInstance(viewType);
    const instance = new ViewInstance(id, viewType, props, parentInstance);
    return instance;
  },

  createTextViewInstance(text: string) {
    //@ts-ignore
    const id = __BlueprintNative__.createTextViewInstance(text);
    return new RawTextViewInstance(id, text);
  },

};
