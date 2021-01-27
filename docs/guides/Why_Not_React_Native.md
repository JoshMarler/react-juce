# Why Not React Native?

The React community first formed in 2013 when React was introduced as a framework for writing web applications. Shortly thereafter, the team announced React Native for rendering native iOS and Android mobile apps, and the React community grew accordingly. The community then introduced a project for rendering native Windows desktop apps from React, and since then several projects have spun off with similar goals. Why, then, do we need another one in React-JUCE?

To start, I think it's helpful to clarify the goals of these projects and the underlying philosophy that brings them together. Then, I think's helpful to recognize JUCE's utility and popularity in the audio application development space. Finally, once we've covered those points, I think the answer to our original question becomes quite clear, and shows why React-JUCE stands to make an impact in the audio application development space.

## The React Philosophy

When the React team introduced React Native in 2015, they made a point to clarify that React and React Native were different projects. With its introduction, they started sharing the idea "Learn once, write anywhere," which we still see in their messaging today. What we see from that message is that their goal was never to be able to write a React web app and then push a button to ship that same app to native mobile. Rather, the goal was slightly higher level.

At the initial React.js announcement, the library focused totally on web applications. When React Native showed up, we saw the team split out the React DOM package. This step is very illuminating. After that, to talk about React in the context of a cross-platform web/mobile app, we have to consider three things: React, React DOM, and React Native. This distinction is helpful in that it shows React itself is only an abstract tool. It deals in abstract representations of a user interface, and the business logic around that user interface. But eventually this abstraction layer decays to real rendering primitives, and here we see what separates React DOM and React Native. React DOM provides a library of rendering primitives targeting the Web DOM. React Native provides a library of rendering primitives targeting native platform components in iOS and Android. (As a brief aside, this distinction in rendering primitives is exactly where React-JUCE fits into the picture as well, offering a library of rendering primitives that target JUCE.)

Through this lens, React Native's "Learn once, write anywhere" philosophy makes sense. Different rendering primitives on different platforms may have totally different semantics, and the idea of writing an application against one set of rendering primitives and then pushing a button to render against a new set of primitives just falls over in some cases. But the idea of _working in React_ never changes. With these tools, a single team can write a web application and then turn around and write a native iOS, Android or Windows desktop application with total familiarity. This isn't "push a button, render anywhere," it's about lowering the barrier to entry to new platforms, unifying the development experience, and empowering developers to cross what have long been great divides in our industry.

## The JUCE Ecosystem

Our brief survey of the React landscape tells a compelling story when viewed from the perspective of a generic application developer. But what happens when we look from the perspective of an audio application developer?

React Native solves a very non-trivial problem in the way it bridges between iOS and Android. A React Native developer can write a single application and deploy it to two totally different platforms, while React Native takes care of answering all the prerequisite questions about how to get an application up and running on each respective platform, how to provision that top level window, and how to evaluate the application JavaScript bundle in an embedded interpreter.

JUCE takes its place in the audio application development ecosystem answering very similar questions. Nearly all audio applications start with the same two requirements:

- A window provisioned somewhere (either directly or perhaps by a host program) into which we can render our interface
- An audio buffer from the driver on which we can operate our signal processing

Unfortunately, in audio, and especially when we consider audio plugin development, this is a combinatorial problem which grows extremely quickly. To answer even the first bullet point above, we have a lot to consider.

- How do I get to that top-level window when running as an AAX plugin on Windows?
  - What rendering primitives do I use once I'm there? Windows Platform APIs?
- How do I get to that top-level window when running as an AU plugin on MacOS?
  - And what rendering primitives can I use here? CoreGraphics?
- And what if I want to ship the same application as a standalone Linux application? Or even an embedded Linux application? How do I get to that top-level window against X11 there?

I could keep that list running for a _long_ time. And that's just to address the first of the core requirements of an audio application; the same list runs back for getting access to the underlying audio callback in all of these scenarios.

Here, I think it's fair to say that JUCE has solved a massively challenging problem very elegantly in abstracting over all of these combinations. To write an audio application with JUCE, developers need only start from the core questions about _what_ the application will do once it has access to that window and the underlying audio buffer, and JUCE will take care of extending that functionality across all of the combinations of target platform, windowing system, audio driver, and plugin host. This is why, in my opinion, JUCE has become such a widely adopted framework for audio application development.

## React + JUCE = <3

Against this backdrop, a compelling case for React-JUCE is already starting to reveal itself. But before going any further, I think it's worthwhile to say that React Native and React Native Windows _are_ great options.

If your goal is singular in focus, say, to ship an audio application on iOS, then starting at React Native is an excellent choice. React Native will solve the top level window question for you, and you may then have to solve the problem of "How do I get to that audio buffer when running as a standalone on iOS," but that's a problem you'll only need to solve once. Or you might start at JUCE and figure out how to mount React Native into the window that JUCE provisions for you. Either way, in a scenario like this, your project will be neatly limited in the scope of the problems that you have to solve. I would happily recommend to people with a goal that has this type of focus to use React Native, or React Native Windows.

But this approach starts to break down when the project goal broadens. For example, maybe you want to ship your audio application as a standalone desktop app and a standalone iOS app. Now, if you start with React Native, you can find your answer to the question above for getting to the audio buffer just the same on iOS, but what happens when you want to ship to desktop? Well, maybe the audio buffer part is the same, and surely if you've integrated JUCE into your React Native project you can rely on JUCE for that part here too, but React Native doesn't render to desktop. So do you integrate a different framework for writing the desktop interface? Maybe you can integrate React Native Windows, but what about MacOS? Suddenly we're creeping back into the combinatorial problem we mentioned above, and we haven't even cracked this door open to include shipping the application as a cross-platform audio plugin, or potentially as a Linux application.

Here is exactly the place where React-JUCE finds its home. With React-JUCE, we can leverage JUCE's elegant solution to this hugely combinatorial problem, while at the same time leveraging the capabilities that React has introduced to the world of app UI programming, with no extra overhead. Shipping an audio application with React as a standalone application across MacOS, Windows, Linux, and as a plugin across AAX, AU, VST3, LV2 is now as easy as it should be.

And there's more still. It would be a shame to acknowledge the popularity of JUCE without addressing all of the many projects already shipped or actively underway with JUCE. React-JUCE is designed to be trivially integrated into any existing JUCE project: anywhere you can mount a `juce::Component`, whether that's in an existing project or a brand new project, you can mount React-JUCE, and start writing React.

I started this project with the goal of fully embracing the "Learn once, write anywhere" message introduced by React and React Native. It's my hope now that in the same way React Native empowers React developers to approach mobile platforms, React-JUCE empowers developers to approach the audio application space.
