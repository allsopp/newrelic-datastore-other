#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "libnewrelic.h"

int
main(int argc, char **argv)
{
    newrelic_app_t *app;
    newrelic_txn_t *tx;
    newrelic_app_config_t *config;
    newrelic_segment_t *s, *dss1, *dss2;
    static newrelic_datastore_segment_params_t dsp1, dsp2;
    bool rs;
    (void)argc;
    (void)argv;

    if (argc < 2) {
        fprintf(stderr, "usage: %s <license-key> <transaction-name>\n", argv[0]);
        return EXIT_FAILURE;
    }
    config = newrelic_create_app_config("foo", argv[1]);
    assert(config);

    /* pass through SQL unobfuscated */
    config->transaction_tracer.datastore_reporting.record_sql = NEWRELIC_SQL_RAW;

    rs = newrelic_configure_log("c_sdk.log", NEWRELIC_LOG_DEBUG); assert(rs);
    rs = newrelic_init(NULL, 0); assert(rs);
    app = newrelic_create_app(config, 10000);
    newrelic_destroy_app_config(&config);
    puts("# created app");

    tx = newrelic_start_web_transaction(app, argv[2]);
    printf("# started transaction: %s\n", argv[2]);
    s = newrelic_start_segment(tx, "segment", "no category");
    puts("## started segment");

    dsp1.product = NEWRELIC_DATASTORE_MYSQL;
    dsp1.query = "SELECT * FROM users WHERE password = 'secret';";
    dss1 = newrelic_start_datastore_segment(tx, &dsp1);
    printf("### started datastore segment (%s)\n", dsp1.product);
    puts("#### sleeping for two seconds");
    sleep(2);
    newrelic_end_segment(tx, &dss1);
    puts("### ended datastore segment");

    dsp2.product = NEWRELIC_DATASTORE_OTHER;
    dsp2.query = "name:one";
    dss2 = newrelic_start_datastore_segment(tx, &dsp2);
    printf("### started datastore segment (%s)\n", dsp2.product);
    puts("#### sleeping for two seconds");
    sleep(2);
    newrelic_end_segment(tx, &dss2);
    puts("### ended datastore segment");

    newrelic_end_segment(tx, &s);
    puts("## ended segment");
    newrelic_end_transaction(&tx);
    puts("# ended transaction");
    newrelic_destroy_app(&app);
    puts("# destroyed app");

    return EXIT_SUCCESS;
}
