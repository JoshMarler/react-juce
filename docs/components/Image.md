# Image

A React component for displaying different types of images, including network images,
local image files, and image data URLs.

The example below demonstrates displaying a local image file, and a data url.

## Example

```js
import React, { Component } from "react";
import { View, Image } from "react-juce";

// Using a bundling tool like Webpack or Rollup, we can configure this import
// to read the image file from disk at the time of bundling, and embed the data
// URL in our javascript bundle which can be imported just like this.
import logoDataUri from "./logo.jpg";

function App(props) {
  return (
    <View {...styles.outer} onMouseDown={(e) => console.log("Mouse event!", e)}>
      <Image {...styles.image} source="file:///Users/nick/Documents/logo.jpg" />
      <Image {...styles.image} source={logoDataUri} />
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
  image: {
    width: "25%",
    height: "25%",
    padding: 20,
  },
};
```

## Props

`Image` inherits support for all of the core `View` properties described in [View](View.md).

#### source

The image source (either a remote URL ([#14](https://github.com/nick-thompson/react-juce/issues/14)), a local file resource, or a data uri).

| Type   | Required | Supported                                                                  |
| ------ | -------- | -------------------------------------------------------------------------- |
| string | No       | Partial: [Standard](https://developer.mozilla.org/en-US/docs/Glossary/URL) |

## Styles

`Image` supports all of the default style properties described in [Style Properties](Styles.md).
