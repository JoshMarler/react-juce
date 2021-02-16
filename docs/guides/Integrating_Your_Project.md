# Starting a New Project

This step assumes you've already reviewed the [Getting Started](Getting_Started.md) guide. If not,
please start there!

Ok, so you're ready to add some React.js to your JUCE project. Whether that's a totally new project or a preexisting project, getting React-JUCE involved is the same. If you're starting your project from scratch, I recommend running through the [Getting started with the Projucer](https://docs.juce.com/master/tutorial_new_projucer_project.html) tutorial on the JUCE website, if you haven't already.

Now before we write any code, we have to add React-JUCE to our JUCE project. Fortunately, JUCE makes this super
easy with the JUCE module format, and React-JUCE abides by that format. Follow along with the [Manage JUCE modules](https://docs.juce.com/master/tutorial_manage_projucer_project.html#tutorial_manage_projucer_project_managing_modules) section of the Projucer tutorial, wherein you'll need to add the React-JUCE module by pointing the Projucer to its location on disk. The actual React-JUCE JUCE module is located in the `react_juce` subdirectory of the root of the React-JUCE project.

## Template Generator

Next, the first thing we want to do here is write some React.js, so let's start with a "Hello World!" of our own. React-JUCE's `react-juce` npm package carries a template generator that you can use to boostrap a React application for your project. For this step, let's assume your JUCE project directory is at `~/MyProject`, the source files are at `~/MyProject/Source`, and we want to put the React application source at `~/MyProject/Source/jsui` (note, you can put this wherever you want). Now, to use the template generator, we start again at the root of the React-JUCE git repository:

```bash
$ pwd
/Users/nick/Dev/react-juce
$ cd packages/react-juce
$ npm run init -- ~/MyProject/Source/jsui
```

The template generator will create the `jsui` directory as suggested in the example command above, fill it
with a basic "Hello World!" app, and install local dependencies like React.js and Webpack. Like the [[GainPlugin Example|running-the-example]], we now need to build our output bundle.

```bash
$ cd ~/MyProject/Source/jsui
$ npm start
```

At this point we've got our app bundle ready to roll, so let's turn over to the native side to mount this into
our JUCE project.

## Native Code

Because we've already added the React-JUCE module to our Projucer project, we can jump straight into the code on the native side. Part of the native React-JUCE API is a particularly important class called `reactjuce::ReactApplicationRoot`. This class is mostly just a `juce::Component`, and in that way you should think about using it the same way you might use a `juce::Slider` in your application.

For example, let's suppose that we have our `MainComponent` or our `AudioProcessorPluginEditor` at the top of our project:

```cpp
class MainComponent   : public Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
```

Adding the `reactjuce::ReactApplicationRoot` is easy, and should be familiar if you've worked with `juce::Component`s before:

```cpp
class MainComponent   : public Component
{
public:
    //==============================================================================
    MainComponent()
    {
        addAndMakeVisible(appRoot);
        setSize(400, 300);
    }

    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;

    void resized() override
    {
        appRoot.setBounds(getLocalBounds());
    }

private:
    //==============================================================================
    reactjuce::ReactApplicationRoot appRoot;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
```

It's important to note here that our `appRoot` will be the point at which our React application code from earlier
will "take over," and it's also important to note that you can add this `appRoot` wherever you need it in your application. For example, if you want to write your entire interface in React, you should mount your `appRoot` at the top of your application in your `MainComponent` or your `AudioProcessorPluginEditor`. If instead you want only to write your preset switcher in React, you can build the rest of your interface as usual with JUCE, and add the `appRoot` wherever the preset switcher should go within the context of your interface.

Now we're almost done here, but if you compile and run right now you won't see your "Hello from React.js!"– of course, we haven't told the `appRoot` where to find the JavaScript bundle we made! So, putting the last piece together here:

```cpp
class MainComponent   : public Component
{
public:
    //==============================================================================
    MainComponent()
    {
        // First thing we have to do is load our javascript bundle from the build
        // directory so that we can evaluate it within our appRot.

        // TODO: Replace this with the appropriate path to your javascript bundle!
        File bundle = File("/path/to/your/jsui/build/js/main.js");

        addAndMakeVisible(appRoot);
        appRoot.evaluate(bundle);

        setSize(400, 300);
    }

    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;

    void resized() override
    {
        appRoot.setBounds(getLocalBounds());
    }

private:
    //==============================================================================
    reactjuce::ReactApplicationRoot appRoot;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
```

That's it! We've now integrated React-JUCE into our project, and now you can write freely in React.js and watch your application take shape.
