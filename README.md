# Blueprint
> Write cross-platform native apps with React.js and JUCE

Blueprint is a hybrid JavaScript/C++ framework that enables a [React.js](https://reactjs.org/) frontend for a [JUCE](http://juce.com/) application or plugin. It provides an embedded, ECMAScript-compliant JavaScript engine via [Duktape](http://duktape.org/), native hooks for rendering the React component tree via `juce::Component` instances, and a flexbox layout engine via [Yoga](https://yogalayout.com/).

For more information, see the introductory blog post here: [Blueprint: A JUCE Rendering Backend for React.js](https://nickwritesablog.com/blueprint-a-juce-rendering-backend-for-react-js)

## Examples
Blueprint is a young project, but already it provides the framework on which the entire user interface for [Creative Intent's Remnant](https://www.creativeintent.co/product/remnant) plugin is built.

![Creative Intent Remnant: Screenshot](https://github.com/nick-thompson/blueprint/blob/master/RemnantScreenShot.jpg)

Besides that, you can check out the example code in the `examples/` directory. See the "Documentation" section
below for building and running the demo plugin. If you have a project written with Blueprint that you want to share, get in touch! I would
love to showcase your work.

## Documentation

See [Blueprint Docs & Getting Started](https://nick-thompson.github.io/blueprint/)

## Contributing
Yes, please! I would be very happy to welcome your involvement. Take a look at the [open issues](https://github.com/nick-thompson/blueprint/issues)
or the [project tracker](https://github.com/nick-thompson/blueprint/projects/1) to see if there's outstanding work that you might
be able to get started. Or feel free to propose an idea or offer feedback by [opening an issue](https://github.com/nick-thompson/blueprint/issues/new) as well.

I don't have a formal style guide at the moment, so please try to match the present formatting in any code contributions.

## License

See [LICENSE.md](https://github.com/nick-thompson/blueprint/blob/master/LICENSE.md)
