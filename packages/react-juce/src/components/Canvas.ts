import React, { Component } from "react";
import Colors from "../lib/MacroProperties/Colors";

// TODO: Need to explicitly bind this to members?
export class CanvasRenderingContext {
  private _drawCommands: any[];

  constructor() {
    this._drawCommands = [];
  }

  reset() {
    this._drawCommands = [];
  }

  getDrawCommands(): any[] {
    return this._drawCommands;
  }

  //================================================================================
  // Properties
  // TODO: Support fillStyle/strokeStyle pattern.
  // TODO: Support fillStyle/strokeStyle gradient.
  set fillStyle(value: string) {
    value = Colors.colorStringToAlphaHex(value);
    this._drawCommands.push(["setFillStyle", value]);
  }

  set strokeStyle(value: string) {
    value = Colors.colorStringToAlphaHex(value);
    this._drawCommands.push(["setStrokeStyle", value]);
  }

  set lineWidth(value: number) {
    this._drawCommands.push(["setLineWidth", value]);
  }

  set font(value: string) {
    this._drawCommands.push(["setFont", value]);
  }

  set textAlign(value: string) {
    this._drawCommands.push(["setTextAlign", value]);
  }

  //================================================================================
  // Rect functions
  fillRect(x: number, y: number, width: number, height: number): void {
    this._drawCommands.push(["fillRect", x, y, width, height]);
  }

  strokeRect(x: number, y: number, width: number, height: number): void {
    this._drawCommands.push(["strokeRect", x, y, width, height]);
  }

  strokeRoundedRect(
    x: number,
    y: number,
    width: number,
    height: number,
    cornerSize: number
  ): void {
    this._drawCommands.push([
      "strokeRoundedRect",
      x,
      y,
      width,
      height,
      cornerSize,
    ]);
  }

  fillRoundedRect(
    x: number,
    y: number,
    width: number,
    height: number,
    cornerSize: number
  ): void {
    this._drawCommands.push([
      "fillRoundedRect",
      x,
      y,
      width,
      height,
      cornerSize,
    ]);
  }

  clearRect(x: number, y: number, width: number, height: number): void {
    this._drawCommands.push(["clearRect", x, y, width, height]);
  }

  //================================================================================
  // Path functions
  //
  // TODO: Should we split things out into CanvasRenderingContextPath
  //       so things like close()/stroke() are more obvious when looking at API.
  //       If you look at the ts type definitions for CanvasRenderingContext2D
  //       CanvasRenderingContext2D is composed of other objects like CanvasRenderingContextPath
  //       which contains all path methods. What is the best way to do this in JS and share
  //       a drawCommands instance?
  beginPath(): void {
    this._drawCommands.push(["beginPath"]);
  }

  lineTo(x: number, y: number): void {
    this._drawCommands.push(["lineTo", x, y]);
  }

  moveTo(x: number, y: number): void {
    this._drawCommands.push(["moveTo", x, y]);
  }

  arc(
    x: number,
    y: number,
    radius: number,
    startAngle: number,
    endAngle: number
  ): void {
    //TODO: Add support for optional antiClockWise?: boolean arg
    this._drawCommands.push(["arc", x, y, radius, startAngle, endAngle]);
  }

  quadraticCurveTo(cpx: number, cpy: number, x: number, y: number): void {
    this._drawCommands.push(["quadraticCurveTo", cpx, cpy, x, y]);
  }

  closePath(): void {
    this._drawCommands.push(["close"]);
  }

  stroke(): void {
    this._drawCommands.push(["stroke"]);
  }

  fill(): void {
    this._drawCommands.push(["fill"]);
  }

  //================================================================================
  // Transform functions
  rotate(angle: number): void {
    this._drawCommands.push(["rotate", angle]);
  }

  translate(x: number, y: number): void {
    this._drawCommands.push(["translate", x, y]);
  }

  setTransform(
    a: number,
    b: number,
    c: number,
    d: number,
    e: number,
    f: number
  ): void {
    this._drawCommands.push(["setTransform", a, b, c, d, e, f]);
  }

  resetTransform(): void {
    this._drawCommands.push(["resetTransform"]);
  }

  //================================================================================
  // Image functions
  //
  //TODO: Add support for other drawImage overloads.
  //      Currently only support SVG string. What is correct
  //      type to use here?
  drawImage(image: string, dx: number, dy: number): void {
    this._drawCommands.push(["drawImage", image, dx, dy]);
  }

  //================================================================================
  // Text functions
  strokeText(text: string, x: number, y: number, maxWidth?: number): void {
    if (maxWidth === undefined)
      this._drawCommands.push(["strokeText", text, x, y]);
    else this._drawCommands.push(["strokeText", text, x, y, maxWidth]);
  }

  fillText(text: string, x: number, y: number, maxWidth?: number): void {
    if (maxWidth === undefined)
      this._drawCommands.push(["fillText", text, x, y]);
    else this._drawCommands.push(["fillText", text, x, y, maxWidth]);
  }

  //================================================================================
}

export interface CanvasProps {
  onDraw: (ctx: CanvasRenderingContext) => void;
  onMeasure?: (e: any) => void;
  stateful?: boolean;
}

interface CanvasState {
  width: number;
  height: number;
}

export class Canvas extends Component<CanvasProps, CanvasState> {
  private _ctx: CanvasRenderingContext;

  constructor(props: CanvasProps) {
    super(props);

    this._ctx = new CanvasRenderingContext();
    this._onMeasure = this._onMeasure.bind(this);
    this._onDraw = this._onDraw.bind(this);

    this.state = {
      width: 0,
      height: 0,
    };
  }

  _onMeasure(e: any) {
    this.setState({
      width: e.width,
      height: e.height,
    });

    if (typeof this.props.onMeasure === "function") {
      this.props.onMeasure(e);
    }
  }

  _onDraw(): any[] {
    if (typeof this.props.onDraw === "function") {
      this._ctx.reset();

      this.props.onDraw(this._ctx);
      return this._ctx.getDrawCommands();
    }

    return [];
  }

  render() {
    return React.createElement(
      "CanvasView",
      Object.assign({}, this.props, {
        onDraw: () => {
          return this._onDraw();
        },
        onMeasure: (e: any) => {
          this._onMeasure(e);
        },
      }),
      this.props.children
    );
  }
}
