# Blueprint
> Write cross-platform native apps with React.js and JUCE

Blueprint is a hybrid JavaScript/C++ framework that enables a [React.js](https://reactjs.org/) frontend for a [JUCE](http://juce.com/) application or plugin. It provides an embedded, ES5 JavaScript engine via [Duktape](http://duktape.org/), native hooks for rendering the React component tree to `juce::Component` instances, and a flexbox layout engine via [Yoga](https://yogalayout.com/).

For more information, see the introductory blog post here: [Blueprint: A JUCE Rendering Backend for React.js](https://nickwritesablog.com/blueprint-a-juce-rendering-backend-for-react-js)

## Status

**Approaching Beta**. We hope to announce a beta release in the coming weeks, after which we will aim our focus at stability and completeness on the path
to a 1.0 release.

**Anticipated Breaking Changes**
* We'll be renaming Blueprint to react-juce before beta (#34)
* ~~Updating the examples and `npm init` template to point to npm instead of the local package~~
* `ReactApplicationRoot::evaluate` and `ReactApplicationRoot::evaluateFile` (#115)
* ~~Refactoring the hot reloader and decoupling the EcmascriptEngine from ReactApplicationRoot (#65)~~

## Resources

* Documentation: [Blueprint Docs & Getting Started](https://nick-thompson.github.io/blueprint/)
* Discussions: [GitHub Discussions](https://github.com/nick-thompson/blueprint/discussions)
* Community: [The Audio Programmer Discord Server](https://discord.gg/3H4wwVf49v)
    * Join the `#blueprint` channel and say hi!

## Maintainers

* [@nick-thompson](https://github.com/nick-thompson)
* [@joshmarler](https://github.com/JoshMarler)

## Examples
Blueprint is a young project, but already it provides the framework on which the entire user interface for [Creative Intent's Remnant](https://www.creativeintent.co/product/remnant) plugin is built.

![Creative Intent Remnant: Screenshot](https://github.com/nick-thompson/blueprint/blob/master/RemnantScreenShot.jpg)

Besides that, you can check out the example code in the `examples/` directory. See the "Documentation" section
below for building and running the demo plugin. If you have a project written with Blueprint that you want to share, get in touch! I would
love to showcase your work.

## Contributing
Yes, please! I would be very happy to welcome your involvement. Take a look at the [open issues](https://github.com/nick-thompson/blueprint/issues)
or the [project tracker](https://github.com/nick-thompson/blueprint/projects/1) to see if there's outstanding work that you might
be able to get started. Or feel free to propose an idea or offer feedback by [opening an issue](https://github.com/nick-thompson/blueprint/issues/new) as well.

I don't have a formal style guide at the moment, so please try to match the present formatting in any code contributions.

## License

See [LICENSE.md](https://github.com/nick-thompson/blueprint/blob/master/LICENSE.md)
