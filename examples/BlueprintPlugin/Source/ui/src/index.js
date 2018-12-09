import "@babel/polyfill";

import React from 'react';
import Blueprint from './Blueprint';
import ReactDOM from 'react-dom';
import App from './App';

Blueprint.render(<App />, document.getElementById('root'));
