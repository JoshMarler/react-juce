import React from 'react';

import invariant from 'invariant';

function ScrollViewContentView(props: any) {
  return React.createElement('ScrollViewContentView', props, props.children);
}

export function ScrollView(props: any) {
  const child = React.Children.only(props.children);

  invariant(
    child.type === ScrollViewContentView,
    'ScrollView must have only one child, and that child must be a ScrollView.ContentView.'
  );

  return React.createElement('ScrollView', props, child);
}

ScrollView.ContentView = ScrollViewContentView;