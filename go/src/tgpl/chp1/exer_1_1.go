package main

import (
    "fmt"
    "os"
)

func main() {
    s, sep := "", ""

    for i := 0; i < len(os.Args); i += 1 {
        s += sep + os.Args[i]
        sep = " "
    }
    fmt.Println(s)
}
