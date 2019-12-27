export default class Backoff {
    /**
     * Create a backoff instance can automatically backoff retries.
     *
     * @param {Number} min
     * @param {Number} [max]
     * @param {Boolean} [jitter]
     */
    constructor(min = 500, max = null, jitter = true) {
        this.min = min;
        this.max = max !== null ? max : min * 10;
        this.jitter = jitter;

        this._current = min;
        this._timeoutId = null;
        this._fails = 0;
    }

    /**
     * Return the number of failures.
     *
     * @return {Number}
     */
    get fails() {
        return this._fails;
    }

    /**
     * Current backoff value in milliseconds.
     *
     * @return {Number}
     */
    get current() {
        return this._current;
    }

    /**
     * A callback is going to fire.
     *
     * @return {Boolean}
     */
    get pending() {
        return this._timeoutId !== null;
    }

    /**
     * Clear any pending callbacks and reset the backoff.
     */
    succeed() {
        this.cancel();
        this._fails = 0;
        this._current = this.min;
    }

    /**
     * Increment the backoff and schedule a callback if provided.
     *
     * @param {Function} callback
     * @return {Number}
     */
    fail(callback = null) {
        this._fails += 1;
        let delay = this._current * 2;
        if (this.jitter) {
            delay *= Math.random();
        }
        this._current = Math.min(this._current + delay, this.max);
        if (callback !== null) {
            if (this._timeoutId !== null) {
                throw new Error('callback already pending');
            }
            this._timeoutId = setTimeout(() => {
                try {
                    callback();
                } finally {
                    this._timeoutId = null;
                }
            }, this._current);
        }
        return this._current;
    }

    /**
     *  Clear any pending callbacks.
     */
    cancel() {
        if (this._timeoutId !== null) {
            clearTimeout(this._timeoutId);
            this._timeoutId = null;
        }
    }
}
