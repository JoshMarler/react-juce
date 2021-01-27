import React from "react";

export function Image(props: any) {
  return React.createElement("Image", props, props.children);
}

Image.PlacementFlags = {
  xLeft: 1,
  xRight: 2,
  xMid: 4,
  yTop: 8,
  yBottom: 16,
  yMid: 32,
  stretchToFit: 64,
  fillDestination: 128,
  onlyReduceInSize: 256,
  onlyIncreaseInSize: 512,
  doNotResize: 256 | 512,
  centred: 4 + 32,
};
