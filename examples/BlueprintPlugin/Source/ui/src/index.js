import "@babel/polyfill";

import React from 'react';
import Blueprint from './Blueprint';
import App from './App';

// Blueprint.enableMethodTrace();
Blueprint.render(<App />, Blueprint.getRootContainer());
