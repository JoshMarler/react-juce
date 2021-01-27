import { Instance } from "./Backend";

export class SyntheticEvent {
  public bubbles: boolean;
  public defaultPrevented: boolean;
  public target: Instance;
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

    return (
      k === "onmousedown" ||
      k == "onmouseup" ||
      k == "onmouseenter" ||
      k == "onmouseleave" ||
      k == "onmousedrag" ||
      k == "onmouseclick"
    );
  },

  isKeyboardEventHandler(key: string): boolean {
    const k = key.toLowerCase();

    return k === "onkeydown" || k == "onkeyup" || k == "onkeypress";
  },
};
