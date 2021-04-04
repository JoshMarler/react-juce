import React from "react";
import ReactJUCE from "react-juce";
import App from "./App";
import { ParameterValueProvider } from "./ParameterValueContext";

ReactJUCE.render(
  <ParameterValueProvider>
    <App />
  </ParameterValueProvider>,
  ReactJUCE.getRootContainer()
);
