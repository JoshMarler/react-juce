//@ts-nocheck

var inspect = require("object-inspect");

/** An object to be used as an ES6 Proxy handler to trace method calls and
    undefined property accesses on the target object.
 */
export default {
  get(target, propKey, receiver) {
    const f = target[propKey];

    if (typeof f === "undefined") {
      console.log(
        "MethodTrace: Stubbing undefined property access for",
        propKey
      );

      return function _noop(...args) {
        console.log(
          "MethodTrace Stub:",
          propKey,
          ...args.map((arg) => {
            return inspect(arg, { depth: 1 });
          })
        );
      };
    }

    if (typeof f === "function") {
      return function _traced(...args) {
        console.log(
          "MethodTrace:",
          propKey,
          ...args.map((arg) => {
            return inspect(arg, { depth: 1 });
          })
        );

        return f.apply(this, args);
      };
    }

    return f;
  },
};
