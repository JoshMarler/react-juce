import NativeMethods from './NativeMethods';

import { all as allCssProps } from "known-css-properties";
import camelCase from "camelcase";

import Colors from "./MacroProperties/Colors";
import { macroPropertyGetters } from "./MacroProperties";

// Get any css properties not beginning with a "-",
// and build a map from any camelCase versions to
// the hyphenated version.
const cssPropsMap = allCssProps
  .filter((s) => !s.startsWith("-") && s.includes("-"))
  .reduce((acc, v) => Object.assign(acc, { [camelCase(v)]: v }), {});

//TODO: Add interface types
export default {
    create(stylesObj) {
        // TODO: Will be another instance of our
        //       styles object type.
        let nativeStyles = {};

        for (const subObj in stylesObj) {
            let nativeSubObj = {};

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
                  return;
                }

                nativeSubObj[propKey] = value;
            }

            nativeStyles[subObj] = nativeSubObj;
        }

        //@ts-ignore
        return NativeMethods.createStyleSheet(nativeStyles);
    },
    cx(...args) {
        return args.map((a) => {
            if (a !== false)
                return a;
        })
    }
}
