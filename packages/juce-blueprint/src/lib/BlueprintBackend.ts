import SyntheticEvents, {
    SyntheticEvent,
    SyntheticMouseEvent,
    SyntheticKeyboardEvent
} from './SyntheticEvents'

/* global __BlueprintNative__:false */

type Instance = ViewInstance | RawTextViewInstance;

let __rootViewInstance: ViewInstance | null    = null;
let __viewRegistry: Map<string, Instance>      = new Map<string, Instance>();
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
    return __BlueprintNative__.addChild(this._id, childInstance._id);
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

    //@ts-ignore
    return __BlueprintNative__.setViewProperty(this._id, propKey, value);
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

function __lastMouseDownInstanceHandlesMouseUp(instance: Instance): boolean {
  while (instance && instance !== __getRootContainer()) {
    if (instance instanceof ViewInstance && __hasFunctionProp(instance, 'onMouseDrag'))
      return true
    instance = instance._parent;
  }
  return false;
}

//TODO: Consider more performant LCA algorithms.
//      Could try something like: https://www.geeksforgeeks.org/lca-n-ary-tree-constant-query-o1
//      though this would require a level of book-keeping in appendChild, removeChild etc.
//      Lets just get the behaviour correct and tested first.
function __getLCA(root: Instance, lhs: Instance, rhs: Instance): Instance | null {
  if (root == null)
    return null;

  // If lhs and rhs are the same component self is the lowest common ancestor.
  if (lhs.getViewId() === rhs.getViewId())
    return lhs;

  if (root.getViewId() === lhs.getViewId() || root.getViewId() === rhs.getViewId())
   return root;

  let i = 0;
  let lastLCA: Instance | null = null;

    // RawTextViewInstance does not hold a child array so
    // treat it as empty here.
  if (!(root instanceof RawTextViewInstance)) {
    root._children.map((v: Instance) => {
      lastLCA = __getLCA(v, lhs, rhs);
      if (lastLCA) {
        i++;
      }
      if (i >= 2) {
        return root;
      }
    });
  }

    return lastLCA;
}

//@ts-ignore
__BlueprintNative__.dispatchViewEvent = function dispatchEvent(viewId: string, eventType: string, event: any) {
  if (__viewRegistry.hasOwnProperty(viewId)) {
    const instance = __viewRegistry[viewId];

    // Convert native event object into it's SyntheticEvent equivalent if required.
    if (SyntheticEvents.isMouseEventHandler(eventType))
      event = new SyntheticMouseEvent(event);
    else if (SyntheticEvents.isKeyboardEventHandler(eventType))
      event = new SyntheticKeyboardEvent(event);

    // If mouseDown event we store the target viewId as the last view
    // to recieve a mouseDown for "onClick" book-keeping.
    if (eventType === "onMouseDown") {
      __lastMouseDownViewId = viewId;
      __bubbleEvent(instance, eventType, event);
      return;
    }

    if (eventType === "onMouseUp" && __lastMouseDownViewId !== null) {
      const lastMouseDownInstance = __viewRegistry[__lastMouseDownViewId];
      __lastMouseDownViewId = null;

      const ancestor = __getLCA(__getRootContainer(), lastMouseDownInstance, instance);

      if (__lastMouseDownInstanceHandlesMouseUp(lastMouseDownInstance))
        __bubbleEvent(lastMouseDownInstance, eventType, event);
      else
        __bubbleEvent(instance, eventType, event);

      // If mouseUp occurs inside a different component to the last
      // mouseDown we must ensure that onClick is raised on the
      // lowest common ancestor of the mouseDown and mouseUp components.
      // See: https://developer.mozilla.org/en-US/docs/Web/API/Element/click_event
      if (ancestor !== null)
        __bubbleEvent(ancestor, "onClick", event);

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
  }
};
