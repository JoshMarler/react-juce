import ColorString from "color-string";
import ColorNames from "color-name";

const COLOR_PROPERTIES = ["border-color", "background-color", "color"];

const isColorProperty = (propKey: string): boolean => {
  return COLOR_PROPERTIES.includes(propKey);
};

const colorStringToAlphaHex = (colorString: string): string => {
  //From Hex
  if (colorString.startsWith("#") && colorString.length === 7) {
    return `ff${colorString}`.replace("#", "");
  }
  //From RGB or RGBA
  else if (colorString.startsWith("rgb")) {
    const rgbValues = colorString
      .split("(")[1]
      .split(")")[0]
      .replace(/ /g, "")
      .split(",");
    const alphaValue =
      rgbValues.length === 4
        ? //@ts-ignore
          percentToHex(100 * rgbValues.pop())
        : "ff";
    const hex = rgbValues.map((color) => {
      const hex = Number.parseInt(color).toString(16);
      return hex.length == 1 ? "0" + hex : hex;
    });
    return alphaValue + hex.join("");
  }
  //From HSL or HSLA
  else if (colorString.startsWith("hsl")) {
    const hslValues = colorString
      .split("(")[1]
      .split(")")[0]
      .replace(/ |[%]/g, "")
      .split(",");
    const alphaValue =
      hslValues.length === 4
        ? //@ts-ignore
          percentToHex(100 * hslValues.pop())
        : "ff";
    const hex = hslToHex(
      Number.parseInt(hslValues[0]),
      Number.parseInt(hslValues[1]),
      Number.parseInt(hslValues[2])
    );
    return alphaValue + hex;
  }
  //From Linear Gradient
  else if (colorString.startsWith("linear-gradient")) {
    const lgValues = colorString
      .split(/\((.+)/)[1]
      .split(/\)$/)[0]
      .split(/,+(?![^\(]*\))/)
      .map((item) => {
        return item.split(" ");
      });
    //concat any arrays that contain hsl or rba strings
    lgValues.forEach((itemArr, idx) => {
      if (itemArr.find((item) => item.includes("(")) !== undefined) {
        const itemArrClone = itemArr.slice();
        const startIdx = itemArrClone.findIndex((item) => item.includes("("));
        const endIdx = itemArrClone.findIndex((item) => item.includes(")"));
        const colorHSLRGB = itemArrClone.slice(startIdx, endIdx + 1).join("");
        itemArr.splice(startIdx, endIdx + 1, colorHSLRGB);
        lgValues[idx] = itemArr;
      } else {
        lgValues[idx] = itemArr;
      }
    });
    //flatten array
    const lgValuesCleaned: Array<String> = [].concat
      // @ts-ignore
      .apply([], lgValues)
      .filter((item) => {
        return item != "";
      });
    //converts any colorStrings in linear gradient to alpha-hex
    let skipCurrentIdx = -1;
    const lgValuesConverted = lgValuesCleaned
      .map((value, idx) => {
        if (skipCurrentIdx == idx) return;
        // @ts-ignore
        let convertedColorString = colorStringToAlphaHex(value);
        //check if the next element is a percent and attach to current and remove it
        const nextIdx = idx + 1;
        if (
          lgValuesCleaned[nextIdx] !== undefined &&
          lgValuesCleaned[nextIdx].includes("%") &&
          !lgValuesCleaned[nextIdx].includes("(")
        ) {
          convertedColorString += lgValuesCleaned[nextIdx];
          skipCurrentIdx = nextIdx;
        }
        return convertedColorString;
      })
      .filter((item) => {
        return item !== undefined;
      });
    return "linear-gradient(" + lgValuesConverted.join(",") + ")";
  }
  //From Named Colours
  else if (Object.keys(ColorNames).includes(colorString)) {
    const rgbValues = ColorString.get.rgb(colorString);
    const hex = ColorString.to.hex(rgbValues);
    return colorStringToAlphaHex(hex);
  } else {
    return colorString;
  }
};

