package main

import "fmt"

var x int = 10

type Int int

func (n Int) String() string { return fmt.Sprintf("MyInt[%d]", n) }

func main() {
    x := 20
    fmt.Println(x)
    var p, q *int
    if (p == nil) {
        fmt.Println("nil pointer")
    }
    p, q = &x, &x
    if (p != nil) {
        fmt.Println(*p)
        *p = 30
    }
    fmt.Println(*q)
    var y Int = 11
    fmt.Println(y)
}
