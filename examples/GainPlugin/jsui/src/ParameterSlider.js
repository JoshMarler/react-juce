import React, { memo, useCallback } from "react";
import { Slider } from "react-juce";
import {
  beginParameterChangeGesture,
  endParameterChangeGesture,
  setParameterValueNotifyingHost,
} from "./nativeMethods";

const ParameterSlider = ({ value, paramId, children, ...props }) => {
  console.log("Slider rendered");
  const onMouseDown = useCallback(
    (e) => {
      beginParameterChangeGesture(paramId);
    },
    [paramId]
  );

  const onMouseUp = useCallback(
    (e) => {
      endParameterChangeGesture(paramId);
    },
    [paramId]
  );

  const onSliderValueChange = useCallback(
    (value) => {
      setParameterValueNotifyingHost(paramId, value);
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
// https://www.npmjs.com/package/prop-types

export default memo(ParameterSlider);
