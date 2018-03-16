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

	for C.get_got_sigterm() == 0 {
		time.Sleep(time.Second * 5)
		C.elog_log(C.CString("Hello World"));
	}

	if C.get_got_sigterm() != 0 {
		C.elog_log(C.CString("got sigterm"));
	}

	C.elog_log(C.CString("Finishing GoBackgroundWorker"))
}

