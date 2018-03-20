package main

/*
#include "main_c.h"
*/
import "C"
import "time"

func main() {}

//export BackgroundWorkerMain
func BackgroundWorkerMain(args C.Datum) {
	C.elog_log(C.CString("Starting GoBackgroundWorker"))

	go func() {
		for C.get_got_sigterm() == 0 {
			time.Sleep(time.Second * 5)
			C.elog_log(C.CString("Hello World 5"));
		}
	}()

	for C.get_got_sigterm() == 0 {
		time.Sleep(time.Second * 3)
		C.elog_log(C.CString("Hello World 3"));
	}

	if C.get_got_sigterm() != 0 {
		C.elog_log(C.CString("got sigterm"));
	}

	C.elog_log(C.CString("Finishing GoBackgroundWorker"))
}

