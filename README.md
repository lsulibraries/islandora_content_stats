# islandora\_content\_stats

Requirements: islandora, php\_lib, php\_filter

Queries are stored in the mysql database as 'id', 'coll','cmodel', 'count', and 'timestamp'

admin -> islandora -> islandora utility modules -> content stats admin
Press "Run Queries Now"to update database. (must do this after installing to see anything at /data

Queries are updated when a designated amount of time has elapsed after the latest cron run, via hook_cron().
The time interval defaults 3 days.

The last run queries can be found at yoursite.org/data

Results can be filtered with dropdowns and the "Filter" button, by content model or collection.

You can check the "Show all results" box and press "Filter" to see all queries in the history of the repository (since you installed it.) 

Data can be downloaded as a csv, with any filters applied to the gui applied to the download.

Special thanks go to
[Rebecca Sutton Koeser](https://github.com/rlskoeser) for an itql query that got me started.
[query found here](https://rlskoeser.github.io/2010/04/06/fedora-risearch-query-get-object-totals-cmodel)
