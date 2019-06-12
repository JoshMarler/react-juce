import "juce-blueprint/cjs/lib/polyfill";

import React from 'react';
import Blueprint from 'juce-blueprint';
import App from './App';

// Blueprint.enableMethodTrace();
Blueprint.render(<App />, Blueprint.getRootContainer());
