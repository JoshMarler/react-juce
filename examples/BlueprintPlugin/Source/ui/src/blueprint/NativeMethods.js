
export default {
  get(target, propKey, receiver) {
    // Everything gets assigned to this internal `__BlueprintNative__` object, so we proxy
    // to that
    if (__BlueprintNative__.hasOwnProperty(propKey) && typeof __BlueprintNative__[propKey] === 'function') {
      return function __nativeWrapper__(...args) {
        __BlueprintNative__[propKey].apply(null, args);
      }
    }

    throw new Error('Attempt to access an undefined NativeMethod.');
  }
};
