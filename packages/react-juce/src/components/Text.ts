import React from "react";

export function Text(props: any) {
  return React.createElement("Text", props, props.children);
}

Text.WordWrap = {
  none: 0,
  byWord: 1,
  byChar: 2,
};

Text.FontStyleFlags = {
  plain: 0,
  bold: 1,
  italic: 2,
  underlined: 4,
};

Text.JustificationFlags = {
  left: 1,
  right: 2,
  horizontallyCentred: 4,
  top: 8,
  bottom: 16,
  verticallyCentred: 32,
  horizontallyJustified: 64,
  centred: 36,
  centredLeft: 33,
  centredRight: 34,
  centredTop: 12,
  centredBottom: 20,
  topLeft: 9,
  topRight: 10,
  bottomLeft: 17,
  bottomRight: 18,
};
