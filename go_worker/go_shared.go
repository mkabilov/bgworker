package main

/*
#include "main_c.h"
*/
import "C"
import (
	"time"
	"context"
)

func main() {}

func elog(str string) {
	C.elog_log(C.CString(str))
}

//export BackgroundWorkerMain
func BackgroundWorkerMain(args C.Datum) {
	elog("Starting GoBackgroundWorker")

	ticker := time.NewTicker(5 * time.Second)
	ctx, cancel := context.WithCancel(context.Background())
	go func() {
		for C.get_got_sigterm() == 0 {
			time.Sleep(time.Second)
		}
		elog("got sig term")
		cancel()
	}()

	go func() {
		for {
			select {
			case <-ctx.Done():
				return
			case <-ticker.C:
				elog("Hello world")
			}
		}
	}()

	<-ctx.Done()

	elog("Finishing GoBackgroundWorker")
}

