/* PostgreSQL related headers */
#include "postgres.h"
#include "pgstat.h"
#include "postmaster/bgworker.h"
#include "storage/ipc.h"
#include "storage/latch.h"
#include "storage/proc.h"
#include "miscadmin.h"
#include "utils/elog.h"
#include "fmgr.h"
#include "dynloader.h"

PG_MODULE_MAGIC;

static char *go_shared_lib = "go_shared.so";

/* Entry point of library loading */
void _PG_init(void);

/* Main loop of process */
void background_main(Datum main_arg);

void elog_log(char *string);

int postmaster_is_dead(int rc);

int get_got_sigterm(void);


/* Wrappers to using in CGO */
void elog_log(char *string) {
	elog(LOG, "Go: %s", string);
}

int postmaster_is_dead(int rc) {
	return (rc & WL_POSTMASTER_DEATH);
}

/* Signal handling */
static volatile sig_atomic_t got_sigterm = false;

int get_got_sigterm() {
	return (got_sigterm == true);
}

/*
 * background_sigterm
 *
 * SIGTERM handler.
 */
static void
background_sigterm(SIGNAL_ARGS)
{
	int save_errno = errno;
	got_sigterm = true;
	if (MyProc)
		SetLatch(&MyProc->procLatch);
	errno = save_errno;
}

/*
 * background_main
 *
 * Main loop processing.
 */
void
background_main(Datum main_arg)
{
	bgworker_main_type entrypt;
	char *golib_path;
	void *handle;

	/* Set up the sigterm signal before unblocking them */
	pqsignal(SIGTERM, background_sigterm);

	/* We're now ready to receive signals */
	BackgroundWorkerUnblockSignals();

	golib_path = (char *) palloc(strlen(pkglib_path) + 1 +
								   strlen(go_shared_lib) + 1);

	join_path_components(golib_path, pkglib_path, go_shared_lib);
    elog(DEBUG1, "loading go shared lib \"%s\"", golib_path);

	handle = pg_dlopen(golib_path);
	pfree(golib_path);

	entrypt = (bgworker_main_type) pg_dlsym(handle, "BackgroundWorkerMain");
	entrypt(main_arg);
	pg_dlclose(handle);

	proc_exit(2);
}

void _PG_init(void) {
	BackgroundWorker worker;

	/* Register the worker processes */
	MemSet(&worker, 0, sizeof(BackgroundWorker));
	worker.bgw_flags = BGWORKER_SHMEM_ACCESS;
	worker.bgw_start_time = BgWorkerStart_RecoveryFinished;
	snprintf(worker.bgw_library_name, BGW_MAXLEN, "go_background_worker");
	snprintf(worker.bgw_function_name, BGW_MAXLEN, "background_main");
	snprintf(worker.bgw_name, BGW_MAXLEN, "GoBackgroundWorker");
	worker.bgw_restart_time = BGW_NEVER_RESTART;
	worker.bgw_main_arg = (Datum) 0;
	worker.bgw_notify_pid = 0;
	RegisterBackgroundWorker(&worker);
}
