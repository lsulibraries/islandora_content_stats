# islandora\_content\_stats

Requirements: islandora, php\_lib, php\_filter

this module provides automatically run queries that are returned as a form at /data, 
queries count the number of items with a given content model, and keep count of each collection's content model totals, as well as site wide totals for each content model.


Queries are stored in the mysql database as 'id', 'coll','cmodel', 'count', and 'timestamp'
Admin -> Islandora -> Islandora Utility Modules -> Content Statistics
Press "Run Queries Now" to update database. (must be done after installing)

Queries are updated on a given month and hour when cron is also running.

The last run queries can be found at yoursite.org/data

Data can be downloaded as a csv, with any filters applied to the gui applied to the download.

The last run queries can be found at yousite.com/data or localhost:8000/data (depending on your setup)
The queries can be filtered with the drop-downs found here, by collection, and content model. (Must press "Filter" button to apply)

You can check the "Show all results" box followed by the "Filter" button to see all queries in the history of the repository. (since installing the module)

Special thanks go to
[Rebecca Sutton Koeser](https://github.com/rlskoeser) for an itql query that got me started.
[query found here](https://rlskoeser.github.io/2010/04/06/fedora-risearch-query-get-object-totals-cmodel)
