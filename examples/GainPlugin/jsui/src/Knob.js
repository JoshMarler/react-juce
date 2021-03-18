import React from "react";
import ParameterSlider from "./ParameterSlider";
import { Slider } from "react-juce";
import Label from "./Label";
import { useParameter } from "./ParameterValueContext";
import { View } from "react-juce";

const sliderFillColor = "#66FDCF";
const sliderTrackColor = "#626262";
const drawRotary = Slider.drawRotary(sliderTrackColor, sliderFillColor);

const Knob = ({ paramId }) => {
  const { stringValue, currentValue } = useParameter(paramId);
  return (
    <View {...styles.container}>
      <ParameterSlider
        paramId={paramId}
        value={currentValue}
        onDraw={drawRotary}
        mapDragGestureToValue={Slider.rotaryGestureMap}
        {...styles.slider}
      />
      <Label value={stringValue} {...styles.label} />
    </View>
  );
};

const styles = {
  container: {
    minWidth: 100.0,
    minHeight: 100.0,
    width: "55%",
    height: "55%",
    marginTop: 15,
    marginBottom: 15,
    justifyContent: "center",
    alignItems: "center",
  },
  slider: {
    width: "100%",
    height: "100%",
  },
  label: {
    width: "100%",
    height: "100%",
    flex: 1.0,
    justifyContent: "center",
    alignItems: "center",
    interceptClickEvents: false,
    position: "absolute",
  },
};
export default Knob;
