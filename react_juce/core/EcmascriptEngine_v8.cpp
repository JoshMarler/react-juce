/*
  ==============================================================================

    blueprint_EcmascriptEngine.cpp
    Created: 24 Oct 2019 3:08:39pm

  ==============================================================================
*/

#include "EcmascriptEngine.h"

#include <libplatform/libplatform.h>
#include <v8.h>


namespace blueprint
{

    //==============================================================================
    struct EcmascriptEngine::Pimpl
    {
        Pimpl() = default;
        ~Pimpl() = default;

        //==============================================================================
        juce::var evaluate (const juce::File& code)
        {
            jassert(code.existsAsFile());
            jassert(code.loadFileAsString().isNotEmpty());

            // Initialize V8.
            v8::V8::InitializeICUDefaultLocation("/Users/nick/Dev/v8/v8/hello_world");
            v8::V8::InitializeExternalStartupData("/Users/nick/Dev/v8/v8/hello_world");
            std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
            v8::V8::InitializePlatform(platform.get());
            v8::V8::Initialize();

            // Create a new Isolate and make it the current one.
            v8::Isolate::CreateParams create_params;
            create_params.array_buffer_allocator = v8::ArrayBuffer::Allocator::NewDefaultAllocator();
            v8::Isolate* isolate = v8::Isolate::New(create_params);

            {
                v8::Isolate::Scope isolate_scope(isolate);

                // Create a stack-allocated handle scope.
                v8::HandleScope handle_scope(isolate);

                // Create a new context.
                v8::Local<v8::Context> context = v8::Context::New(isolate);

                // Create an empty object representing __BlueprintNative__
                v8::Local<v8::ObjectTemplate> emptyTempl = v8::ObjectTemplate::New(isolate);
                v8::Local<v8::Object> native = emptyTempl->NewInstance(context).ToLocalChecked();

                native->Set(
                    context,
                    v8::String::NewFromUtf8(isolate, "getRootInstanceId").ToLocalChecked(),
                    v8::Function::New(context, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
                        if (args.Length() >= 1)
                        {
                            auto* isolate = args.GetIsolate();
                            v8::HandleScope scope(isolate);
                            v8::Local<v8::Value> arg = args[0];
                            v8::String::Utf8Value value(isolate, arg);
                            DBG(*value);
                        }
                        args.GetReturnValue().Set(1);
                    }).ToLocalChecked()
                );

                context->Global()->Set(
                    context,
                    v8::String::NewFromUtf8(isolate, "__BlueprintNative__").ToLocalChecked(),
                    native
                );

                context->Global()->Set(
                    context,
                    v8::String::NewFromUtf8(isolate, "setTimeout").ToLocalChecked(),
                    v8::Function::New(context, [](const v8::FunctionCallbackInfo<v8::Value>& args) {
                        DBG("setTimeout called");
                    }).ToLocalChecked()
                );

                // Enter the context for compiling and running the hello world script.
                v8::Context::Scope context_scope(context);

                {
                    // Create a string containing the JavaScript source code.
                    v8::Local<v8::String> source =
                        v8::String::NewFromUtf8(isolate, code.loadFileAsString().toRawUTF8(),
                                v8::NewStringType::kNormal)
                        .ToLocalChecked();

                    // Compile the source code.
                    v8::Local<v8::Script> script =
                        v8::Script::Compile(context, source).ToLocalChecked();

                    // Run the script to get the result.
                    v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();

                    // Convert the result to an UTF8 string and print it.
                    v8::String::Utf8Value utf8(isolate, result);
                    printf("%s\n", *utf8);
                }
            }

            // Dispose the isolate and tear down V8.
            isolate->Dispose();
            v8::V8::Dispose();
            v8::V8::ShutdownPlatform();
            delete create_params.array_buffer_allocator;

            return juce::var("TODO");
        }
    };

    //==============================================================================
    EcmascriptEngine::EcmascriptEngine()
        : mPimpl(std::make_unique<Pimpl>())
    {
        /** If you hit this, you're probably trying to run a console application.

            Please make use of juce::ScopedJuceInitialiser_GUI because this JS engine requires event loops.
            Without the initialiser, the console app would always crash on exit,
            and things will probably not get cleaned up.
        */
        jassert (juce::MessageManager::getInstanceWithoutCreating() != nullptr);
    }

    EcmascriptEngine::~EcmascriptEngine() = default;

    //==============================================================================
    juce::var EcmascriptEngine::evaluateInline (const juce::String& code)
    {
        return juce::var("Not Implemented.");
    }

    juce::var EcmascriptEngine::evaluate (const juce::File& code)
    {
        return mPimpl->evaluate(code);
    }

    //==============================================================================
    void EcmascriptEngine::registerNativeMethod (const juce::String& name, juce::var::NativeFunction fn)
    {
        registerNativeProperty(name, juce::var(fn));
    }

    void EcmascriptEngine::registerNativeMethod (const juce::String& target, const juce::String& name, juce::var::NativeFunction fn)
    {
        registerNativeProperty(target, name, juce::var(fn));
    }

    //==============================================================================
    void EcmascriptEngine::registerNativeProperty (const juce::String& name, const juce::var& value)
    {
        juce::ignoreUnused(name);
        juce::ignoreUnused(value);
    }

    void EcmascriptEngine::registerNativeProperty (const juce::String& target, const juce::String& name, const juce::var& value)
    {
        juce::ignoreUnused(name);
        juce::ignoreUnused(value);
    }

    //==============================================================================
    juce::var EcmascriptEngine::invoke (const juce::String& name, const std::vector<juce::var>& vargs)
    {
        return juce::var("Not Implemented");
    }

    void EcmascriptEngine::reset() {}

    //==============================================================================
    void EcmascriptEngine::debuggerAttach() {}
    void EcmascriptEngine::debuggerDetach() {}

}
