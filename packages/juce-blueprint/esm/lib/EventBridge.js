import EventEmitter from 'events';

const EventBridge = new EventEmitter();

EventBridge.setMaxListeners(30);

EventBridge.__listeners = {};
EventBridge.__nextToken = 0;

// An internal hook for the native side, from which we propagate events through
// the EventEmitter interface.
__BlueprintNative__.dispatchEvent = function dispatchEvent(eventType, ...args) {
  EventBridge.emit(eventType, ...args);
}

__BlueprintNative__.subscribe = function(eventType, handler) {
    EventBridge.addListener(eventType, handler);

    const token = EventBridge.__nextToken++;
    EventBridge.__listeners[token] = {eventType, handler};
    return token;
};

__BlueprintNative__.unsubscribe = function subscribe(token) {
    if (!(token in EventBridge.__listeners)) {
      console.log("Error: Attempt to remove non-existent EventBridge listener. Token invalid.");
      return;
    }

    const { eventType, handler} = EventBridge.__listeners[token];
    EventBridge.removeListener(eventType, handler);
};

export default EventBridge;
