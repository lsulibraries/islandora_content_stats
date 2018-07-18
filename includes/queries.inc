<?php

/**
 * @file
 * the RI SPARQL queries
 * for aggregated SPARQL queries use ->countQuery() on the tuque connection
 */
module_load_include('php', 'islandora_namespace_homepage', 'includes/namespaces');
module_load_include('php', 'islandora_namespace_homepage', 'includes/db');
module_load_include('php', 'islandora_namespace_homepage', 'includes/utilities');

/**
 * Query repository index for all cmodels that exist, owned by 'fedoraAdmin'
 * Special thanks to rebecca.s.koeser@princeton.edu for inspiring
 * @link https://rlskoeser.github.io/2010/04/06/fedora-risearch-query-get-object-totals-cmodel query. @endlink
 * @return array $modelCount
 * keyed valued with 'cmodel' (string) => count (int)
 */

// function get_all_models() {
//   $tuque = islandora_get_tuque_connection();
//   $modelquery = <<<EOL
// select ?cmodel
// from <#ri>
// where { ?cmodel <info:fedora/fedora-system:def/model#ownerId> 'fedoraAdmin' }
// EOL;
//   $models = $tuque->repository->ri->sparqlQuery($modelquery, 'unlimited');
//   $model_strings = array();
//   foreach ($models as $model) {
//     $model_strings[] = $model['cmodel']['value'];
//   }
//   return $model_strings;
// }



