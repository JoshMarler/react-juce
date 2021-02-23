# Button

A basic React component for rendering a button with web-like behavior and
event interactions.

## Example

```js
import React, { Component } from "react";
import { View, Button, Text } from "react-juce";

function App(props) {
  return (
    <View {...styles.outer}>
      <Button
        {...styles.button}
        onClick={(e) => console.log('Click')}
        onMouseEnter={(e) => console.log('Mouse Enter!)}
        onMouseLeave={(e) => console.log('Mouse Leave!)}>
        <Text>Click me!</Text>
      </Button>
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
  button: {
    borderColor: "#66cffd",
    padding: 20,
  },
};
```

## Props

`Button` inherits support for all of the core `View` properties described in [View](View.md).

#### onClick

A callback property which will be invoked in response to a mouse click. The callback should
accept a single [`SyntheticMouseEvent`](Events.md) argument.

| Type     | Required | Supported                                                                                 |
| -------- | -------- | ----------------------------------------------------------------------------------------- |
| function | No       | Partial: [Standard](https://developer.mozilla.org/en-US/docs/Web/API/Element/click_event) |

## Styles

`Button` supports all of the default style properties described in [Style Properties](Styles.md).
