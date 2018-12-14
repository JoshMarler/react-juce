import "@babel/polyfill";

import React from 'react';
import Blueprint from './Blueprint';
import App from './App';

Blueprint.render(<App />, Blueprint.getRootContainer());