function get_all_collections($prefix = FALSE) {
  $filter = $prefix ? "FILTER regex(str(?pid), '$prefix')" : '';
  $tuque = islandora_get_tuque_connection();
  $query = <<<EOQ
SELECT ?obj
FROM <#ri>
WHERE {{ ?obj <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root> .
?obj <info:fedora/fedora-system:def/model#hasModel> <info:fedora/islandora:collectionCModel> }
$filter }
EOQ;
  $colls =  $tuque->repository->ri->sparqlQuery($query, 'unlimited');
  $coll_pids = array();
  foreach ($colls as $coll) {
    $coll_pids[] = $coll['obj']['value'];
  }
  return $coll_pids;
}

function total_model_count($model, $prefix = FALSE, $coll_pid = FALSE) {
  $where_coll = $coll_pid ? "?item <info:fedora/fedora-system:def/relations-external#isMemberOfCollection> <info:fedora/$coll_pid>" : '';
  $filter = $prefix ? "FILTER regex(str(?item), '$prefix')" : '';
    $query = <<<EOL
select ?item
from <#ri>
where {{ ?item <info:fedora/fedora-system:def/model#hasModel> <info:fedora/$model> .
$where_coll

} $filter}
EOL;
  if($prefix){dpm($query);}
    $tuque = islandora_get_tuque_connection();
    $result = $tuque->repository->ri->countQuery($query, 'sparql');

  return $result;
}

/**
 * @return array $return
 * array of mapped cmodel labels
 * puts into dropdown for filter on data.form.inc
 */
function cmodel_types_for_form() {
    module_load_include('inc', 'islandora_content_stats', 'includes/utilities');
    $model_counts = array_keys(islandora_get_content_models());
    $return = array();
    foreach ($model_counts as $model) {
        //when cmodel is FedoraObject or ContentModel, don't map
        $not_fobj = $model != 'fedora-system:FedoraObject-3.0';
        $not_cmobj = $model != 'fedora-system:ContentModel-3.0';
        if (($not_fobj) && ($not_cmobj)) {
            $string = map_cmodels($model);
            $return[$model] = t($string);
        }
    }
    return $return;
}

/**
 * query for all institutional an sub-institutional prefixes
 * @return array $prefixes
 * array of possible prefixes for inst and sub-inst
 * (better to use namespace_homepage? or not?)
 */
function get_all_prefixes() {
    $tuque = islandora_get_tuque_connection();
    $prefixes = array();
    $all_root_members = <<<EOL
SELECT DISTINCT ?collection
FROM <#ri>
WHERE
{?collection <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root>}
EOL;
    $result = $tuque->repository->ri->sparqlQuery($all_root_members, 'unlimited');
    foreach ($result as $key => $val) {
        $value = $val['collection']['value'];
        if ((isset($value)) && (strpos($value, '-'))) {
            $plode = explode('-', $value);
            array_pop($plode);
            $joined = implode('-', $plode);
            $prefixes[] = $joined;
        }
    }
    return $prefixes;
}

/**
 * @return array $return
 * mapped names of institutions as array
 * puts into dropdown for filter on data.form.inc
 */
function institutions_for_form() {
    module_load_include('inc', 'islandora_content_stats', 'includes/utilities');
    $prefixes = get_all_prefixes();
    $return = array();
    foreach ($prefixes as $inst) {
        $return[$inst . '-'] = map_institution($inst, 'form');
    }
    return $return;
}

/**
 * Query the ri for a count of a collection's content models
 * @param string $collection
 * a given collection's pid, to apply to the query
 * @return array $return
 * array of content model totals for a given collection
 */
function collection_cmodel_query($collection) {
    $tuque = islandora_get_tuque_connection();
    $coll_items_models = <<<EOL
select ?cmodel ?item
from <#ri>
where { ?item <fedora-rels-ext:isMemberOfCollection> <info:fedora/$collection> .
?item <info:fedora/fedora-system:def/model#hasModel> ?cmodel }
EOL;
    $cmodel_counts = $tuque->repository->ri->sparqlQuery($coll_items_models, 'unlimited');
    $modelCount = array();
    foreach ($cmodel_counts as $model) {
        $model = $model['cmodel']['value'];
        if ($model != 'fedora-system:FedoraObject-3.0') {
            $countq = <<<EOL
select ?item
from <#ri>
where { ?item <info:fedora/fedora-system:def/model#hasModel> <info:fedora/$model> .
?item <fedora-rels-ext:isMemberOfCollection> <info:fedora/$collection> }
EOL;
            $result = $tuque->repository->ri->countQuery($countq, 'sparql');
            if ($result > 0) {
                $modelCount[$model] = $result;
            }
        }
    }
    $return[$collection] = $modelCount;
    return $return;
}



/**
 * @return array $return
 * options for data.form.inc dropdown of colleciton names
 */
function collection_pid_labels_for_form() {
    module_load_include('inc', 'islandora_content_stats', 'includes/utilities');
    $return = array();
    $tuque = islandora_get_tuque_connection();
    $query = <<<EOL
SELECT ?pid
FROM <#RI>
{?pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root>}
EOL;
    $collections = $tuque->repository->ri->sparqlQuery($query, 'unlimited');
    foreach ($collections as $coll) {
        $coll = $coll['pid']['value'];
        $return[$coll] = map_collection($coll);
    }
    $return['All'] = 'All';
    return $return;
}

/**
 * @return array $fields
 * each inst + digit as key =>
 * value is array of fields for db
 */
function all_institution_each_collection_query_count() {
    $tuque = islandora_get_tuque_connection();
    $institution_colls = array();
    $fields = array();
    $institutions = get_all_prefixes();
    foreach ($institutions as $lib) {
        $lib = check_prefix($lib);
        $query = <<<EOQ
SELECT ?pid
FROM <#ri>
WHERE { ?pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root> }
EOQ;
        $coll_results = $tuque->repository->ri->sparqlQuery($query, 'unlimited');
        foreach ($coll_results as $num => $coll) {
            $institution_colls[$lib . $num] = $coll['pid']['value'];
        }
    }
    foreach ($institution_colls as $inst_num => $collection) {
        $coll_count_results = collection_cmodel_query($collection);
        foreach ($coll_count_results as $collection => $array) {
            foreach ($array as $model => $count) {
                $fields[$inst_num]['inst'] = preg_replace('/\d/', '', $inst_num);
                $fields[$inst_num]['coll'] = $collection;
                $fields[$inst_num]['models'][$model] = $count;
            }
        }
    }
    return $fields;
}

/**
 * marks up the content models available
 * @param string $prefix
 *  prefix of institution or sub-institution
 * @return array
 *  array of an institution's total of items with a particular content model.
 */
function institution_cmodel_totals($prefix, $model) {
    module_load_include('inc', 'islandora_content_stats', 'includes/utilities');
    $valid_cmodels = array();
    //total count of each cmodel
    $cmodels = islandora_get_content_models();
    //we only use the model
    foreach ($cmodels as $model => $count) {
        if (cmodel_check($model)) {
            $valid_cmodels[] = $model;
        }
    }
    $tuque = islandora_get_tuque_connection();
    //first query gets all the collections that contain the $prefix string.
    $prefix = check_prefix($prefix);
    $ns_member_q = <<<EOQ
SELECT ?pid
FROM <#ri>
WHERE {
{ ?pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root> }
FILTER regex(str(?pid), '$prefix') }
ORDER BY ?pid
EOQ;
    //total collections in an institution
    $member_colls = $tuque->repository->ri->sparqlQuery($ns_member_q, 'unlimited');
    $coll_cmodel_items = array();
    foreach ($member_colls as $coll) {
        $coll = $coll['pid']['value'];
        //counts array
        $count_array = array();
        //count of number of collections with given namespace
        //for each cmodel, count members of each collection
        foreach ($valid_cmodels as $model) {
          $count_array[$model] = 0;
                //query asks for items and models, which are active, and belong
                //to a collection, Filter by current model only.
                $query_cm = <<<EOL
SELECT ?cmodel ?item
FROM <#ri>
WHERE { { ?item <info:fedora/fedora-system:def/model#hasModel> ?cmodel .
?item <info:fedora/fedora-system:def/model#state> <fedora-model:Active> .
?item <fedora-rels-ext:isMemberOfCollection> <info:fedora/$coll> . }
FILTER (!sameterm(str(?cmodel), '<info:fedora/$model>')) }
EOL;
                $coll_cmodel_items = $tuque->repository->ri->sparqlQuery($query_cm, 'unlimited');
                foreach ($coll_cmodel_items as $item) {
                    $itemModel = $item['cmodel']['value'];
                    if (cmodel_check($itemModel) && $itemModel == $model) {
                        $count_array[$itemModel] += 1;
                    }
                }
            }
        }
        $count_array['islandora:collectionCModel'] = $tuque->repository->ri->countQuery($ns_member_q, 'sparql');
    return $count_array;
}

/**
 * saves the results to a database table (with a timestamp)
 * manually runs the queries now.
 */
function islandora_content_stats_run_queries() {
    module_load_include('inc','islandora','includes/utilities');
    $time = time();
    //array of each namespace prefix and sub-inst namespace prefix ie [lsu,lsu-ag];
    $all_prefixes = get_all_prefixes();
    dpm($all_prefixes);
    //all available models
    $all_cmodels = array_keys(islandora_get_content_models());

    //all available collections at islandora:root
    $all_colls = get_all_collections();
    foreach ($all_cmodels as $model) {
        $count = total_model_count($model);
        //dpm($model .': ' . $count);

        $coll = 'islandora:root';
        queries_db_insert(NULL, $coll, $model, $count, $time);
        foreach ($all_prefixes as $prefix) {
          $inst_model_count = total_model_count($model, $prefix);
          dpm( "$prefix $model $inst_model_count");
          queries_db_insert($prefix . '-', NULL, $model, $inst_model_count, $time);
          //dpm($model .' ' . $inst_colls);
          $inst_colls = get_all_collections($prefix);
          foreach ($inst_colls as $coll) {
            $inst_collection_model_count = total_model_count($model, $prefix, $coll);
            //dpm($inst_collection_model_count);
            queries_db_insert($prefix . '-', $coll, $model, $inst_collection_model_count, $time);
          }
        }
        foreach ($all_colls as $coll) {
          $count = total_model_count($model, NULL, $coll);
          //dpm($count);
          queries_db_insert(NULL,$coll,$model,$count,$time);
        }
    }
}

/**
 *  execute query returns to mysql.
 */
function queries_db_insert($inst, $coll, $cmodel, $count, $time) {
    db_insert('islandora_content_stats')
            ->fields(array('id', 'inst', 'coll', 'cmodel', 'count', 'timestamp'))
            ->values(array(
                'id' => NULL,
                'inst' => $inst,
                'coll' => $coll,
                'cmodel' => $cmodel,
                'count' => $count,
                'timestamp' => $time,
            ))
            ->execute();
}

/**
 * makes sure queries for ns are distinct
 * otherwise lsu matches lsu-sub
 * @param string $prefixe
 * institution string to check for '-'
 */
function check_prefix($prefix) {
    if (strpos($prefix, '-') === FALSE) {
        $prefix = $prefix . '-';
    }
    return $prefix;
}

/**
 * @param string $pid
 * pid of collection is parsed from path of browser
 * @return array $display
 * each query for a particular collection pid
 */
function islandora_content_stats_block_coll($pid) {
    $pref = 'select * from ldl.islandora_content_stats where coll = \'';
    $last_time = db_query('select max(timestamp) from ldl.islandora_content_stats');
    $time = $last_time->fetchField();
    $suff = ' and timestamp = \'';
    $query = db_query($pref . $pid . '\'' . $suff . $time . '\'');
    $results = $query->fetchAll();
    $display = array();
    foreach ($results as $result) {
        $test = $result->cmodel;
        if (strpos($test, 'fedora-system') === FALSE) {
            $display[$result->id] = array(
                // 'id' => $result->id,
                // 'inst' => $result->inst,
                'coll' => $result->coll,
                'cmodel' => $result->cmodel,
                'count' => $result->count
            );
        }
    }
    return $display;
}

/**
 * @param string $path
 * path of browser which matches a prefix
 * @return array $display
 * each query for a particular institution
 */
function islandora_content_stats_block_inst($path) {
    $pref = 'select * from ldl.islandora_content_stats where inst = \'';
    $end = '\' and coll is NULL';
    $last_time = db_query('select max(timestamp) from ldl.islandora_content_stats');
    $time = $last_query_time_db->fetchField();
    $suff = ' and timestamp = \'';
    $query = db_query($pref . $path . '\'' . $suff . $time . $end);
    $results = $query->fetchAll();
    $display = array();
    foreach ($results as $result) {
        $test = $result->cmodel;
        if (strpos($test, 'fedora-system') === FALSE) {
            $display[$result->id] = array(
                // 'id' => $result->id,
                // 'inst' => $result->inst,
                'coll' => $result->coll,
                'cmodel' => $result->cmodel,
                'count' => $result->count
            );
        }
    }
    return $display;
}

/**
 * @return array $display
 * each query where coll is islandora:root (ints == NULL)
 */
function islandora_root_stats_queries() {
    $last_time = db_query('select max(timestamp) from ldl.islandora_content_stats');
    $time = $last_time->fetchField();
    $pref = 'select * from ldl.islandora_content_stats where inst is NULL and timestamp = \'';
    $suff = '\' and cmodel not like \'fedora-system%\'';
    $query = db_query($pref . $time . $suff);
    $results = $query->fetchAll();
    $display = array();
    foreach ($results as $result) {
        $display[$result->id] = array(
            //'id' => $result->id,
            //'inst' => $result->inst,
            //'coll' => $result->coll,
            'cmodel' => $result->cmodel,
            'count' => $result->count,
        );
    }
    return $display;
}
