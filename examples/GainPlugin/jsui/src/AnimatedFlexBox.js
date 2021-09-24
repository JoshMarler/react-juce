import React, { Component } from "react";
import { View, Text, StyleSheet } from "react-juce";

const Cell = (props) => (
    <View styles={styles.cell}>
      <Text styles={styles.text}>{props.text}</Text>
    </View>
)

class AnimatedFlexBoxExample extends Component {
  render() {
    return (
      <View styles={styles.container}>
        <Cell text="Look at me, cell #1!" />
        <Cell text="Look at me, cell #2!" />
        <Cell text="Look at me, cell #3!" />
        <Cell text="Look at me, cell #4!" />
        <Cell text="Look at me, cell #5!" />
        <Cell text="Look at me, cell #6!" />
        <Cell text="Look at me, cell #7!" />
        <Cell text="Look at me, cell #8!" />
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    width: "100%",
    height: "100%",
    backgroundColor: "#17191f",
    justifyContent: "flex-start",
    alignContent: "flex-start",
    flexWrap: "wrap",
    layoutAnimated: {
      duration: 200.0,
      frameRate: 45,
      easing: View.EasingFunctions.quadraticInOut,
    },
  },
  cell: {
    flex: 0.0,
    width: 100.0,
    height: 100.0,
    justifyContent: "space-around",
    alignItems: "center",
    backgroundColor: "#87898f",
    margin: 6.0,
    padding: 6.0,
  },
  text: {
    fontSize: 16.0,
    lineSpacing: 1.6,
    justification: Text.JustificationFlags.centred,
  },
});

export default AnimatedFlexBoxExample;
