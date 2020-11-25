import SyntheticEvents,
       { SyntheticMouseEvent,
         SyntheticKeyboardEvent,
         SyntheticTouchEvent } from './SyntheticEvents'
import { macroPropertyGetters } from './MacroProperties';

/* global __BlueprintNative__:false */

//TODO: Keep this union or introduce a common base class ViewInstanceBase?
export type Instance = ViewInstance | RawTextViewInstance;

let __rootViewInstance: ViewInstance | null = null;
let __viewRegistry: Map<string, Instance> = new Map<string, Instance>();
let __lastMouseDownViewId: string | null = null;

if (typeof window !== 'undefined') {
  // This is just a little shim so that I can build for web and run my renderer
  // in the browser, which can be helpful for debugging my renderer implementation.

  //@ts-ignore
  window.__BlueprintNative__ = {
    appendChild(parent: ViewInstance, child: Instance) {
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

export class ViewInstance {
  private _id: string;
  private _type: string;
  public _children: Instance[];
  public _props:  any = null;
  public _parent: any = null;

  constructor(id: string, type: string, props?: any, parent?: ViewInstance) {
    this._id = id;
    this._type = type;
    this._children = [];
    this._props = props;
    this._parent = parent;
  }

  getViewId(): string {
    return this._id;
  }

  getType(): string {
    return this._type;
  }

  getChildIndex(childInstance: Instance): number {
    for (let i = 0; i < this._children.length; ++i) {
      if (this._children[i] === childInstance) {
        return i;
      }
    }

    return -1;
  }

  appendChild(childInstance: Instance): any {
    childInstance._parent = this;

    this._children.push(childInstance);

    //@ts-ignore
    return __BlueprintNative__.addChild(this._id, childInstance._id, -1);
  }

  insertChild(childInstance: Instance, index: number): any {
    childInstance._parent = this;

    this._children.splice(index, 0, childInstance);

    //@ts-ignore
    return __BlueprintNative__.addChild(this._id, childInstance._id, index);
  }

  removeChild(childInstance: Instance): any {
    const index = this._children.indexOf(childInstance);

    if (index >= 0) {
      this._children.splice(index, 1);

    __viewRegistry.delete(childInstance.getViewId());

      //@ts-ignore
      return __BlueprintNative__.removeChild(this._id, childInstance._id);
    }
  }

  setProperty(propKey: string, value: any): any {
    this._props = Object.assign({}, this._props, {
      [propKey]: value,
    });

    // Our React Ref equivalent. This is needed
    // as it appears the 'ref' prop isn't passed through
    // to our renderer's setProperty from the reconciler.
    if (propKey === 'viewRef') {
      value.current = this;
      return;
    }

    if (macroPropertyGetters.hasOwnProperty(propKey)) {
      for (const [k, v] of macroPropertyGetters[propKey](value))
        //@ts-ignore
        __BlueprintNative__.setViewProperty(this._id, k, v);
      return;
    }

    //@ts-ignore
    return __BlueprintNative__.setViewProperty(this._id, propKey, value);
  }

  contains(node: Instance): boolean {
    if (node === this) {
      return true;
    }

    for (let i = 0; i < this._children.length; ++i) {
      const child = this._children[i];

      // A ViewInstance may hold RawTextViewInstances but a
      // RawTextViewInstance contains no children.
      if (child instanceof ViewInstance && child.contains(node))
        return true;
    }

    return false;
  }
}

export class RawTextViewInstance {
  private _id: string;
  private _text: string;
  public  _parent: ViewInstance;

  constructor(id: string, text: string, parent: ViewInstance) {
    this._id     = id;
    this._text   = text;
    this._parent = parent
  }

  getViewId(): string {
    return this._id;
  }

  getText() {
    return this._text;
  }

  setTextValue(text: string): any {
    this._text = text;
    //@ts-ignore
    return __BlueprintNative__.setRawTextValue(this._id, text);
  }
}

function __getRootContainer(): ViewInstance {
  if (__rootViewInstance !== null)
    return __rootViewInstance;

  //@ts-ignore
  const id = __BlueprintNative__.getRootInstanceId();
  __rootViewInstance = new ViewInstance(id, 'View');

  return __rootViewInstance;
}

function __hasFunctionProp(view: ViewInstance, prop: string) {
   return view._props.hasOwnProperty(prop) &&
          typeof view._props[prop] === 'function';
}

function __callEventHandlerIfPresent(view: Instance, eventType: string, event: any) {
  if (view instanceof ViewInstance && __hasFunctionProp(view, eventType)) {
    view._props[eventType](event);
  }
}

function __bubbleEvent(view: Instance, eventType: string, event: any): void {
  if (view && view !== __getRootContainer()) {
    // Always call the event callback on the target before bubbling.
    // Some events may not bubble or have bubble defined. i.e. onMeasure
    __callEventHandlerIfPresent(view, eventType, event);

    if (event.bubbles)
      __bubbleEvent(view._parent, eventType, event);
  }
}

//@ts-ignore
__BlueprintNative__.dispatchViewEvent = function dispatchEvent(viewId: string, eventType: string, event: any) {
  if (__viewRegistry.hasOwnProperty(viewId)) {
    const instance = __viewRegistry[viewId];

    // Convert target/relatedTarget to concrete ViewInstance refs
    if (event.target && __viewRegistry.hasOwnProperty(event.target)) {
      event.target = __viewRegistry[event.target];
    }

    if (event.relatedTarget && __viewRegistry.hasOwnProperty(event.relatedTarget)) {
      event.relatedTarget = __viewRegistry[event.relatedTarget];
    }

    // Convert native event object into it's SyntheticEvent equivalent if required.
    if (SyntheticEvents.isMouseEventHandler(eventType))
      event = new SyntheticMouseEvent(event);
    else if (SyntheticEvents.isKeyboardEventHandler(eventType))
      event = new SyntheticKeyboardEvent(event);
    else if (SyntheticEvents.isTouchEventHandler(eventType))
      event = new SyntheticTouchEvent(event);

    // If mouseDown event we store the target viewId as the last view
    // to recieve a mouseDown for "onClick" book-keeping.
    if (eventType === "onMouseDown") {
      __lastMouseDownViewId = viewId;
      __bubbleEvent(instance, eventType, event);
      return;
    }

    if (eventType === "onMouseUp") {
      __bubbleEvent(instance, eventType, event);

      if (__lastMouseDownViewId && viewId === __lastMouseDownViewId) {
      	__lastMouseDownViewId = null;
        __bubbleEvent(instance, "onClick", event);
      }
      return;
    }

    __bubbleEvent(instance, eventType, event);
  }
}

export default {
  getRootContainer(): ViewInstance {
    return __getRootContainer();
  },
  createViewInstance(viewType: string, props: any, parentInstance: ViewInstance): ViewInstance {
    //@ts-ignore
    const id = __BlueprintNative__.createViewInstance(viewType);
    const instance = new ViewInstance(id, viewType, props, parentInstance);

    __viewRegistry[id] = instance;
    return instance;
  },
  createTextViewInstance(text: string, parentInstance: ViewInstance) {
    //@ts-ignore
    const id       = __BlueprintNative__.createTextViewInstance(text);
    const instance = new RawTextViewInstance(id, text, parentInstance);

    __viewRegistry[id] = instance;
    return instance;
  },
  resetAfterCommit() {
    //@ts-ignore
    return __BlueprintNative__.resetAfterCommit();
  },
};
