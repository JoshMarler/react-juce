# JavaScript/Native Interop

There are generally two ways to cross the Native/JavaScript boundary:

1. You can pass C++ values and functions into JavaScript, and JavaScript values and functions into C++ directly or via an event model
2. Or, you can create a custom native View and render it in your React component tree

## Value Passing

To pass C++ values and functions into your JS environment, see `EcmascriptEngine::registerNativeProperty`
and `EcmascriptEngine::registerNativeMethod`. These methods will install the given values on either the `global`
object, or named object of your choosing.

In the other direction, you can pass values from JavaScript into C++ either via invoking the methods that
were installed via `EcmascriptEngine::registerNativeMethod` or by assigning a value to a named object accessible
from the global object, and looking that value up or invoking it on the C++ side with `EcmascriptEngine::evaluateInline` or `EcmascriptEngine::invoke`.

### Example

```cpp
// From C++
engine->registerNativeProperty("world", "Hello!");
engine->registerNativeMethod("sayHello", [this](juce::var::NativeFunctionArgs const& args) {
    // Here, `args.arguments[0]` comes from JavaScript
    std::cout << args.arguments[0].toStdString() << std::endl;
});
```

```js
// Then, from js
console.log("Hello", global.world);
global.sayHello("World!");
```

And similarly, in the other direction:

```js
// From js
global.myFun = (x) => x * x;
global.myValue = 17;
```

```cpp
// From C++
auto myValue = engine->evaluateInline("global.myValue");
auto result = engine->invoke("myFun", (int) myValue);
```

## Custom Native View

Now the other option is to define a totally custom native view, and register it with React.
To do this, you can make your own C++ class that inherits `reactjuce::View`, and override the necessary methods.
Basically, a `reactjuce::View` is just a `juce::Component` with some extra goodies.

For greater detail, see [Custom Native Components](Custom_Native_Components.md), but for a brief example, see below.

### Example

```cpp
class MyCoolView : public reactjuce::View {
  void paint(juce::Graphics& g) override {
    // Do your own paint routine like usual.
    // You can also treat this whole class instance like your normal juce::Components. Add children, `addAndMakeVisible`,
   // `resized` and everything!
  }
}
```

Now once you've got your custom view implementation, you have to tell React about it so that you can use it. The end goal with this is to be able to write something like this:

```js
function MyReactApp(props) {
  return (
    <View>
      <MyCoolView customProp="hi" otherCustomProp="bye" />
    </View>
  );
}
```

With this, you can write your juce::Components like normal, but use React to compose these things together into your full application.
So, how do we register your C++ class with React? All you need is `ReactApplicationRoot::registerViewType`:

```cpp
mAppRoot.registerViewType("MyCoolView", []() {
  // This lambda will be called when we need to construct one of your custom view instances. So, first, we make one:
  auto view = std::make_unique<MyCoolView>();

  // Then we need to construct a shadow view, which will govern the layout. 99% of the time, you can just use the
  // default shadow view:
  auto shadowView = std::make_unique<ShadowViewType>(view.get());

  // Then just return these guys as a pair
  return {std::move(view), std::move(shadowView)};
});
```

That's it, React can now understand your `MyCoolView`!

Now there's one last cosmetic issue. In React, you can always make elements with `React.createElement("MyCoolView", props, children);`,
but you rarely see that because people are used to using JSX and writing `<MyCoolView {...props}>`.
If you want to use JSX like that you have to make this little wrapper to abstract over the dynamic string argument:

```js
function MyCoolView(props) {
  return React.createElement("MyCoolView", props, props.children);
}

// Now I can use it elsewhere in JSX...
function MyApp(props) {
  return <MyCoolView {...props} />;
}
```
