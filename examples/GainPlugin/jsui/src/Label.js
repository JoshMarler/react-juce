import React, { useEffect, useState, useCallback, memo } from "react";
import { EventBridge, Text, View } from "react-juce";

const Label = ({ paramId, ...props }) => {
  const [text, setText] = useState("");

  const onParameterValueChange = useCallback(
    (index, changedParamId, defaultValue, currentValue, stringValue) => {
      if (changedParamId === paramId) {
        setText(stringValue);
      }
    },
    [setText]
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

  return (
    <View {...props}>
      <Text {...styles.labelText}>{text}</Text>
    </View>
  );
};

const styles = {
  labelText: {
    color: "#626262",
    fontSize: 16.0,
    lineSpacing: 1.6,
  },
};

export default memo(Label);
