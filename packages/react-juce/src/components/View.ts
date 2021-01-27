import React from "react";

// We'll need to wrap the default native components in stuff like this so that
// you can use <View> in your JSX. Otherwise we need the dynamic friendliness
// of the createElement call (note that the type is a string...);
export function View(props: any) {
  return React.createElement("View", props, props.children);
}

View.ClickEventFlags = {
  disableClickEvents: 0,
  allowClickEvents: 1,
  allowClickEventsExcludingChildren: 2,
  allowClickEventsOnlyOnChildren: 3,
};

View.EasingFunctions = {
  linear: 0,
  quadraticIn: 1,
  quadraticOut: 2,
  quadraticInOut: 3,
};
