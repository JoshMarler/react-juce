import { all as allCssProps } from "known-css-properties";
import camelCase from "camelcase";
import NativeMethods from "./NativeMethods";
import SyntheticEvents, {
  SyntheticMouseEvent,
  SyntheticKeyboardEvent,
} from "./SyntheticEvents";
import { macroPropertyGetters } from "./MacroProperties";
import Colors from "./MacroProperties/Colors";

//TODO: Keep this union or introduce a common base class ViewInstanceBase?
export type Instance = ViewInstance | RawTextViewInstance;

let __rootViewInstance: ViewInstance | null = null;
let __viewRegistry: Map<string, Instance> = new Map<string, Instance>();
let __lastMouseDownViewId: string | null = null;

// get any css properties not beginning with a "-",
// and build a map from any camelCase versions to
// the hyphenated version
const cssPropsMap = allCssProps
  .filter((s) => !s.startsWith("-") && s.includes("-"))
  .reduce((acc, v) => Object.assign(acc, { [camelCase(v)]: v }), {});

export class ViewInstance {
  private _id: string;
  private _type: string;
  public _children: Instance[];
  public _props: any = null;
  public _parent: any = null;

  constructor(id: string, type: string, props?: any, parent?: ViewInstance) {
    this._id = id;
    this._type = type;
    this._children = [];
    this._props = props;
    this._parent = parent;

    //TODO: This has been added to resolve a bug in
    //      our Button component when calling contains()
    //      on a viewRef. This is a result of wrapping our
    //      viewRefs in a Proxy object which means this is
    //      no longer bound to the original ViewInstance object
    //      during the contains call. Ideally we would use Reflect.get()
    //      here but Duktape does not fully support Reflect at the moment.
    this.contains = this.contains.bind(this);
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
    return NativeMethods.insertChild(this._id, childInstance._id, -1);
  }

  insertChild(childInstance: Instance, index: number): any {
    childInstance._parent = this;

    this._children.splice(index, 0, childInstance);

    //@ts-ignore
    return NativeMethods.insertChild(this._id, childInstance._id, index);
  }

  removeChild(childInstance: Instance): any {
    const index = this._children.indexOf(childInstance);

    if (index >= 0) {
      this._children.splice(index, 1);

      __viewRegistry.delete(childInstance.getViewId());

      //@ts-ignore
      return NativeMethods.removeChild(this._id, childInstance._id);
    }
  }

  setProperty(propKey: string, value: any): any {
    // if the supplied propkey is a camelCase equivalent
    // of a css prop, first convert it to kebab-case
    propKey = cssPropsMap[propKey] || propKey;

    // convert provided color string to alpha-hex code for JUCE
    let nativeValue;
    if (Colors.isColorProperty(propKey)) {
      value = Colors.colorStringToAlphaHex(value);
      if (value.startsWith("linear-gradient")) {
        nativeValue = Colors.convertLinearGradientStringToNativeObject(value);
      }
    }

    this._props = Object.assign({}, this._props, {
      [propKey]: value,
    });

    // Our React Ref equivalent. This is needed
    // as it appears the 'ref' prop isn't passed through
    // to our renderer's setProperty from the reconciler.
    // We wrap the ViewInstance in a proxy object here to allow
    // invocation of native ViewInstance methods via React refs.
    // If a property is not present on the ViewInstance object
    // we assume the caller is attempting to access/invoke a
    // native View method.
    if (propKey === "viewRef") {
      value.current = new Proxy(this, {
        get: function (target, prop, receiver) {
          if (prop in target) {
            return target[prop];
          }

          return function (...args) {
            //@ts-ignore
            return NativeMethods.invokeViewMethod(target._id, prop, ...args);
          };
        },
      });

      return;
    }

    if (macroPropertyGetters.hasOwnProperty(propKey)) {
      //@ts-ignore
      for (const [k, v] of macroPropertyGetters[propKey](value))
        NativeMethods.setViewProperty(this._id, k, v);
      return;
    }
    //@ts-ignore
    return NativeMethods.setViewProperty(
      this._id,
      propKey,
      nativeValue ? nativeValue : value
    );
  }

  contains(node: Instance): boolean {
    if (node === this) {
      return true;
    }

    for (let i = 0; i < this._children.length; ++i) {
      const child = this._children[i];

      // A ViewInstance may hold RawTextViewInstances but a
      // RawTextViewInstance contains no children.
      if (child instanceof ViewInstance && child.contains(node)) return true;
    }

    return false;
  }
}

export class RawTextViewInstance {
  private _id: string;
  private _text: string;
  public _parent: ViewInstance;

  constructor(id: string, text: string, parent: ViewInstance) {
    this._id = id;
    this._text = text;
    this._parent = parent;
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
    return NativeMethods.setRawTextValue(this._id, text);
  }
}

function __getRootContainer(): ViewInstance {
  if (__rootViewInstance !== null) return __rootViewInstance;

  //@ts-ignore
  const id = NativeMethods.getRootInstanceId();
  __rootViewInstance = new ViewInstance(id, "View");

  return __rootViewInstance;
}

function __hasFunctionProp(view: ViewInstance, prop: string) {
  return (
    view._props.hasOwnProperty(prop) && typeof view._props[prop] === "function"
  );
}

function __callEventHandlerIfPresent(
  view: Instance,
  eventType: string,
  event: any
) {
  if (view instanceof ViewInstance && __hasFunctionProp(view, eventType)) {
    view._props[eventType](event);
  }
}

function __bubbleEvent(view: Instance, eventType: string, event: any): void {
  if (view && view !== __getRootContainer()) {
    // Always call the event callback on the target before bubbling.
    // Some events may not bubble or have bubble defined. i.e. onMeasure
    __callEventHandlerIfPresent(view, eventType, event);

    if (event.bubbles) __bubbleEvent(view._parent, eventType, event);
  }
}

//@ts-ignore
NativeMethods.dispatchViewEvent = function dispatchEvent(
  viewId: string,
  eventType: string,
  event: any
) {
  if (__viewRegistry.hasOwnProperty(viewId)) {
    const instance = __viewRegistry[viewId];

    // Convert target/relatedTarget to concrete ViewInstance refs
    if (event.target && __viewRegistry.hasOwnProperty(event.target)) {
      event.target = __viewRegistry[event.target];
    }

    if (
      event.relatedTarget &&
      __viewRegistry.hasOwnProperty(event.relatedTarget)
    ) {
      event.relatedTarget = __viewRegistry[event.relatedTarget];
    }

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
};

export default {
  getRootContainer(): ViewInstance {
    return __getRootContainer();
  },
  createViewInstance(
    viewType: string,
    props: any,
    parentInstance: ViewInstance
  ): ViewInstance {
    //@ts-ignore
    const id = NativeMethods.createViewInstance(viewType);
    const instance = new ViewInstance(id, viewType, props, parentInstance);

    __viewRegistry[id] = instance;
    return instance;
  },
  createTextViewInstance(text: string, parentInstance: ViewInstance) {
    //@ts-ignore
    const id = NativeMethods.createTextViewInstance(text);
    const instance = new RawTextViewInstance(id, text, parentInstance);

    __viewRegistry[id] = instance;
    return instance;
  },
  resetAfterCommit() {
    //@ts-ignore
    return NativeMethods.resetAfterCommit();
  },
};
