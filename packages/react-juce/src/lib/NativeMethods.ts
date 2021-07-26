let Native = global["__NativeBindings__"] || {};
let DefaultExport = Native;

declare var process: {
  env: {
    NODE_ENV: string;
  };
};

if (process.env.NODE_ENV !== "production") {
  // @ts-ignore
  DefaultExport = new Proxy(Native, {
    get: function (target, propKey, receiver) {
      if (
        target.hasOwnProperty(propKey) &&
        typeof target[propKey] === "function"
      ) {
        return function __NativeMethodWrapper__(...args: any): any {
          return target[propKey].call(null, ...args);
        };
      }

      return function __NativeMethodWrapper__() {
        console.warn(
          `WARNING: Attempt to access undefined native method ${target}`
        );
      };
    },
  });
}

export default DefaultExport;
