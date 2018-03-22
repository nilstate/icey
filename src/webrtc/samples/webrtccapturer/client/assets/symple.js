// -----------------------------------------------------------------------------
// Symple JavaScript Client
//
var Symple = {
    // Version
    VERSION: "0.9.0",

    // Return an array of nested objects matching
    // the given key/value strings.
    filterObject: function(obj, key, value) { // (Object[, String, String])
        var r = []
        for (var k in obj) {
            if (obj.hasOwnProperty(k)) {
                var v = obj[k];
                if ((!key || k == key) &&
                    (!value || v == value)) {
                    r.push(obj)
                }
                else if (typeof v === 'object') {
                    var a = Symple.filterObject(v, key, value);
                    if (a) r = r.concat(a);
                }
            }
        }
        return r;
    },
        
    // Delete nested objects with properties
    // that match the given key/value strings.
    deleteNested: function(obj, key, value) { // (Object[, String, String])
        for (var k in obj) {
            var v = obj[k];
            if ((!key || k == key) &&
                (!value || v == value)) {
                delete obj[k];
            }
            else if (typeof v === 'object')
                 Symple.deleteNested(v, key);
        }
    },
    
    // Count nested object properties which
    // match the given key/value strings.
    countNested: function(obj, key, value, count) {
        if (count === undefined) count = 0;
        for (var k in obj) {
            if (obj.hasOwnProperty(k)) {
                var v = obj[k];
                if ((!key || k == key) &&
                    (!value || v == value)) {
                    count++;
                }
                else if (typeof(v) === 'object') {
                //else if (v instanceof Object) {
                    count = Symple.countNested(v, key, value, count);
                }
            }
        }
        return count;
    },
    
    // Traverse an objects nested properties
    traverse: function(obj, fn) { // (Object, Function)
        for (var k in obj) {
            if (obj.hasOwnProperty(k)) {
                var v = obj[k];
                fn(k, v);
                if (typeof v === 'object')
                    Symple.traverse(v, fn);
            }
        }
    },
    
    // Generate a random string
    randomString: function(n) {
        return Math.random().toString(36).slice(2) //Math.random().toString(36).substring(n || 7);
    },
    
    // Recursively merge object properties of r into l
    merge: function(l, r) { // (Object, Object)
        for (var p in r) {
            try {
                // Property in destination object set; update its value.
                //if (typeof r[p] == "object") {
                if (r[p].constructor == Object) {
                    l[p] = merge(l[p], r[p]);
                } else {
                    l[p] = r[p];
                }
            } catch(e) {
                // Property in destination object not set; 
                // create it and set its value.
                l[p] = r[p];
            }
        }
        return l;
    },
    
    // Object extend functionality
    extend: function() {   
        var process = function(destination, source) {   
            for (var key in source) {
                if (hasOwnProperty.call(source, key)) {
                    destination[key] = source[key];
                }
            }
            return destination;
        };
        var result = arguments[0];
        for(var i=1; i<arguments.length; i++) {
            result = process(result, arguments[i]);
        }
        return result;
    },
    
    // Run a vendor prefixed method from W3C standard method.
    runVendorMethod: function(obj, method) {      
        var p = 0, m, t, pfx = ["webkit", "moz", "ms", "o", ""];
        while (p < pfx.length && !obj[m]) {
            m = method;
            if (pfx[p] == "") {
                m = m.substr(0,1).toLowerCase() + m.substr(1);
            }
            m = pfx[p] + m;
            t = typeof obj[m];
            if (t != "undefined") {
                pfx = [pfx[p]];
                return (t == "function" ? obj[m]() : obj[m]);
            }
            p++;
        }
    },
    
    //
    // Date parseing for ISO 8601
    // Based on https://github.com/csnover/js-iso8601
    //
    // Parses dates like:
    // 2001-02-03T04:05:06.007+06:30
    // 2001-02-03T04:05:06.007Z
    // 2001-02-03T04:05:06Z
    //
    parseISODate: function (date) { // (String)
        
        // ISO8601 dates were introduced with ECMAScript v5, 
        // try to parse it natively first...
        var timestamp = Date.parse(date)
        if (isNaN(timestamp)) {
            var struct,
                minutesOffset = 0,
                numericKeys = [ 1, 4, 5, 6, 7, 10, 11 ];

            // ES5 §15.9.4.2 states that the string should attempt to be parsed as a Date
            // Time String Format string before falling back to any implementation-specific
            // date parsing, so that's what we do, even if native implementations could be faster
            //
            //              1 YYYY                2 MM       3 DD           4 HH    5 mm       6 ss        7 msec        8 Z 9 ±    10 tzHH    11 tzmm
            if ((struct = /^(\d{4}|[+\-]\d{6})(?:-(\d{2})(?:-(\d{2}))?)?(?:T(\d{2}):(\d{2})(?::(\d{2})(?:\.(\d{3}))?)?(?:(Z)|([+\-])(\d{2})(?::(\d{2}))?)?)?$/.exec(date))) {
                // Avoid NaN timestamps caused by "undefined" values being passed to Date.UTC
                for (var i = 0, k; (k = numericKeys[i]); ++i)
                    struct[k] = +struct[k] || 0;

                // Allow undefined days and months
                struct[2] = (+struct[2] || 1) - 1;
                struct[3] = +struct[3] || 1;

                if (struct[8] !== 'Z' && struct[9] !== undefined) {
                    minutesOffset = struct[10] * 60 + struct[11];
                    if (struct[9] === '+')
                        minutesOffset = 0 - minutesOffset;
                }

                timestamp = Date.UTC(struct[1], struct[2], struct[3], struct[4], struct[5] + minutesOffset, struct[6], struct[7]);
            }
        }

        return new Date(timestamp);
    },
    
    isMobileDevice: function() {
        return 'ontouchstart' in document.documentElement;
    },    
    
    // Returns the current iOS version, or false if not iOS
    iOSVersion: function(l, r) {
        return parseFloat(('' + (/CPU.*OS ([0-9_]{1,5})|(CPU like).*AppleWebKit.*Mobile/i.exec(navigator.userAgent) || [0,''])[1])
            .replace('undefined', '3_2').replace('_', '.').replace('_', '')) || false;
    },
    
    // Match the object properties of l with r
    match: function(l, r) { // (Object, Object)
        var res = true;
        for (var prop in l) {
            if (!l.hasOwnProperty(prop) ||
                !r.hasOwnProperty(prop) ||
                r[prop] != l[prop]) {
                res = false;
                break;
            }
        }
        return res
    },
    
    formatTime: function(date) {        
        function pad(n) { return n < 10 ? ('0' + n) : n }
        return pad(date.getHours()).toString() + ':' +
            pad(date.getMinutes()).toString() + ':' +
            pad(date.getSeconds()).toString() + ' ' +
            pad(date.getDate()).toString() + '/' +
            pad(date.getMonth()).toString();
    },
    
    // Debug logger
    log: function () {
        if (typeof console != "undefined" && 
            typeof console.log != "undefined") {
            console.log.apply(console, arguments);
        }
    }
};


