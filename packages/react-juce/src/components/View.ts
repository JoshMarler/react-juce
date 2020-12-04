import React, { Component } from 'react';

import {
  parseBorderColorProp,
  parseBorderColorProps,
  parseBorderWidthProp,
  parseBorderWidthProps,
  parseBorderRadiusProp,
  parseBorderRadiusProps,
  buildBorderSVG
} from './ViewBorders';

interface ViewState {
  width: number,
  height: number,
}

//TODO: replace any with ViewProps
export class View extends Component<any, ViewState> {
  public static ClickEventFlags = {
    disableClickEvents: 0,
    allowClickEvents: 1,
    allowClickEventsExcludingChildren: 2,
    allowClickEventsOnlyOnChildren: 3,
  };

  public static EasingFunctions = {
    linear: 0,
    quadraticIn: 1,
    quadraticOut: 2,
    quadraticInOut: 3,
  };

  constructor(props: any) {
    super(props);

    this._onMeasure = this._onMeasure.bind(this);
    this._buildBorder = this._buildBorder.bind(this);

    this.state = {
      width: 0,
      height: 0
    }
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

  //TODO: OK to always prefer border-width, border-color, border-radius over
  //      their more specific counterparts?
  _buildBorder() {
    const borderWidths = (typeof this.props['border-width'] !== 'undefined')
                          ? parseBorderWidthProp(this.props['border-width'])
                          : parseBorderWidthProps(this.props);


    const borderColors = (typeof this.props['border-color'] !== 'undefined')
                          ? parseBorderColorProp(this.props['border-color'])
                          : parseBorderColorProps(this.props);

    const borderRadii = (typeof this.props['border-radius'] !== 'undefined')
                          ? parseBorderRadiusProp(this.props['border-radius'])
                          : parseBorderRadiusProps(this.props);

    //TODO: Better way to do this?
    // If no border props have been set we return an empty svg border string
    // to native.
    if (Object.values(borderWidths).every(el => el === undefined) ||
        Object.values(borderColors).every(el => el === undefined) ||
        Object.values(borderRadii).every(el => el === undefined)) {
      return "";
    }

    //TODO: Do we pass background color here or somehow pickup
    //      the clipping path in native?
    return buildBorderSVG(
      this.state.width,
      this.state.height,
      this.props['background-color'],
      borderColors,
      borderWidths,
      borderRadii
    );
  }

  render() {
    const borderSVG = this._buildBorder();

    // We'll need to wrap the default native components in stuff like this so that
    // you can use <View> in your JSX. Otherwise we need the dynamic friendliness
    // of the createElement call (note that the type is a string...);
    return React.createElement('View', Object.assign({}, this.props, {
      onMeasure: (e: any) => { this._onMeasure(e); },
      'border-svg': borderSVG
    }), this.props.children);
  }
}



