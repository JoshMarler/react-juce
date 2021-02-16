# Style Properties

Style properties in React-JUCE generally aim to match the behavior and experience
of writing CSS or writing a StyleSheet with React Native. It's important to note, however,
that these style properties _are not_ proper CSS, and that we have no notion of parsing CSS
stylesheets. In React-JUCE, these properties are simple directives passed to the underlying `juce::Component`
heirarchy that yield behavior that matches much of the CSS specificiation.

At present, these style properties are applied as a top-level prop to any given component, as you would with
any other prop. In the future, we may adopt an API closer to that of React Native's StyleSheet.

## Layout Props

| Property           | Support | Spec                                                                             |
| ------------------ | ------- | -------------------------------------------------------------------------------- |
| flex               | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| flex-grow          | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| flex-shrink        | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| flex-basis         | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| flex-direction     | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| justify-content    | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| align-items        | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| align-content      | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| align-self         | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| flex-wrap          | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| position           | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| left               | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| top                | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| right              | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| bottom             | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| width              | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| height             | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| min-width          | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| min-height         | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| max-width          | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| max-height         | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| aspect-ratio       | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| direction          | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| overflow           | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| margin             | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| margin-left        | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| margin-top         | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| margin-right       | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| margin-bottom      | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| margin-start       | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| margin-end         | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| margin-horizontal  | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| margin-vertical    | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| padding            | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| padding-left       | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| padding-top        | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| padding-right      | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| padding-bottom     | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| padding-start      | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| padding-end        | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| padding-horizontal | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |
| padding-vertical   | Yes     | [Flexbox](https://developer.mozilla.org/en-US/docs/Learn/CSS/CSS_layout/Flexbox) |

## View Props

| Property                   | Support                                                                  | Spec                                                                                                            |
| -------------------------- | ------------------------------------------------------------------------ | --------------------------------------------------------------------------------------------------------------- |
| border-left-width          | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-width](https://developer.mozilla.org/en-US/docs/Web/CSS/border-width)                               |
| border-right-width         | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-width](https://developer.mozilla.org/en-US/docs/Web/CSS/border-width)                               |
| border-top-width           | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-width](https://developer.mozilla.org/en-US/docs/Web/CSS/border-width)                               |
| border-bottom-width        | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-width](https://developer.mozilla.org/en-US/docs/Web/CSS/border-width)                               |
| border-left-color          | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-color](https://developer.mozilla.org/en-US/docs/Web/CSS/border-color)                               |
| border-right-color         | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-color](https://developer.mozilla.org/en-US/docs/Web/CSS/border-color)                               |
| border-top-color           | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-color](https://developer.mozilla.org/en-US/docs/Web/CSS/border-color)                               |
| border-bottom-color        | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-color](https://developer.mozilla.org/en-US/docs/Web/CSS/border-color)                               |
| border-bottom-left-radius  | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-radius](https://developer.mozilla.org/en-US/docs/Web/CSS/border-radius)                             |
| border-bottom-right-radius | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-radius](https://developer.mozilla.org/en-US/docs/Web/CSS/border-radius)                             |
| border-top-left-radius     | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-radius](https://developer.mozilla.org/en-US/docs/Web/CSS/border-radius)                             |
| border-top-right-radius    | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-radius](https://developer.mozilla.org/en-US/docs/Web/CSS/border-radius)                             |
| border-width               | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-width](https://developer.mozilla.org/en-US/docs/Web/CSS/border-width)                               |
| border-color               | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-color](https://developer.mozilla.org/en-US/docs/Web/CSS/border-color)                               |
| border-radius              | Planned [(#143)](https://github.com/nick-thompson/react-juce/issues/143) | [MDN border-radius](https://developer.mozilla.org/en-US/docs/Web/CSS/border-radius)                             |
| background-color           | Partial [(#84)](https://github.com/nick-thompson/react-juce/issues/84)   | [MDN background-color](https://developer.mozilla.org/en-US/docs/Web/CSS/background-color)                       |
| opacity                    | Yes                                                                      | [juce::Component::setAlpha](https://docs.juce.com/master/classComponent.html#a1b9329a87c71ed01319071e0fedac128) |
