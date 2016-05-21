package main

import (
    "fmt"
    "flag"
    "crypto/sha256"
)

var flag384 = flag.Bool("3", false, "sha384 digest")
var flag512 = flag.Bool("5", false, "sha512 digest")

func main() {
    flag.Parse()

    if *flag384 && *flag512 {
        fmt.Println("Both sha384 and sha512 are set")
    } else if *flag384 {
    } else if *flag512 {
    } else {
        c := sha256.Sum256([]byte("x"))
        fmt.Printf("%x\n%T\n", c, c)
    }

}
