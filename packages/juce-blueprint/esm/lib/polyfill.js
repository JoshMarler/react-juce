/** Polyfill ES2015 data structures with core-js. */
import 'core-js/es6/set';
import 'core-js/es6/map';

/** The EventLoop manages all outstanding timers, invoking callbacks and
 *  clearing the registry in response to a regular interrupt from the JUCE
 *  backend.
 *
 *  This is not a proper event loop; just a thin wrapper for polyfilling
 *  timer methods in the global scope.
 */
class EventLoop {
  constructor() {
    this._timers = {};
    this._nextId = 0;
  }

  setTimeout(fn, time, a, b, c, d, e, f) {
    let id = this._nextId++;
    this._timers[id] = {
      f: fn,
      eventTime: performance.now() + time,
      vargs: [a, b, c, d, e, f],
    };
  }

  clearTimeout(id) {
    if (this._timers.hasOwnProperty(id)) {
      delete this._timers[id];
    }
  }

  tick() {
    for (let key in this._timers) {
      if (this._timers.hasOwnProperty(key)) {
        let timer = this._timers[key];
        let eventTime = timer.eventTime;

        if (performance.now() >= eventTime) {
          timer.f.apply(null, timer.vargs);
          delete this._timers[key];
        }
      }
    }
  }
}

/** Our polyfills all operate over a single event loop instance. */
const el = new EventLoop();

/** This is the native hook that the juce backend looks to call. */
function __schedulerInterrupt__() {
  el.tick();
}

/** A simple global setTimeout wrapper around the event loop instance. */
function __setTimeout__ (a, b, c, d, e, f, g) {
  return el.setTimeout(a, b, c, d, e, f, g);
}

/** A global setInterval implementation which falls back to setTimeout. */
function __setInterval__ (cb, wait, c, d, e, f, g) {
  // TODO: This isn't quite correct because we'll generate a new timer id
  // on each iteration and the id returned to the caller will become invalid
  // by the second iteration.
  return el.setTimeout(function() {
    cb.call(null, c, d, e, f, g);
    el.setTimeout(cb, wait, c, d, e, f, g);
  }, wait);
}


/** Attach our polyfills */
global.__schedulerInterrupt__ = __schedulerInterrupt__;
global.setTimeout = __setTimeout__;
global.setInterval = __setInterval__;
