import React, { PropsWithChildren } from "react";

import invariant from "invariant";

function ScrollViewContentView(props: any) {
  return React.createElement("ScrollViewContentView", props, props.children);
}

function parseScrollbarColorProp(scrollbarColorProp: string) {
  const values = scrollbarColorProp.split(" ");

  invariant(
    values.length === 2,
    'scrollbar-color should be a space separated string with two values: "{thumbColor} {trackColor}".'
  );

  return {
    "scrollbar-thumb-color": values[0],
    "scrollbar-track-color": values[1],
  };
}

function parseScrollbarWidthProp(scrollBarWidthProp: string | number) {
  invariant(
    typeof scrollBarWidthProp === "string" ||
      typeof scrollBarWidthProp === "number",
    "scrollbar-width should be a string or a number."
  );

  let props = {};

  if (typeof scrollBarWidthProp === "string") {
    switch (scrollBarWidthProp) {
      // JUCE's look and feel V2 uses 18px as default scrollbar thickness.
      case "auto":
        props["scrollbar-width"] = 18;
        return props;
      case "thin":
        props["scrollbar-width"] = 9;
        return props;
      case "none":
        props["overflow-x"] = "hidden";
        props["overflow-y"] = "hidden";
        props["scroll-on-drag"] = true;
        return props;
      default:
        invariant(
          false,
          "scrollbar-width as string should be one of 'auto', 'thin' or 'none'"
        );
    }
  }

  props["scrollbar-width"] = scrollBarWidthProp;

  return props;
}

function parseOverflowProp(
  overflowProp: string | undefined,
  overflowXProp: string | undefined,
  overflowYProp: string | undefined
) {
  let props = {};

  props["overflow-x"] = overflowXProp || overflowProp;
  props["overflow-y"] = overflowYProp || overflowProp;

  return props;
}

export interface ScrollEvent {
  scrollTop: number;
  scrollLeft: number;
}

export interface ScrollViewProps {
  overflow?: string;
  "overflow-x"?: string;
  "overflow-y"?: string;
  "scrollbar-color"?: string;
  "scrollbar-width"?: string | number;
  "scroll-on-drag"?: boolean;
  onScroll: (e: ScrollEvent) => void;
}

//TODO: Remove any once ViewProps typed
export function ScrollView(props: PropsWithChildren<ScrollViewProps | any>) {
  const child = React.Children.only(props.children);

  invariant(
    child && child["type"] === ScrollViewContentView,
    "ScrollView must have only one child, and that child must be a ScrollView.ContentView."
  );

  // Unpack non-native props
  let {
    overflow,
    "scrollbar-color": scrollbarColor,
    "scrollbar-width": scrollBarWidth,
    ...other
  } = props;

  if (typeof scrollbarColor !== "undefined")
    Object.assign(other, parseScrollbarColorProp(scrollbarColor));

  if (typeof scrollBarWidth !== "undefined")
    Object.assign(other, parseScrollbarWidthProp(scrollBarWidth));

  if (typeof overflow !== "undefined")
    Object.assign(
      other,
      parseOverflowProp(overflow, other["overflow-x"], other["overflow-y"])
    );

  return React.createElement("ScrollView", other, child);
}

ScrollView.ContentView = ScrollViewContentView;
