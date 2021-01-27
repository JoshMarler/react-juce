# View

The most fundamental of the core React-JUCE components, `View` is a container that supports layout with flexbox, style, and some event handling.
`View` maps directly to the underlying native `reactjuce::View` instance, which is effectively a simple `juce::Component`.

`View` is designed to be nested inside other views and can have 0 to many children of any type.

## Example

```js
import React, { Component } from "react";
import { View } from "react-juce";

function App(props) {
  return (
    <View {...styles.outer} onMouseDown={(e) => console.log("Mouse event!", e)}>
      <View {...styles.inner} />
    </View>
  );
}

const styles = {
  outer: {
    width: "100%",
    height: "100%",
    backgroundColor: "ff17191f",
    justifyContent: "center",
    alignItems: "center",
  },
  inner: {
    width: "25%",
    height: "25%",
    backgroundColor: "ffa7191f",
  },
};
```

## Props

#### onMeasure

A callback which will be invoked any time the `View`'s layout calculation changes.
The callback should accept a single argument, a [SyntheticEvent](Events.md) object holding
a `width` and `height` property reflecting the new size of the `View`.

| Type     | Required | Supported         |
| -------- | -------- | ----------------- |
| function | No       | Yes: Non-Standard |

#### onMouseDown

A callback which will be invoked in response to a mouse button down event on the
underlying native component. The callback should accept a single argument, a [SyntheticMouseEvent](Events.md)
object similar in interface to [MouseEvent](https://developer.mozilla.org/en-US/docs/Web/API/MouseEvent).

| Type     | Required | Supported         |
| -------- | -------- | ----------------- |
| function | No       | Partial: Standard |

#### onMouseUp

A callback which will be invoked in response to a mouse button up event on the
underlying native component. The callback should accept a single argument, a [SyntheticMouseEvent](Events.md)
object similar in interface to [MouseEvent](https://developer.mozilla.org/en-US/docs/Web/API/MouseEvent).

| Type     | Required | Supported         |
| -------- | -------- | ----------------- |
| function | No       | Partial: Standard |

#### onMouseEnter

A callback which will be invoked in response to a mouse entering the local bounds of a native component.
The callback should accept a single argument, a `SyntheticMouseEvent`
object similar in interface to [MouseEvent](https://developer.mozilla.org/en-US/docs/Web/API/MouseEvent).

| Type     | Required | Supported         |
| -------- | -------- | ----------------- |
| function | No       | Partial: Standard |

#### onMouseLeave

A callback which will be invoked in response to a mouse leaving the local bounds of a native component.
The callback should accept a single argument, a `SyntheticMouseEvent`
object similar in interface to [MouseEvent](https://developer.mozilla.org/en-US/docs/Web/API/MouseEvent).

| Type     | Required | Supported         |
| -------- | -------- | ----------------- |
| function | No       | Partial: Standard |

#### onMouseDoubleClick

A callback which will be invoked in response to a mouse button double click on the
underlying native component. The callback should accept a single argument, a [SyntheticMouseEvent](Events.md)
object similar in interface to [MouseEvent](https://developer.mozilla.org/en-US/docs/Web/API/MouseEvent).

| Type     | Required | Supported         |
| -------- | -------- | ----------------- |
| function | No       | Partial: Standard |

#### onKeyPress

A callback which will be invoked in response to a key press event while the
underlying native component has focus. The callback should accept a single argument, a [SyntheticKeyboardEvent](Events.md)
object similar in interface to [KeyboardEvent](https://developer.mozilla.org/en-US/docs/Web/API/KeyboardEvent).

| Type     | Required | Supported         |
| -------- | -------- | ----------------- |
| function | No       | Partial: Standard |

## Styles

`View` supports all of the default style properties described in [Style Properties](Styles.md).
