;
; DELUXE example
; 
; Some thoughts about DELUXE:
; DELUXE is a functional, procedural and object oriented programming language.
; It is neither a LISP nor a Scheme.
; It is based on S-Expressions, favors functional style, simplicity and written-out words instead of abbrevations.
; Every expression is a function call.
; You can use Bracket pairs of (), [] or {}
; () should be used in General
; [] should be used for collections like lists and maps
; {} should be used for function or class definitions
; But finally, use whatever you like to make your code readable.

(module my-module
    (var my-variable "some string")
    (var my-variable "new value") ; Just replace the value
    (let my-binding 42)
    (let my-binding 43) ; ERROR: my-binding is already bound
    { function my-function a b c ; define a function with 3 parameters
        (let !result (add a b c)) ; bind a symbol "!result" to the result of add function
        (return !result) ; return the result
    }
    { function my-async-function fn
        (let !result (add a b c))
        (return (fn !result))
    }
    (let my-array [list "foo" "bar" "baz"]) ; bind an array (standard function list)
    (let my-map (map [pair foo "foo"] [pair "bar" "bar"] [pair baz "baz"])) ; bind a map (take note of the "bar" key)
    (let foo-value (my-map.get foo)) ; get map entry by symbol (symbols can be values, too)
    (let bar-value (my-map.get "bar")) ; get map entry by string

    { class my-class object-name
        (member name (sprintf "my-class:%s" object-name))
        (member my-method this ; the first argument is always the instance reference. You may call it this or anything else
            (printf "My name is: %s" this.name)
        )
        (on my-message message-data ; define a message handler
            (match (typeof message-data)
                (case "string" (printf "Got message: %s" message-data))
                (case "number" (printf "Got message: %d" message-data))
                (case-default (printf "Got message, but I don't understand it"))
            )
        )
    }

    (let my-object (my-class "instance"))
    (my-object.my-method) ; prints "My name is: my-class:instance"
    (send my-message my-object "hello there") ; send a message to my object (prints: "Got message: hello there")
    (send unknown-message my-object 1234) ; sends a message, but won't do anything as the objects doesn't handle it
)