import NativeMethods from './NativeMethods';

import { all as allCssProps } from "known-css-properties";
import camelCase from "camelcase";

import Colors from "./MacroProperties/Colors";
import { macroPropertyGetters } from "./MacroProperties";

//TODO: Move various style interfaces out into their own files to break this up.
//TODO: Support hyphenated string style props?
//TODO: Add TextInputViewStyles ?

/**
 *
 */
interface BorderStyles {
    borderColor?: string;
    borderPath?: string;
    borderRadius?: string;
    borderWidth?: number;
}

/**
 *
 */
interface BackgroundStyles {
    backgroundColor?: string
}

/**
 *
 */
interface FontStyles {
    color?: string,
    fontSize?: number,
    fontStyle?: string,
    fontFamily?: string,
    justification?: number, //TODO: How to actually type this? Use enum?
    kerningFactor?: string,
    lineSpacing?: number,
    wordWrap?: number, //TODO: How to actually type this? Use enum?
}

type FlexBoxAlignmentValue = "auto" | "baseline" | "center" | "flex-start" | "flex-end" | "space-around" | "space-between" | "stretch";

/**
 *
 */
//TODO: Add margin props
//TODO: Add padding props
interface FlexboxStyles {
    alignContent?: FlexBoxAlignmentValue,
    alignItems?: FlexBoxAlignmentValue,
    alignSelf?: FlexBoxAlignmentValue,
    aspectRatio?: number,
    bottom?: number | string,
    direction?: "ltr" | "rtl" | "inherit" ,
    flex?: number,
    flexBasis?: number | "auto",
    flexDirection?: "column" | "row" | "column-reverse" | "row-reverse",
    flexGrow?: number,
    flexShrink?: number,
    flexWrap?: "nowrap" | "wrap" | "wrap-reverse",
    height?: number | string,
    justifyContent?: "flex-start" | "flex-end" | "center" | "space-between" | "space-around",
    left?: number | string,
    maxHeight?: number | string,
    maxWidth?: number | string,
    minHeight?: number | string,
    minWidth?: number | string,
    overflow?: "visible" | "hidden" | "scroll",
    position?: "absolute" | "relative",
    right?: number | string,
    top?: number | string,
    width?: number | string,
}

/**
 *
 */
interface ImageStyles {
    placement?: number //TODO: How to actually type this? Use enum?
}

// TODO: How to add LayoutAnimated types. Do they belong in StyleSheet
//       or should they be an additional prop on `View` ?
interface LayoutAnimatedProps {
    duration?: number,
    frameRate?: number,
    easing?: number, //TODO: How to actually type this? Use enum?
}

interface LayoutAnimatedStyles {
    layoutAnimated?: LayoutAnimatedProps;
}


/**
 *
 */
interface ScrollBarStyles {
    overflowX?: string, // TODO: Add possible strings to type
    overflowY?: string, // TODO: Add possible strings to type
    scrollBarColor?: string,
    scrollBarWidth?: string | number, // TODO: Add possible strings to type
    scrollBarTrackColor?: string,
}

interface StyleSheetObj extends BackgroundStyles, BorderStyles, FlexboxStyles, FontStyles, ImageStyles { }

/**
 *
 */
export interface StyleSheet {
    [styleObj: string]: StyleSheetObj
}

/**
 *
 */
export interface StyleSheetIdentifier {

}

export type StyleSheetIdentifierSwitch = StyleSheetIdentifier | boolean;


// Get any css properties not beginning with a "-",
// and build a map from any camelCase versions to
// the hyphenated version.
const cssPropsMap = allCssProps
  .filter((s) => !s.startsWith("-") && s.includes("-"))
  .reduce((acc, v) => Object.assign(acc, { [camelCase(v)]: v }), {});

export default {
    create(stylesObj: StyleSheet): StyleSheetIdentifier {
        let nativeStyles: StyleSheet = {};

        for (const subObj in stylesObj) {
            let nativeSubObj: StyleSheetObj = {};

            for (let propKey in stylesObj[subObj]) {
                let value = stylesObj[subObj][propKey];

                // If the supplied propkey is a camelCase equivalent
                // of a css prop, first convert it to kebab-case.
                propKey = cssPropsMap[propKey] || propKey;

                // Convert provided color string to alpha-hex code for JUCE.
                if (Colors.isColorProperty(propKey)) {
                  value = Colors.colorStringToAlphaHex(value);
                  if (value.startsWith("linear-gradient")) {
                    value = Colors.convertLinearGradientStringToNativeObject(value);
                  }
                }
                else if (macroPropertyGetters.hasOwnProperty(propKey)) {
                    for (const [k, v] of macroPropertyGetters[propKey](value)) {
                        nativeSubObj[k] = v;
                    }
                }

                nativeSubObj[propKey] = value;
            }

            nativeStyles[subObj] = nativeSubObj;
        }

        //@ts-ignore
        return NativeMethods.createStyleSheet(nativeStyles);
    },
    cx(...args: StyleSheetIdentifierSwitch[]) {
        return args.map((a: StyleSheetIdentifierSwitch) => {
            if (a !== false)
                return a;
        })
    }
}
