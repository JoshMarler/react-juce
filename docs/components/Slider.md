# Slider

A basic React component for rendering a linear slider or rotary knob, common in
many audio applications.

`Slider` is simply a composition of a single `View` component, a single `Canvas` component,
and a set of default drawing operations to render common slider representations.

## Example

```js
import React, { Component } from "react";
import { View, Slider, Text } from "react-juce";

function App(props) {
  return (
    <View {...styles.outer}>
      <Slider
        {...this.props}
        value={props._value}
        onDraw={Slider.drawRotary}
        onMouseDown={props._onMouseDown}
        onMouseUp={props._onMouseUp}
        onChange={props._onSliderValueChange}
      >
        <Text>Cutoff Frequency</Text>
      </Slider>
    </View>
  );
}

const styles = {
  outer: {
    width: "100%",
    height: "100%",
    backgroundColor: "#17191f",
    justifyContent: "center",
    alignItems: "center",
  },
};
```

## Props

`Slider` inherits support for all of the core `View` properties described in [View](View.md).

#### sensitivity

Sets the sensitivity of a mouse drag interaction as it relates to changing the slider value.
The default value is `0.005`. Smaller numbers are less sensitive, larger numbers more sensitive.

| Type   | Required | Supported    |
| ------ | -------- | ------------ |
| number | No       | Non-Standard |

#### value

Sets the current value of the slider.

See React.js' notion of [controlled and uncontrolled components](https://reactjs.org/docs/forms.html#controlled-components). The `Slider`
value property and `onChange` behavior follow this model.

| Type   | Required | Supported    |
| ------ | -------- | ------------ |
| number | No       | Non-Standard |

#### onDraw

A callback property which will be invoked when its time to draw to the underlying `Canvas`.
This property is much like the `onDraw` property on the `Canvas` view itself, but with additional
arguments: (`context`, `width`, `height`, `value`).

These arguments are, respectively, the underlying canvas rendering context, the current width of the
underlying canvas object, the current height of the underlying canvas object, and the current slider value
on the range [0, 1].

Default drawing operations are provided as static members of the `Slider` class, which
can be passed directly as the `onDraw` property:

- `Slider.drawLinearHorizontal`
- `Slider.drawLinearVertical`
- `Slider.drawRotary` (Default)

| Type     | Required | Supported    |
| -------- | -------- | ------------ |
| function | No       | Non-Standard |

#### onChange

A callback property which will be invoked when the slider's value changes. The callback should
accept a single argument: a number on the range [0, 1].

| Type     | Required | Supported    |
| -------- | -------- | ------------ |
| function | No       | Non-Standard |

#### mapDragGestureToValue

A callback property which will be invoked to map a drag gesture to a new slider value. This
allows for customization of the drag behavior, for example:

- Behavior that maps drags "up and to the right" as increasing in value, and "down and to the left" as decreasing.
- Behavior that maps only rightward drag as increasing in value and leftward drag as decreasing invalue.
- Behavior that maps only upwards drag as increasing in value and downwards drag as decreasing invalue.

The callback should accept five arguments:
`(mouseDownX, mouseDownY, sensitivity, valueAtDragStart, dragEvent)`. Respectively, these
arguments are the x and y position of the mouse at the time of the mouseDown event, the `sensitivity`
of the slider as defined by the `sensitivity` prop above, the slider value at the time the drag started,
and the current `SyntheticMouseEvent` for the drag operation.

Default mapping operations are provided as static members of the `Slider` class, which
can be passed directly as the `mapDragGestureToValue` property:

- `Slider.linearHorizontalGestureMap`
- `Slider.linearVerticalGestureMap`
- `Slider.rotaryGestureMap` (Default)

| Type     | Required | Supported    |
| -------- | -------- | ------------ |
| function | No       | Non-Standard |

## Styles

`Slider` supports all of the default style properties described in [Style Properties](Styles.md).
