import React, { memo, useCallback } from "react";
import { Slider } from "react-juce";
import {
  beginParameterChangeGesture,
  endParameterChangeGesture,
  setParameterValueNotifyingHost,
} from "./nativeMethods";

const ParameterSlider = ({ value, paramId, children, ...props }) => {
  const onMouseDown = (e) => {
    beginParameterChangeGesture(paramId);
  };

  const onMouseUp = (e) => {
    endParameterChangeGesture(paramId);
  };

  const onSliderValueChange = (value) => {
    setParameterValueNotifyingHost(paramId, value);
  };

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
