#pragma once

template <typename Error>
struct TimeoutFunctionManager : private juce::MultiTimer
{
    ~TimeoutFunctionManager() override {}

    void reset()
    {
        JUCE_ASSERT_MESSAGE_THREAD

        for(const auto &[id, timer] : timeoutFunctions)
        {
            stopTimer(id);
        }
        timeoutFunctions.clear();
    }

    juce::var clearTimeout(const juce::var::NativeFunctionArgs& args)
    {
        JUCE_ASSERT_MESSAGE_THREAD

        // all number arguments are read as double by dukReadVarFromDukStack
        if(args.numArguments < 1 || !(*args.arguments).isDouble())
        {
            throw Error("clearTimeout / clearInterval require a single integer id argument");
        }
        const int id = *args.arguments;
        clear(*args.arguments);
        return juce::var();
    }

    int newTimeout(const juce::var::NativeFunctionArgs& args, const bool recurring=false)
    {
        JUCE_ASSERT_MESSAGE_THREAD

        validateNewTimeoutArgs(args);
        const auto id = reserveId();
        timeoutFunctions.emplace(id, TimeoutFunction(args, recurring));
        const int timeoutMillis = *(args.arguments + 1);
        startTimer(id, timeoutMillis);
        return id;
    }

    int newInterval(const juce::var::NativeFunctionArgs& args)
    {
        return newTimeout(args, true);
    }

    void timerCallback(int id) override
    {
        JUCE_ASSERT_MESSAGE_THREAD

        const auto f = timeoutFunctions.find(id);
        if(f != timeoutFunctions.cend())
        {
            if(!f->second.call())
            {
            // f doesn't want to run again
            clear(id);
            }
        }
    }

    private:

        void clear(int id)
        {
            stopTimer(id);
            freeId(id);

            const auto f = timeoutFunctions.find(id);
            if(f != timeoutFunctions.cend()) timeoutFunctions.erase(f);
        }

        void validateNewTimeoutArgs(const juce::var::NativeFunctionArgs& args)
        {
            if(args.numArguments < 2)
            throw Error("setTimeout / setInterval require callback and interval arguments");
            if(!(*args.arguments).isMethod())
            throw Error("First argument to setTimeout / setInterval must be a callback");
            // readVarFromDukStack returns a juce::var double for DUK_TYPE_NUMBER
            // leave the rest to juce::var::operator int()
            // note this actually doesn't throw when NaN is passed in from js!
            if(!(*(args.arguments + 1)).isDouble())
            throw Error("Second argument to setTimeout / setInterval must be a number of milliseconds");
        }

        int reserveId()
        {
            // TODO something smarter with a release pool
            static int id = 0;
            return id++;
        }

        void freeId(int id)
        {
            // TODO if we decide to get smart with reserveId
            juce::ignoreUnused(id);
        }

        struct TimeoutFunction
        {
            TimeoutFunction(const juce::var::NativeFunctionArgs& _args, const bool _recurring=false)
            : f(_args.arguments->getNativeFunction()), recurring(_recurring)
            {
            args.reserve(_args.numArguments - 2);
            for(int i = 2; i < _args.numArguments; i++)
                args.push_back(*(_args.arguments + i));
            }

            bool call()
            {
            JUCE_ASSERT_MESSAGE_THREAD

            std::invoke(f, juce::var::NativeFunctionArgs(juce::var(), args.data(), static_cast<int>(args.size())));
            // return whether you want to run again
            return recurring;
            }

            private:
            bool recurring;
            juce::var::NativeFunction f;
            std::vector<juce::var> args;
        };

        std::map<int, TimeoutFunction> timeoutFunctions;
    };