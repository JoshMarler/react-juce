export class SyntheticEvent {
  public  bubbles: boolean;
  public  defaultPrevented: boolean;
  private _internal: any;

  constructor(props: any) {
    this.bubbles = true;
    this.defaultPrevented = false;

    this._internal = props;
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

  constructor(props: any) {
    super(props);

    this.x = this.clientX = props.x;
    this.y = this.clientY = props.y;
    this.screenX = props.screenX;
    this.screenY = props.screenY;
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
  }
}