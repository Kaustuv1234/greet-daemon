package main

/*
#include "xpc.h"
*/
import "C"
import (
	"time"
)

//export getGreeting
func getGreeting(signingString *C.char) *C.char {

	greeting := "Hello " + C.GoString(signingString) + "!"
	res := C.CString(greeting)

	return res
}

func main() {
	go func() {
		C.runXPCServer()
	}()

	for {
		time.Sleep(10 * time.Hour) // Keep the program alive for a long time
	}
}
