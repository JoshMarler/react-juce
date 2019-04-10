## Minor Issues
* Need to provide manual `Date` support (`new Date()` / `Date.now()`)
* Need to implement timers (`setTimeout`, `setInterval`)

## Major Questions

### Debugging
* As implemented, there's no debug support, which means that any issues occuring
  in the JavaScript bundle will be uncaught and unresolved, and largely tracked down
  via console.log statements.
* Duktape does provide a debugging protocl which seems easy enough to adopt. It'd
  be nice perhaps to have a hotkey when building in Debug mode that automatically
  attaches the debugger and opens the bundled remote debugging UI.
* https://duktape.org/guide.html#debugger

### Images
* There are a couple options here. One is to load image data into the JavaScript
  bundle and provide a default `<Image />` view type that receives a `source` property
  containing a `data:` URI scheme, interprets it to whatever representation JUCE needs,
  and then draws it. This seems to be roughly how React Native works.
  Another option is to initially support JUCE's BinaryData out of the
  box and provide a way for the `<Image />` view type to specify which data a particular
  `source` property is referring to.
* I'm tempted to lean on option 1 because it's a better development experience, and very
  easy to convince webpack to bundle image data into the JavaScript bundle itself. Surely
  this has a performance penalty for things like huge high-res knob strips, but perhaps we
  can address a BinaryData implementation after implementing option 1: a `<BinaryData />` interface
  introduced after the `<Image />` interface.

### Events
* React has a SyntheticEvent and an internal implementation for event bubbling through the
  virtual DOM. It looks like that code is public on github but not quite regarded as a public
  tool. I think it'd be unwise to mark that as a proper dependency here unless we can get the
  React team on board with publishing a proper package and maintaining it.
* For the time being I'm just exposing a BlueprintNative.pushEvent() interface on the C++ side
  which fills a JavaScript object and gets pushed through the central EventBroker interface on
  that side of things. Any JavaScript code which needs events must listen through there. That means
  that click events are not supported via `onClick` in JSX, etc.
* Obviously we want to get to a point where we have proper SyntheticEvents and proper bubbling through
  the virtual DOM, but can we get that Events package published? If not, do we roll our own or consider
  a different option (i.e. Rxjs)?
