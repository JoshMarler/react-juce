import React, { memo } from "react";
import { Text, View, StyleSheet } from "react-juce";

const Label = ({ value, ...props }) => {
  return (
    <View {...props}>
      <Text styles={styles.labelText}>{value}</Text>
    </View>
  );
};

const styles = StyleSheet.create({
  labelText: {
    color: "#626262",
    fontSize: 16.0,
    lineSpacing: 1.6,
  },
});

export default memo(Label);
