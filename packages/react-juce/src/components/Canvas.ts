import React, { Component } from 'react';
import { Touch } from '../lib/SyntheticEvents'

export function bindCanvasContextProperties(ctx: any) {
  Object.defineProperty(ctx, 'fillStyle', {
    enumerable: false,
    configurable: false,
    get: function() {
      return 'Not Supported';
    },
    set: function(value) {
      this.__setFillStyle(value);
    }
  });

  Object.defineProperty(ctx, 'strokeStyle', {
    enumerable: false,
    configurable: false,
    get: function() {
      return 'Not Supported';
    },
    set: function(value) {
      this.__setStrokeStyle(value);
    }
  });

  Object.defineProperty(ctx, 'lineWidth', {
    enumerable: false,
    configurable: false,
    get: function() {
      return 'Not Supported';
    },
    set: function(value) {
      this.__setLineWidth(value);
    }
  });

  Object.defineProperty(ctx, 'font', {
    enumerable: false,
    configurable: false,
    get: function() {
      return 'Not Supported';
    },
    set: function(value) {
      this.__setFont(value);
    }
  });

  Object.defineProperty(ctx, 'textAlign', {
    enumerable: false,
    configurable: false,
    get: function() {
      return 'Not Supported';
    },
    set: function(value) {
      this.__setTextAlign(value);
    }
  });
}

export interface CanvasProps {
  onMeasure?: (e: any) => void;
  onDraw: (ctx: CanvasRenderingContext2D, sizeState: object) => void;
  autoclear?: boolean;
  ongoingTouches?: Array<Touch>;
}

interface CanvasState {
  width: number;
  height: number;
  firstStroke?: boolean;
}

export class Canvas extends Component<CanvasProps, CanvasState> {
  constructor(props: CanvasProps) {
    super(props);

    this._onMeasure = this._onMeasure.bind(this);
    this._onDraw = this._onDraw.bind(this);

    this.state = {
      width: 0,
      height: 0,
      firstStroke: true
    };
  }

  _onMeasure(e: any) {
    this.setState({
      width: e.width,
      height: e.height
    });

    if (typeof this.props.onMeasure === 'function') {
      this.props.onMeasure(e);
    }
  }

  _onDraw(ctx: CanvasRenderingContext2D) {
    if (typeof this.props.onDraw === 'function') {
      bindCanvasContextProperties(ctx);

      if (this.props.autoclear) {
        ctx.clearRect(0, 0, this.state.width, this.state.height);
      }

      this.props.onDraw(ctx, this.state);
      if(this.state.firstStroke) {
        this.setState({
          firstStroke: false
        });
      }
    }
  }

  render() {
    //TODO: Check whether need to use below arrow function for "this" binding
    //      is a bug in duktape. Possible this only occurs on linux. Does not
    //      appear to occur on mac.
    return React.createElement('CanvasView', Object.assign({}, this.props, {
      onDraw: (ctx: CanvasRenderingContext2D) => { this._onDraw(ctx) },
      onMeasure: (e: any) => { this._onMeasure(e)}
    }), this.props.children);
  }
}
