import { TMacroCall } from "./types";

// don't try too hard, we don't need to support qouted strings containing commas etc.
export const splitArgs = (a: string) => {
  const argArray = a.trim().split(/\s*,\s*/);
  if (argArray.length === 1 && argArray[0] === "") return [];
  return argArray;
};

export const getMacroCalls = (s: string): TMacroCall[] => {
  const macroMatcher = String.raw`([a-zA-Z0-9]+)\(([^)]*)\)`;
  let r = new RegExp(macroMatcher, "g");
  // the rest would be trivial with matchAll
  // which duktape sadly lacks
  const matches = s.match(r);
  if (!matches) return [];
  // unset "g" flag, otherwise r.exec returns null when multiple matches occurred
  r = new RegExp(macroMatcher);
  const macroCalls = [];
  for (const match of matches) {
    // @ts-ignore
    const [, macro, args] = r.exec(match);
    macroCalls.push({
      // @ts-ignore
      macro,
      // @ts-ignore
      args: splitArgs(args),
    });
  }
  return macroCalls;
};
