import React, { memo } from "react";
import ParameterSlider from "./ParameterSlider";
import { Slider } from "react-juce";
import Label from "./Label";

const sliderFillColor = "#66FDCF";
const sliderTrackColor = "#626262";
const drawRotary = Slider.drawRotary(sliderTrackColor, sliderFillColor);

const Knob = ({ paramId }) => {
  return (
    <ParameterSlider
      paramId={paramId}
      onDraw={drawRotary}
      mapDragGestureToValue={Slider.rotaryGestureMap}
      {...styles.slider}
    >
      <Label paramId={paramId} {...styles.label} />
    </ParameterSlider>
  );
};

const styles = {
  slider: {
    minWidth: 100.0,
    minHeight: 100.0,
    width: "55%",
    height: "55%",
    marginTop: 15,
    marginBottom: 15,
    justifyContent: "center",
    alignItems: "center",
  },
  label: {
    flex: 1.0,
    justifyContent: "center",
    alignItems: "center",
    interceptClickEvents: false,
  },
};
export default memo(Knob);
