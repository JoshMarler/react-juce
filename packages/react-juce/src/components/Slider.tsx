import React, { Component, PropsWithChildren } from "react";
import invariant from "invariant";

import { Canvas, CanvasRenderingContext } from "./Canvas";
import { View } from "./View";
import { SyntheticMouseEvent } from "../lib/SyntheticEvents";

// Some simple helpers for slider drag gesture -> value mapping
const _linearHorizontalGestureMap = (
  mouseDownX: number,
  mouseDownY: number,
  sensitivity: number,
  valueAtDragStart: number,
  dragEvent: SyntheticMouseEvent
): number => {
  const dx = dragEvent.x - mouseDownX;
  return Math.max(0.0, Math.min(1.0, valueAtDragStart + dx * sensitivity));
};

const _linearVerticalGestureMap = (
  mouseDownX: number,
  mouseDownY: number,
  sensitivity: number,
  valueAtDragStart: number,
  dragEvent: SyntheticMouseEvent
): number => {
  const dy = dragEvent.y - mouseDownY;
  return Math.max(0.0, Math.min(1.0, valueAtDragStart - dy * sensitivity));
};

const _rotaryGestureMap = (
  mouseDownX: number,
  mouseDownY: number,
  sensitivity: number,
  valueAtDragStart: number,
  dragEvent: SyntheticMouseEvent
): number => {
  const dx = dragEvent.x - mouseDownX;
  const dy = mouseDownY - dragEvent.y;
  return Math.max(
    0.0,
    Math.min(1.0, valueAtDragStart + (dx + dy) * sensitivity)
  );
};

const _drawLinearHorizontalSlider = (trackColor: string, fillColor: string) => {
  return (
    ctx: CanvasRenderingContext,
    width: number,
    height: number,
    value: number
  ): void => {
    const lineWidth = 2.0;

    const x = 0 + lineWidth / 2;
    const y = 0 + lineWidth / 2;

    width = width - lineWidth;
    height = height - lineWidth;

    const fillWidth = value * width;

    ctx.lineWidth = lineWidth;
    ctx.strokeStyle = trackColor;
    ctx.fillStyle = fillColor;

    ctx.fillRect(x, y, fillWidth, height);
    ctx.strokeRect(x, y, width, height);
  };
};

const _drawLinearVerticalSlider = (trackColor: string, fillColor: string) => {
  return (
    ctx: CanvasRenderingContext,
    width: number,
    height: number,
    value: number
  ): void => {
    const lineWidth = 2.0;

    const x = 0 + lineWidth / 2;
    const y = 0 + lineWidth / 2;

    width = width - lineWidth;
    height = height - lineWidth;

    const fillHeight = value * height;
    const fillY = y + (height - fillHeight);

    ctx.lineWidth = lineWidth;
    ctx.strokeStyle = trackColor;
    ctx.fillStyle = fillColor;

    ctx.fillRect(x, fillY, width, fillHeight);
    ctx.strokeRect(x, y, width, height);
  };
};

function _drawArc(
  ctx,
  centerX,
  centerY,
  radius,
  arcSize,
  startAngle,
  endAngle,
  lineWidth
) {
  const deltaX = centerX;
  const deltaY = centerY;

  const rotateAngle = ((180 + 180 * (1 - arcSize)) * Math.PI) / 180;

  ctx.lineWidth = lineWidth;

  ctx.beginPath();
  ctx.moveTo(centerX - lineWidth / 2, 0);
  ctx.arc(
    centerX - lineWidth / 2,
    centerY - lineWidth / 2,
    radius,
    startAngle,
    endAngle
  );

  ctx.translate(deltaX, deltaY);
  ctx.rotate(rotateAngle);
  ctx.translate(-deltaX, -deltaY);

  ctx.stroke();
  ctx.resetTransform();
}

const _drawRotarySlider = (trackColor, fillColor) => {
  return (ctx, width, height, value) => {
    const lineWidth = 3;
    const arcSize = 0.8;

    const radius = Math.min(width, height) * 0.5 - lineWidth / 2;

    const centerX = width / 2;
    const centerY = height / 2;

    const strokeStart = 0;
    const strokeEnd = 2 * Math.PI * arcSize;

    const fillStart = -(2.5 * Math.PI * arcSize);
    const fillEnd = fillStart + value * (2 * Math.PI * arcSize);

    ctx.strokeStyle = trackColor;
    _drawArc(
      ctx,
      centerX,
      centerY,
      radius,
      arcSize,
      strokeStart,
      strokeEnd,
      lineWidth
    );

    ctx.strokeStyle = fillColor;
    _drawArc(
      ctx,
      centerX,
      centerY,
      radius,
      arcSize,
      fillStart,
      fillEnd,
      lineWidth
    );
  };
};