// -----------------------------------------------------------------------------
// Symple OOP Base Class
//    
(function(Symple) {
    var initializing = false, 
        fnTest = /xyz/.test(function(){xyz;}) ? /\b_super\b/ : /.*/;
   
    // The base Class implementation (does nothing)
    Symple.Class = function(){};
   
    // Create a new Class that inherits from this class
    Symple.Class.extend = function(prop) {
        var _super = this.prototype;
       
        // Instantiate a base class (but only create the instance,
        // don't run the init constructor)
        initializing = true;
        var prototype = new this();
        initializing = false;
       
        // Copy the properties over onto the new prototype
        for (var name in prop) {
            // Check if we're overwriting an existing function
            prototype[name] = typeof prop[name] == "function" &&
                typeof _super[name] == "function" && fnTest.test(prop[name]) ?
                (function(name, fn){
                    return function() {
                        var tmp = this._super;
                       
                        // Add a new ._super() method that is the same method
                        // but on the super-class
                        this._super = _super[name];
                       
                        // The method only need to be bound temporarily, so we
                        // remove it when we're done executing
                        var ret = fn.apply(this, arguments);        
                        this._super = tmp;
                       
                        return ret;
                    };
                })(name, prop[name]) :
                prop[name];
        }
       
        // The dummy class constructor
        function Class() {
          // All construction is actually done in the init method
          if (!initializing && this.init)
            this.init.apply(this, arguments);
        }
       
        // Populate our constructed prototype object
        Class.prototype = prototype;
       
        // Enforce the constructor to be what we expect
        Class.prototype.constructor = Class;
     
        // And make this class extendable
        Class.extend = arguments.callee;
       
        return Class;
    };
})(Symple);


// -----------------------------------------------------------------------------
// Dispatcher
//
Symple.Dispatcher = Symple.Class.extend({
    init: function() {      
        this.listeners = {};
    },
    
    on: function(event, fn) {
        if (typeof this.listeners[event] == 'undefined')
            this.listeners[event] = [];
        if (typeof fn != 'undefined' && fn.constructor == Function)
            this.listeners[event].push(fn);
    },

    clear: function(event, fn) {
        if (typeof this.listeners[event] != 'undefined') {
            for (var i = 0; i < this.listeners[event].length; i++) {
                if (this.listeners[event][i] == fn) {
                    this.listeners[event].splice(i, 1);
                }
            }
        }
    },

    dispatch: function() {
        //Symple.log('Dispatching: ', arguments);
        var event = arguments[0];
        var args = Array.prototype.slice.call(arguments, 1);
        if (typeof this.listeners[event] != 'undefined') {
            for (var i = 0; i < this.listeners[event].length; i++) {
                //Symple.log('Dispatching: Function: ', this.listeners[event][i]);
                if (this.listeners[event][i].constructor == Function)
                    this.listeners[event][i].apply(this, args);
            }
        }
    }
});


// -----------------------------------------------------------------------------
// Manager
//
Symple.Manager = Symple.Class.extend({
    init: function(options) {    
        this.options = options || {};
        this.key = this.options.key || 'id';
        this.store = [];
    },
    
    add: function(value) {
        this.store.push(value);
    },

    remove: function(key) {
        var res = null;
        for (var i = 0; i < this.store.length; i++) {
            if (this.store[i][this.key] == key) {
                res = this.store[i];
                this.store.splice(i, 1);
                break;
            }
        }
        return res;
    },

    get: function(key) {
        for (var i = 0; i < this.store.length; i++) {
            if (this.store[i][this.key] == key) {
                return this.store[i];
            }
        }
        return null;
    },

    find: function(params) {
        var res = [];
        for (var i = 0; i < this.store.length; i++) {
            if (Symple.match(params, this.store[i])) {
                res.push(this.store[i])
            }
        }
        return res;
    },

    findOne: function(params) {
        var res = this.find(params);
        return res.length ? res[0] : undefined;
    },

    last: function() {
        return this.store[this.store.length - 1];
    },

    size: function() {
        return this.store.length;
    }
});
