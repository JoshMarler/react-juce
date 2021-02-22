# Text

A React component for displaying text.

Unlike React Native and React-DOM, `Text` does not yet support nesting ([#6](https://github.com/nick-thompson/react-juce/issues/6)), though
it does happily support styling, and touch handling.

## Example

```js
import React, { Component } from "react";
import { View, Text } from "react-juce";

function App(props) {
  return (
    <View {...styles.outer}>
      <Text {...styles.labelText}>Hello, </Text>
      <Text {...styles.nameText}>{props.name}</Text>
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
  labelText: {
    color: "#626262",
    fontSize: 16.0,
    lineSpacing: 1.6,
  },
  nameText: {
    color: "#6262f8",
    fontSize: 16.0,
    lineSpacing: 1.6,
  },
};
```

## Props

`Text` inherits support for all of the core `View` properties described in [View](View.md).

#### color

The color used to draw the text

| Type   | Required | Supported                                                          |
| ------ | -------- | ------------------------------------------------------------------ |
| string | No       | [Standard](https://developer.mozilla.org/en-US/docs/Web/CSS/color) |

#### fontSize

The font size used to draw the text

| Type   | Required | Supported                                                             |
| ------ | -------- | --------------------------------------------------------------------- |
| string | No       | [Partial](https://developer.mozilla.org/en-US/docs/Web/CSS/font-size) |

#### fontFamily

The font family name with which to draw the text

| Type   | Required | Supported                                                               |
| ------ | -------- | ----------------------------------------------------------------------- |
| string | No       | [Partial](https://developer.mozilla.org/en-US/docs/Web/CSS/font-family) |

#### fontStyle

Sets whether the text should be rendered bold, italics, or normal.

| Type   | Required | Supported                                                              |
| ------ | -------- | ---------------------------------------------------------------------- |
| string | No       | [Partial](https://developer.mozilla.org/en-US/docs/Web/CSS/font-style) |

#### justification

Sets the horizontal alignment of the text within the container.

TODO: Rename this prop and match the text-align spec

| Type   | Required | Supported                                                            |
| ------ | -------- | -------------------------------------------------------------------- |
| string | No       | [Non-Standard](https://docs.juce.com/master/classJustification.html) |

#### kerningFactor

Sets the horizontal spacing between text characters.

TODO: Rename this prop and match the letter-spacing spec

| Type   | Required | Supported                                                                                     |
| ------ | -------- | --------------------------------------------------------------------------------------------- |
| string | No       | [Non-Standard](https://docs.juce.com/master/classFont.html#a996b7095b0956f62b71f24893e72a914) |

#### lineSpacing

Sets the height of a line box, commonly used to apply vertical spacing between
neighboring lines of text.

TODO: Rename this prop and match the line-height spec

| Type   | Required | Supported                                                                                                 |
| ------ | -------- | --------------------------------------------------------------------------------------------------------- |
| string | No       | [Non-Standard](https://docs.juce.com/master/classAttributedString.html#a0506d7b2000aaebd5873ea23eca6ae6a) |

#### wordWrap

Sets the word-wrap behavior for when the text content overflows its content box.

| Type   | Required | Supported                                                                                                 |
| ------ | -------- | --------------------------------------------------------------------------------------------------------- |
| string | No       | [Non-Standard](https://docs.juce.com/master/classAttributedString.html#ad752f270294ec5b2cef0c80863ee3a3c) |

## Styles

`Text` supports all of the default style properties described in [Style Properties](Styles.md).
