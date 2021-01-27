import React, { Component } from "react";

import { View } from "./View";
import { SyntheticMouseEvent } from "../lib/SyntheticEvents";
import { ViewInstance } from "../lib/Backend";

//TODO: Once ViewProps work is complete we can probably
//      remove this in favour of ViewProps.
export interface ButtonProps {
  onClick: (e: SyntheticMouseEvent) => void;
  onMouseDown?: (e: SyntheticMouseEvent) => void;
  onMouseUp?: (e: SyntheticMouseEvent) => void;
  onMouseEnter?: (e: SyntheticMouseEvent) => void;
  onMouseLeave?: (e: SyntheticMouseEvent) => void;
}

type ButtonState = {
  down: boolean;
};

/**
 * A simple Button component which can be used as a building block
 * for more complex button types.
 *
 * @example
 *
 * <Button onClick={() => { console.log("clicked"); }} {...styles.button}>
 *   <Text {...styles.text}>
       Hello World
 *   </Text>
 * </Button>
 *
 * const styles = {
 *   text: {
 *     'font-size': 18.0,
 *     'line-spacing': 1.6,
 *     'color': 'ff626262'
 *   },
 *   button: {
 *     'justify-content': 'center',
 *     'align-items': 'center',
 *     'width': '100%',
 *     'height': '100%',
 *     'border-radius': 5.0,
 *     'border-width': 2.0,
 *     'border-color': 'ff626262'
 *   }
 * };
 *
 */
export class Button extends Component<ButtonProps, ButtonState> {
  private _ref: React.RefObject<ViewInstance>;

  constructor(props: ButtonProps) {
    super(props);
    this._ref = React.createRef();

    this.state = {
      down: false,
    };
  }

  handleDown = (e: SyntheticMouseEvent): void => {
    if (typeof this.props.onMouseDown === "function")
      this.props.onMouseDown.call(null, e);

    this.setState({
      down: true,
    });
  };

  handleUp = (e: SyntheticMouseEvent): void => {
    if (typeof this.props.onMouseUp === "function")
      this.props.onMouseUp.call(null, e);

    this.setState({
      down: false,
    });

    if (typeof this.props.onClick === "function") {
      const instance = this._ref ? this._ref.current : null;

      if (instance && instance.contains(e.relatedTarget)) {
        this.props.onClick(e);
      }
    }
  };

  render = () => {
    const { onMouseDown, onMouseUp, onClick, ...other } = this.props;
    const opacity = this.state.down ? 0.8 : 1.0;

    return (
      <View
        onMouseDown={this.handleDown}
        onMouseUp={this.handleUp}
        opacity={opacity}
        viewRef={this._ref}
        {...other}
      >
        {this.props.children}
      </View>
    );
  };
}
