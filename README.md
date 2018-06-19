# islandora\_content\_stats

Requirements: islandora, php\_lib, php\_filter, lsulibraries/islandora_namespace_homepage, lsulibraries/islandora_utils

Queries are stored in the mysql database as 'id', 'inst','coll','cmodel', 'count' and 'timestamp'

Admin -> Islandora -> Islandora Utility Modules -> Content Statistics
Press "Run Queries Now" to update database. (must be done after installing)

Queries are updated when a designated amount of time has elapsed after the latest cron run, via hook_cron().
The time interval defaults 3 days.

The last run queries can be found at yousite.com/data or localhost:8000/data (depending on your setup)
The queries can be filtered with the drop-downs found here, by collection, and content model. (Must press "Filter" button to apply)
You can check the "Show all results" box followed by the "Filter" button to see all queries in the history of the repository. (since installing the module)

download of data as csv are available, and respect the filters selected in the gui. press "Download as CSV"

Special thanks go to
[Rebecca Sutton Koeser](https://github.com/rlskoeser) for an itql query that got me started.
[query found here](https://rlskoeser.github.io/2010/04/06/fedora-risearch-query-get-object-totals-cmodel)
