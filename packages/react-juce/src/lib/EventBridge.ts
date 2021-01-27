import EventEmitter from "events";
import NativeMethods from "./NativeMethods";

const EventBridge = new EventEmitter.EventEmitter();
EventBridge.setMaxListeners(30);

// An internal hook for the native side, from which we propagate events through
// the EventEmitter interface.
// @ts-ignore
NativeMethods.dispatchEvent = function dispatchEvent(
  eventType: string,
  ...args: any
): void {
  EventBridge.emit(eventType, ...args);
};

export default EventBridge;
