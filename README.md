# React-JUCE

> Write cross-platform native apps with React.js and JUCE

React-JUCE (formerly named Blueprint) is a hybrid JavaScript/C++ framework that enables a [React.js](https://reactjs.org/) frontend for a [JUCE](http://juce.com/) application or plugin. It provides an embedded, ES5 JavaScript engine via [Duktape](http://duktape.org/), native hooks for rendering the React component tree to `juce::Component` instances, and a flexbox layout engine via [Yoga](https://yogalayout.com/).

For more information, see the introductory blog post here: [Blueprint: A JUCE Rendering Backend for React.js](https://nickwritesablog.com/blueprint-a-juce-rendering-backend-for-react-js)

## Status

**Approaching Beta**. We hope to announce a beta release in the coming weeks, after which we will aim our focus at stability and completeness on the path
to a 1.0 release.

**Anticipated Breaking Changes**

- We'll be renaming Blueprint to react-juce before beta (#34)
- ~~Updating the examples and `npm init` template to point to npm instead of the local package~~
- ~~`ReactApplicationRoot::evaluate` and `ReactApplicationRoot::evaluateFile` (#115)~~
- ~~Refactoring the hot reloader and decoupling the EcmascriptEngine from ReactApplicationRoot (#65)~~

## Resources

- Documentation: [Docs & Getting Started](https://docs.react-juce.dev)
- Discussions: [GitHub Discussions](https://github.com/nick-thompson/react-juce/discussions)
- Community: [The Audio Programmer Discord Server](https://discord.gg/3H4wwVf49v)
  - Join the `#blueprint` channel and say hi!

## Maintainers

- [@nick-thompson](https://github.com/nick-thompson)
- [@joshmarler](https://github.com/JoshMarler)

## Examples

React-JUCE is a young project, but already it provides the framework on which the entire user interface for [Creative Intent's Remnant](https://www.creativeintent.co/product/remnant) plugin is built.

![Creative Intent Remnant: Screenshot](https://github.com/nick-thompson/react-juce/blob/master/RemnantScreenShot.jpg)

Besides that, you can check out the example code in the `examples/` directory. See the "Documentation" section
below for building and running the demo plugin. If you have a project written with React-JUCE that you want to share, get in touch! I would
love to showcase your work.

## Contributing

See [CONTRIBUTING.md](https://github.com/nick-thompson/react-juce/blob/master/CONTRIBUTING.md)

## License

See [LICENSE.md](https://github.com/nick-thompson/react-juce/blob/master/LICENSE.md)
