(module test-example
    (printf (sprintf "Hello there, %s" "General Kenobi"))
    (printf "That's it, from our example")
    (error 123.45 "1a1")
    [list 1 2 3 4 "foo bar" (to-number "123.456") ]
    [foo]
    (define my-variable "asdf 42")
    (define my-list [list 1337 1338 1339])
    { for-each @item [list 1 2 3 4 5] (printf "Item: %i" @item) }
    (web.get "/" (fn (param req res) {res.send "Hello World"} ))
)