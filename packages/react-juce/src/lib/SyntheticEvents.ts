import { Instance } from "./BlueprintBackend";

export class SyntheticEvent {
  public  bubbles: boolean;
  public  defaultPrevented: boolean;
  public  target: Instance;
  private _internal: any;

  constructor(props: any) {
    this.bubbles = true;
    this.defaultPrevented = false;

    const { target, ...other } = props;
    this.target = target;

    this._internal = other;
  }

  stopPropagation(): void {
    this.bubbles = false;
  }

  preventDefault(): void {
    this.defaultPrevented = true;
  }
}

export class Touch {
  public identifier: number;
  public x: number;
  public y: number;
  public screenX: number;
  public screenY: number;
  public target: Instance;

  constructor(props: any) {
    this.identifier = props.identifier;
    this.x = props.x;
    this.y = props.y;
    this.screenX = props.screenX;
    this.screenY = props.screenY;
    this.target = props.target;
  }
}

export class SyntheticMouseEvent extends SyntheticEvent {
  public x: number;
  public y: number;
  public clientX: number;
  public clientY: number;
  public screenX: number;
  public screenY: number;
  public relatedTarget: Instance;

  constructor(props: any) {
    super(props);

    this.x = this.clientX = props.x;
    this.y = this.clientY = props.y;
    this.screenX = props.screenX;
    this.screenY = props.screenY;
    this.relatedTarget = props.relatedTarget;
  }
}

export class SyntheticTouchEvent extends SyntheticEvent {
  public changedTouches: Touch[];
  public targetTouches: Touch[];
  public touches: Touch[];

  constructor(props: any) {
    super(props);

    this.changedTouches = props.changedTouches ? props.changedTouches.map((touch) => new Touch(touch)) : [];
    this.targetTouches = props.targetTouches ? props.targetTouches.map((touch) => new Touch(touch)) : [];
    this.touches = props.touches ? props.touches.map((touch) => new Touch(touch)) : [];
  }
}

export class SyntheticKeyboardEvent extends SyntheticEvent {
  public keyCode: number;
  public key: string;

  constructor(props: any) {
    super(props);

    this.keyCode = props.keyCode;
    this.key = props.key;
  }
}

export default {
  isMouseEventHandler(key: string): boolean {
    const k = key.toLowerCase();

    return k === 'onmousedown' ||
      k == 'onmouseup' ||
      k == 'onmousedrag' ||
      k == 'onmouseclick';
  },

  isKeyboardEventHandler(key: string): boolean {
    const k = key.toLowerCase();

    return k === 'onkeydown' ||
      k == 'onkeyup' ||
      k == 'onkeypress';
  },

  isTouchEventHandler(key: string): boolean {
    const k = key.toLowerCase();

    return k === 'ontouchcancel' ||
      k == 'ontouchend' ||
      k == 'ontouchmove' ||
      k == 'ontouchstart';
  },
}

