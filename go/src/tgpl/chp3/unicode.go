package main

import "fmt"

func main() {
    for i, r := range "Hello, 世界" {
        fmt.Printf("%d\t%q\t%d\n", i, r, r)
    }
}
