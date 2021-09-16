TextInput component imitates web's `<input type="text">`.
https://developer.mozilla.org/en-US/docs/Web/HTML/Element/input/text
So it supports props like `placeholder`, `maxlength`, `onInput`, etc...

TextInput also supports React's controlled components model:
https://reactjs.org/docs/uncontrolled-components.html.
If the user supplies a `value` prop,
then it never renders anything into that text editor other than
the string supplied by that `value` prop.

## Example

```js
import React, { Component } from "react";

import { Text, TextInput, View } from "react-juce";

class App extends Component {
  constructor(props) {
    super(props);
    this._onInput = this._onInput.bind(this);

    this.state = {
      textValue: "",
    };
  }

  _onInput(event) {
    console.log(`onInput: ${event.value}`);
    this.setState({ textValue: event.value });
  }

  render() {
    return (
      <View>
        <TextInput
          value={this.state.textValue}
          placeholder="init message"
          onInput={this._onInput}
          {...styles.text_input}
        />
      </View>
    );
  }
}

const styles = {
  text_input: {
    backgroundColor: "ff303030",
    color: "ff66FDCF",
    fontSize: 15.0,
    fontFamily: "Menlo",
    fontStyle: Text.FontStyleFlags.bold,
    "placeholder-color": "ffAAAAAA",
    height: 30,
    width: 200,
  },
};

export default App;
```

## Props

**TODO**: Document props. See [the class](https://github.com/nick-thompson/react-juce/blob/master/packages/react-juce/src/components/TextInput.tsx#L11) for props in the meantime.
