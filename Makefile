SDK := vendor/newrelic/c-sdk

all: solr

$(SDK)/libnewrelic.so:
	$(MAKE) -C $(SDK) libnewrelic.so

solr: solr.c $(SDK)/libnewrelic.so
	ln -sfv $(SDK)/libnewrelic.so
	$(CC) -o $@ $^ -I$(SDK)/include -L$(SDK) -lnewrelic -lpcre -lm -pthread 

.PHONEY:
clean:
	rm -f solr libnewrelic.so
