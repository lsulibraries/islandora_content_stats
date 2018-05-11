# islandora\_content\_stats

Requirements: islandora, php\_lib, php\_filter

Queries are stored in the mysql database as 'id' 'query' 'count' and 'timestamp'


admin -> islandora -> islandora utility modules -> content stats admin
Press "Run Queries Now"to update database.

Queries are updated when a designated amount of time has elapsed after the latest cron run, via hook_cron().
The time interval defaults 3 days.

The last run queries can be found at admin -> reports -> content statistics report last run.
You can click the "Show All Queries" button to see all queries in the history of the repository.

downloads of results as a csv are available, results can also be filtered according to content model or collection.

Special thanks go to
[Rebecca Sutton Koeser](https://github.com/rlskoeser) for an itql query that got me started.
[query found here](https://rlskoeser.github.io/2010/04/06/fedora-risearch-query-get-object-totals-cmodel)
