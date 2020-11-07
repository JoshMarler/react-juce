
//@ts-ignore
export default new Proxy(__BlueprintNative__, {
  get(target, propKey, receiver) {
    if (target.hasOwnProperty(propKey) && typeof target[propKey] === 'function') {
      return function __nativeWrapper__(...args: any): void {
        target[propKey].call(null, ...args);
      }
    }

    console.warn('WARNING: Attempt to access an undefined NativeMethod.');
    return function noop() {};
  }
});