const convertLinearGradientStringToNativeObject = (
  lgColorStringHex: string
): object => {
  const linearGradientNativeObject = {};
  const lgValues = lgColorStringHex
    .split(/\((.+)/)[1]
    .split(/\)$/)[0]
    .replace(/ /g, "")
    .split(/,+(?![^\(]*\))/);
  //Check for directional words
  let angle: number = 0;
  let rotation: string = lgValues[0];
  if (rotation == "toleft") angle = 270;
  if (rotation == "toright") angle = 90;
  if (rotation == "tobottom") angle = 180;
  if (rotation == "totop") angle = 0;

  //These are not the exact match to CSS as boundary dimensions are required for calculation
  if (rotation == "totopleft" || rotation == "tolefttop") angle = 295;
  if (rotation == "totopright" || rotation == "torighttop") angle = 45;
  if (rotation == "tobottomright" || rotation == "torightbottom") angle = 115;
  if (rotation == "tobottomleft" || rotation == "toleftbottom") angle = 205;

  //Check for Turns
  if (rotation.includes("turn")) {
    rotation = rotation.replace("turn", "");
    const turns: number = parseFloat(rotation);
    angle = 360 * turns;
  }
  //Check for Degrees
  if (rotation.includes("deg")) {
    rotation = rotation.replace("deg", "");
    angle = parseInt(rotation);
  }

  linearGradientNativeObject["angle"] = angle;
  linearGradientNativeObject["colours"] = [];
  const colorPositions: Array<String> = lgValues.slice(1);

  colorPositions.forEach((colorPos, idx) => {
    const hexPosObj = { id: idx };
    const hex = colorPos.slice(0, 8);
    //check string is an alpha hex
    const isValidAlphaHex =
      // @ts-ignore
      hex.match(
        /^[0-9a-fA-F]{8}$|#[0-9a-fA-F]{6}$|#[0-9a-fA-F]{4}$|#[0-9a-fA-F]{3}$/
      ).length > 0;
    if (!isValidAlphaHex) return;
    colorPos = colorPos.replace(hex, "");
    if (colorPos.includes("%")) {
      colorPos = colorPos.replace("%", "");
      // @ts-ignore
      let colorPercent = parseInt(colorPos) / 100;
      //check if previous percent is greater or equal to the current
      if (linearGradientNativeObject["colours"].length > 0) {
        const previousPos =
          linearGradientNativeObject["colours"][idx - 1]["position"];
        if (previousPos >= colorPercent) colorPercent = previousPos + 0.001;
      }
      hexPosObj["position"] = colorPercent;
    } else {
      if (idx == 0) {
        hexPosObj["position"] = 0.0;
      } else if (idx == colorPositions.length - 1) {
        hexPosObj["position"] = 1.0;
      }
      //assign null and iterate over again later
      else {
        hexPosObj["position"] = undefined;
      }
    }
    hexPosObj["hex"] = hex;
    linearGradientNativeObject["colours"].push(hexPosObj);
  });
  //Find half way between previous and next given percent if no percentage assigned
  linearGradientNativeObject["colours"].forEach((colorPos, idx) => {
    if (colorPos["position"] === undefined) {
      //find next color with percentage
      const colorsClone = JSON.parse(
        JSON.stringify(linearGradientNativeObject["colours"])
      );
      let currentArrayChunk = colorsClone.splice(idx);
      const nextColorPercent = currentArrayChunk.find(
        (colorPos) => colorPos["position"] != undefined && colorPos["id"] !== 0
      );
      const nextColorPercentIdx = currentArrayChunk.findIndex(
        (colorPos) => colorPos["position"] != undefined && colorPos["id"] !== 0
      );
      currentArrayChunk = currentArrayChunk.splice(0, nextColorPercentIdx + 1);
      const previousColorPercent = colorsClone[idx - 1];
      const y = nextColorPercent["position"];
      const x = previousColorPercent["position"];
      const n = currentArrayChunk.length + 1;
      //assign evenly distributed values to each undefined color in current array chunk
      currentArrayChunk.forEach((colorPosChunk, idx) => {
        idx += 1;
        if (colorPosChunk["position"] === undefined) {
          const colorPosition = x + ((y - x) / (n - 1)) * idx;
          const colorPosObj =
            linearGradientNativeObject["colours"][colorPosChunk["id"]];
          colorPosObj["position"] = colorPosition;
        }
      });
    }
  });
  return linearGradientNativeObject;
};

const hslToHex = (h: number, s: number, l: number): string => {
  l /= 100;
  const a = (s * Math.min(l, 1 - l)) / 100;
  const f = (n) => {
    const k = (n + h / 30) % 12;
    const color = l - a * Math.max(Math.min(k - 3, 9 - k, 1), -1);
    return Math.round(255 * color)
      .toString(16)
      .padStart(2, "0");
  };
  return `${f(0)}${f(8)}${f(4)}`;
};

const percentToHex = (p: number): string => {
  p = p > 100 ? 100 : p;
  const intValue = Math.round((p / 100) * 255);
  const hexValue = intValue.toString(16);
  return hexValue.padStart(2, "0").toLowerCase();
};

export default {
  isColorProperty,
  colorStringToAlphaHex,
  convertLinearGradientStringToNativeObject,
};
