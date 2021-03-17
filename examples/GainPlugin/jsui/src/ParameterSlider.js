import React, { memo, useEffect, useCallback, useState } from "react";
import { EventBridge, Slider } from "react-juce";

const ParameterSlider = ({ paramId, children, ...props }) => {
  const [value, setValue] = useState(0.0);

  const onParameterValueChange = useCallback(
    (index, changedParamId, defaultValue, currentValue) => {
      if (changedParamId === paramId) {
        setValue(currentValue);
      }
    },
    [setValue]
  );

  useEffect(() => {
    EventBridge.addListener("parameterValueChange", onParameterValueChange);
    return () => {
      EventBridge.removeListener(
        "parameterValueChange",
        onParameterValueChange
      );
    };
  }, [onParameterValueChange]);

  const onMouseDown = useCallback(
    (e) => {
      global.beginParameterChangeGesture(paramId);
    },
    [paramId]
  );

  const onMouseUp = useCallback(
    (e) => {
      global.endParameterChangeGesture(paramId);
    },
    [paramId]
  );

  const onSliderValueChange = useCallback(
    (value) => {
      global.setParameterValueNotifyingHost(paramId, value);
    },
    [paramId]
  );

  return (
    <Slider
      {...props}
      value={value}
      onMouseDown={onMouseDown}
      onMouseUp={onMouseUp}
      onChange={onSliderValueChange}
    >
      {children}
    </Slider>
  );
};

// TODO: PropTypes Validation
// paramId should be required and has type of string

export default memo(ParameterSlider);
