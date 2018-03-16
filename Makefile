all: background_worker go_worker

background_worker:
	$(MAKE) -C $@

go_worker:
	$(MAKE) -C $@

clean:
	$(MAKE) -C background_worker clean
	$(MAKE) -C go_worker clean

install:
	$(MAKE) -C background_worker install
	$(MAKE) -C go_worker install

.PHONY: all background_worker go_worker