export interface SliderProps {
  value?: number;
  sensitivity?: number;
  onChange?: (value: number) => void;
  onDraw?: (
    ctx: CanvasRenderingContext,
    width: number,
    height: number,
    value: number
  ) => void;
  mapDragGestureToValue?: (
    mouseDownX: number,
    mouseDownY: number,
    sensitivity: number,
    valueAtDragStart: number,
    dragEvent: SyntheticMouseEvent
  ) => number;
}

type SliderState = {
  width: number;
  height: number;
  value: number;
};

/**
 * A generic slider component which can be used as a building block for more complex
 * sliders.
 *
 * Slider takes an onDraw function prop for rendering the Slider using a CanvasRenderingContext.
 * Slider also accepts a mapDragGestureToValue pop which allows customisation when converting mouse drag
 * positions/events to the Slider's normalised value reported via the onChange prop callback.
 *
 * Some default onDraw and mapDragGestureToValue implementations are provided:
 * @see Slider.drawLinearHorizontal
 * @see Slider.drawLinearVertical
 * @see Slider.drawRotary
 *
 * @see Slider.linearHorizontalGestureMap
 * @see Slider.linearVerticalGestureMap
 * @see Slider.rotaryGestureMap
 *
 * @example
 *
 * <Slider
 *  onChange={(value) => console.log("Slider value changed: " + value)}
 *  onDraw={Slider.drawRotary('ff66FDCF', 'ff626262')}
 *  mapDragGestureToValue={Slider.rotaryGestureMap}
 * />
 *
 */
export class Slider extends Component<
  PropsWithChildren<SliderProps | any>,
  SliderState
> {
  static linearHorizontalGestureMap = _linearHorizontalGestureMap;
  static linearVerticalGestureMap = _linearVerticalGestureMap;
  static rotaryGestureMap = _rotaryGestureMap;

  static drawLinearHorizontal = _drawLinearHorizontalSlider;
  static drawLinearVertical = _drawLinearVerticalSlider;
  static drawRotary = _drawRotarySlider;

  private _valueAtDragStart = 0.0;
  private _mouseDownX = 0;
  private _mouseDownY = 0;

  static defaultProps = {
    sensitivity: 1 / 200,
    onDraw: _drawRotarySlider("ff626262", "ff66FDCF"),
    mapDragGestureToValue: _rotaryGestureMap,
  };

  constructor(props: PropsWithChildren<SliderProps | any>) {
    super(props);

    this._onMeasure = this._onMeasure.bind(this);
    this._onMouseDown = this._onMouseDown.bind(this);
    this._onMouseDrag = this._onMouseDrag.bind(this);
    this._onDraw = this._onDraw.bind(this);

    this.state = {
      width: 0,
      height: 0,
      value: 0.0,
    };
  }

  _onMeasure(e: any) {
    this.setState({
      width: e.width,
      height: e.height,
    });
  }

  _onMouseDown(e: SyntheticMouseEvent) {
    this._valueAtDragStart = this.props.hasOwnProperty("value")
      ? this.props.value
      : this.state.value;

    this._mouseDownX = e.x;
    this._mouseDownY = e.y;

    if (typeof this.props.onMouseDown === "function") {
      this.props.onMouseDown(e);
    }
  }

  _onMouseDrag(e: SyntheticMouseEvent) {
    let value = 0.0;

    if (typeof this.props.mapDragGestureToValue !== "function") {
      invariant(false, "Invalid gesture mapping function supplied.");
      return;
    }

    value = this.props.mapDragGestureToValue(
      this._mouseDownX,
      this._mouseDownY,
      this.props.sensitivity,
      this._valueAtDragStart,
      e
    );

    if (!this.props.hasOwnProperty("value")) {
      this.setState({
        value: value,
      });
    }

    if (typeof this.props.onChange === "function") {
      this.props.onChange(value);
    }

    if (typeof this.props.onMouseDrag === "function") {
      this.props.onMouseDrag(e);
    }
  }

  _onDraw(ctx: CanvasRenderingContext) {
    const value = this.props.hasOwnProperty("value")
      ? this.props.value
      : this.state.value;

    if (typeof this.props.onDraw === "function") {
      return this.props.onDraw(ctx, this.state.width, this.state.height, value);
    }
  }

  render() {
    return (
      <View
        {...this.props}
        onMeasure={this._onMeasure}
        onMouseDown={this._onMouseDown}
        onMouseDrag={this._onMouseDrag}
      >
        <Canvas onDraw={this._onDraw} {...styles.canvas} />
        {this.props.children}
      </View>
    );
  }
}

const styles = {
  canvas: {
    width: "100%",
    height: "100%",
    position: "absolute",
    interceptClickEvents: false,
  },
};
