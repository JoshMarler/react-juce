import EventEmitter from 'events';

const EventBridge = new EventEmitter.EventEmitter();
EventBridge.setMaxListeners(30);

// An internal hook for the native side, from which we propagate events through
// the EventEmitter interface.
// @ts-ignore
__BlueprintNative__.dispatchEvent = function dispatchEvent(eventType: string, ...args: any): void {
  EventBridge.emit(eventType, ...args);
}

export default EventBridge;
