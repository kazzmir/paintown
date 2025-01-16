package main

import (
    "log"
)

func main(){
    log.SetFlags(log.Ldate | log.Lmicroseconds | log.Lshortfile)
}
