import React, { PropsWithChildren } from "react";

export interface InputEvent {
  value: string;
}

export interface ChangeEvent {
  value: string;
}

export interface TextInputProps {
  value?: string;

  color?: string;
  fontSize?: number;
  fontStyle?: number;
  fontFamily?: string;
  justification?: number;
  "kerning-factor"?: number;

  placeholder?: string;
  "placeholder-color"?: string;
  maxlength?: number;
  readonly?: boolean;
  "outline-color"?: string;
  "focused-outline-color"?: string;
  "highlighted-text-color"?: string;
  "highlight-color"?: string;
  "caret-color"?: string;

  onChange?: (e: ChangeEvent) => void;
  onInput?: (e: InputEvent) => void;
}

export function TextInput(props: PropsWithChildren<TextInputProps | any>) {
  return React.createElement("TextInput", props, props.children);
}
