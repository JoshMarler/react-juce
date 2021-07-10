import AnimatedFlexBoxExample from "./AnimatedFlexBox";
import Meter from "./Meter";
import Knob from "./Knob";
import ParameterToggleButton from "./ParameterToggleButton";
import React, { Component, useState } from "react";
import { Canvas, Image, Text, View, StyleSheet } from "react-juce";

function animatedDraw(ctx) {
  let now = Date.now() / 10;
  let width = now % 100;
  let red = Math.sqrt(width / 100) * 255;
  let hex = Math.floor(red).toString(16);

  ctx.fillStyle = `#${hex}ffaa`;
  ctx.fillRect(0, 0, width, 2);
}

// Example of callback for image onLoad/onError
function imageLoaded() {
  console.log("Image is loaded!");
}

function imageError(error) {
  console.log(error.name);
  console.log(error.message);
}

const ImageType = {
    IMAGE_URL: "IMAGE_URL",
    IMAGE_PNG: "IMAGE_PNG",
    IMAGE_SVG: "IMAGE_SVG",
}

const MuteButton = () => {
    const [muted, setMuted] = useState(false);

    const muteButtonStyles = StyleSheet.cx(styles.mute_button, muted && styles.mute_button_active);
    const muteTextStyles = StyleSheet.cx(styles.mute_button_text, muted && styles.mute_button_text_active);

    return (
      <ParameterToggleButton
         paramId="MainMute"
         onToggled={setMuted}
         styles={muteButtonStyles}
       >
           <Text styles={muteTextStyles}>
           MUTE
         </Text>
       </ParameterToggleButton>
    )
}

const Logo = (props) => {
    let logo = "";

    switch(props.imageType) {
        case ImageType.IMAGE_URL:
            logo = "https://raw.githubusercontent.com/nick-thompson/react-juce/master/examples/GainPlugin/jsui/src/logo.png";
        case ImageType.IMAGE_PNG:
            logo = require("./logo.png");
        case ImageType.IMAGE_SVG:
        default:
            logo = require("./logo.svg");
    }

    return (
      <Image
          source={logo}
          onLoad={imageLoaded}
          onError={imageError}
          styles={styles.logo}
      />
    )
}


class App extends Component {
  constructor(props) {
    super(props);
  }

  render() {
    // Uncomment here to watch the animated flex box example in action
    //return (
    //  <View styles={styles.container}>
    //    <AnimatedFlexBoxExample />
    //  </View>
    //);

    return (
      <View styles={styles.container}>
          <View styles={styles.content}>
          <Logo />
          <Knob paramId="MainGain" />
          <Meter styles={styles.meter} />
          <Canvas styles={styles.canvas} animate={true} onDraw={animatedDraw} />
          <MuteButton />
        </View>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    width: "100%",
    height: "100%",
    backgroundColor:
      "linear-gradient(45deg, hsla(225, 15%, 11%, 0.3), #17191f 50%)",
    justifyContent: "center",
    alignItems: "center",
  },
  content: {
    flex: 1.0,
    flexDirection: "column",
    justifyContent: "space-around",
    alignItems: "center",
    padding: 24.0,
    maxWidth: 600,
    aspectRatio: 400.0 / 240.0,
  },
  logo: {
    flex: 0.0,
    width: "80%",
    aspectRatio: 281.6 / 35.0,
    placement: Image.PlacementFlags.centred,
  },
  meter: {
    flex: 0.0,
    width: 100.0,
    height: 16.0,
  },
  canvas: {
    flex: 0.0,
    width: 100.0,
    height: 2,
    marginTop: 10,
  },
  mute_button: {
    justifyContent: "center",
    alignItems: "center",
    backgroundColor: "#66FDCF",
    borderRadius: 5.0,
    borderWidth: 2.0,
    borderColor: "rgba(102, 253, 207, 1)",
    marginTop: 10,
    minWidth: 30.0,
    minHeight: 30.0,
    width: "20%",
    height: "17.5%",
  },
  mute_button_active: {
    backgroundColor: "hsla(162, 97%, 70%, 0)"
  },
  mute_button_text: {
    color: "#17191f",
    fontSize: 20.0,
    lineSpacing: 1.6,
    fontStyle: Text.FontStyleFlags.bold,
  },
  mute_button_text_active: {
      color: "hsla(162, 97%, 70%, 1)"
  },
});

export default App;
