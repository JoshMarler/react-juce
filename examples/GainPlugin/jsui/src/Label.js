import React, { memo } from "react";
import { Text, View } from "react-juce";

const Label = ({ value, ...props }) => {
  return (
    <View {...props}>
      <Text {...styles.labelText}>{value}</Text>
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
