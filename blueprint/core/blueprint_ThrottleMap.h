/*
  ==============================================================================

    blueprint_ThrottleMap.h
    Created: 6 Oct 2019 10:10:23am

  ==============================================================================
*/

#pragma once


namespace blueprint
{

    //==============================================================================
    /** A small utility that throttles invocations to a given callback based on a given
        key. Particularly useful for propagating parameter updates into the Duktape
        environment at a controlled rate.
     */
    class ThrottleMap
    {
    public:
        //==============================================================================
        ThrottleMap() = default;

        //==============================================================================
        template <typename Fn>
        void throttle (int key, double throttleWindowMs, Fn callback)
        {
            // We need to be able to invoke the callback..
            // C++17 only: static_assert(std::is_invocable<Fn>::value);

            double timeNow = juce::Time::getMillisecondCounterHiRes();

            if (tMap.count(key) == 0)
            {
                // TODO: Should this automatically invoke on the message thread?
                tMap[key] = timeNow;
                callback();
            }
            else
            {
                double lastDispatchTime = tMap[key];
                double diff = timeNow - lastDispatchTime;

                if (diff >= throttleWindowMs)
                {
                    tMap[key] = timeNow;
                    callback();
                }
            }
        }

    private:
        //==============================================================================
        std::map<int, double> tMap;

        //==============================================================================
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThrottleMap)
    };

}
