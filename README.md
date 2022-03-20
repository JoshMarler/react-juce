# React-JUCE

> Write cross-platform native apps with React.js and JUCE

TODO: Reference Yoga and Hermes/Duktape

React-JUCE is a hybrid JavaScript/C++ framework that enables a [React.js](https://reactjs.org/) UI for a [JUCE](http://juce.com/) application or plugin.

For more information, see the introductory blog post here: [A JUCE Rendering Backend for React.js](https://nickwritesablog.com/blueprint-a-juce-rendering-backend-for-react-js)

## So you wanna use JavaScript to build audio apps/plugins ...

Wait! Have you seen [Elementary](https://github.com/nick-thompson/elementary)?

Why are we pointing you at another project?

React-JUCE and Elementary serve two slightly different purposes.
If you're looking to develop audio apps in modern functional JavaScript
then Elementary might be the project for you. React-JUCE allows you to
add a React JS UI to your JUCE C++ app. What if we told you there is also a JS
based environment for your DSP/audio code (and yes the performance is pretty darn good)?
You can write your entire plugin (UI and DSP) in JS, oh and you can hot-reload eveything too!

[Elementary](https://github.com/nick-thompson/elementary)?

OK, so why React-JUCE?

Well, many reasons. Maybe you have an existing library of C++ DSP/Audio code,
perhaps its a library that you've spent hard hours (OK, hard years) crafting and
you just want a rapid development workflow for your UI? React-JUCE is for you.
Perhaps you're developing embedded Linux apps and you need C++ but want to 
use React/JavaScript for your UI code? React-JUCE can help.
Maybe you hate JavaScript? Wait ... You probably shouldn't be here.
Maybe you're just much more comfortable writing DSP (or code in general) in C++
and you just want to try out React for your UI? React-JUCE is for you.

We believe React-JUCE still has lot to offer even with [Elementary](https://github.com/nick-thompson/elementary)'s existence.
So, if you've weighed up your pros and cons then jump in, we're happy to have you!

## Status

**Approaching Beta**. Maintenance/Development of React-JUCE has recently been
taken over from Nick Thompson (original creator) by Joshua Marler. The repository
is going through a temporary restructure but we are pushing towards a stable v1.0.
Updates will be posted once v1.0 is hit. In the meantime please direct any questions
to discussions or to our Discord channel.

## Resources

- Documentation: [Docs & Getting Started](https://docs.react-juce.dev)
- Discussions: [GitHub Discussions](https://github.com/JoshMarler/react-juce/discussions)
- Community: [The Audio Programmer Discord Server](https://discord.gg/3H4wwVf49v)
  - Join the `#react-juce` channel and say hi!

## Maintainers

Originally authored by @nick-thompson, now maintained by @JoshMarler.

- [@joshmarler](https://github.com/JoshMarler)
- [@nick-thompson](https://github.com/nick-thompson)

## Examples

You can check out a selection of example plugins over at [react-juce-examples](https://github.com/JoshMarler/react-juce-examples)

To see a real world example of a plugin build with react-juce checkout [Creative Intent's Remnant](https://www.creativeintent.co/product/remnant).

If you have a project written with React-JUCE that you want to share, get in touch! We would love to showcase your work.

## Contributing

See [CONTRIBUTING.md](https://github.com/JoshMarler/react-juce/blob/master/CONTRIBUTING.md)

## License

See [LICENSE.md](https://github.com/JoshMarler/react-juce/blob/master/LICENSE.md)
