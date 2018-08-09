<?php

/**
 * @file
 * the RI SPARQL queries
 * for aggregated SPARQL queries use ->countQuery() on the tuque connection
 * Special thanks to rebecca.s.koeser@princeton.edu for inspiring
 * @link https://rlskoeser.github.io/2010/04/06/fedora-risearch-query-get-object-totals-cmodel query. @endlink
 */

function get_all_collections($prefix = FALSE) {
  $prefix = $prefix . '-';
  $filter = $prefix ? "FILTER regex(str(?obj), '$prefix')" : '';
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
  if($prefix != FALSE){$prefix = $prefix . '-';}
  $where_coll = $coll_pid ? "?item <info:fedora/fedora-system:def/relations-external#isMemberOfCollection> <info:fedora/$coll_pid> ." : '';
  $xfactor = $model == 'islandora:sp_large_image_cmodel' ? '?x' : '';
  $not_constituent = $model == 'islandora:sp_large_image_cmodel' ? "?item <info:fedora/fedora-system:def/relations-external#isMemberOfCollection> ?x ." : '';

  $filter = $prefix ? "FILTER regex(str(?item), '$prefix')" : '';
    $query = <<<EOL
select ?item $xfactor
from <#ri>
where {{ ?item <info:fedora/fedora-system:def/model#hasModel> <info:fedora/$model> .
$where_coll
$not_constituent
} $filter}
EOL;
    $tuque = islandora_get_tuque_connection();
    $result = $tuque->repository->ri->countQuery($query, 'sparql');
  return $result;
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
    return array_unique($prefixes);
}

function get_collection_policy_array($pid) {
    $query_models = array();
    $repo = islandora_get_tuque_connection();
    $api_a = $repo->api->a;
    $pol = $api_a->getDatastreamDissemination($pid, 'COLLECTION_POLICY');
    $models = simplexml_load_string($pol)->content_models;
    foreach( $models->content_model as $model){
      $query_models[] = (string) $model['pid'];
    }
  return $query_models;
}

function compound_check_constituency($prefix, $coll = FALSE) {
    $coll = $coll ? islandora_object_load($coll) : '';
    $label = gettype($coll) == 'object' ? $coll->label : '';
    $tuque = islandora_get_tuque_connection();
    $query = <<<EOL
SELECT ?item ?parent ?coll_parent ?parentmodel ?itemmodel
FROM <#RI>
WHERE {{
?item <info:fedora/fedora-system:def/relations-external#isConstituentOf> ?parent .
?parent <info:fedora/fedora-system:def/model#hasModel> ?itemmodel .
?parent <info:fedora/fedora-system:def/relations-external#isMemberOfCollection> ?coll_parent .
?parent <info:fedora/fedora-system:def/model#hasModel> ?parentmodel
}
FILTER regex(str(?item), '$prefix')
}
EOL;
    $count = $tuque->repository->ri->countQuery($query, 'sparql');
    $sparql = $tuque->repository->ri->sparqlQuery($query, 'unlimited');


    $return = array();
    $return['count'] = $count / 2;
    //loop for dc relation to collection parent of constituent items.
    $coll_parents = array();
    $item_models = array();
    foreach ($sparql as $sparq){
            $coll_pid = $sparq['coll_parent']['value'];
            explode('/',$coll_pid);
            $coll_parents[] = $coll_pid;
            $item_models[] = $sparq['itemmodel']['value'];
        }

    $coll_parent = array_unique($coll_parents);
    if (isset($coll_parent)){
      $return['coll_parent'] = $coll_parent;
    }

    //loop for cmodel of compound parents
    $cpdparentcmodels = array();
    foreach ($sparql as $sparq) {
        $cpdparentcmodels[] = $sparq['parentmodel']['value'];
    }
    $parent_models = array_unique($cpdparentcmodels);
    $parent_models = array_diff($parent_models, ['fedora-system:FedoraObject-3.0']);
    $item_models = array_unique($item_models);
    $item_models = array_diff($item_models, ['fedora-system:FedoraObject-3.0']);
    $return['parentModels'] = $parent_models;
    $return['itemmodels'] = $item_models;
    return $return;
}

/**
 * saves the results to a database table (with a timestamp)
 * manually runs the queries now.
 */

function islandora_content_stats_run_queries() {
    module_load_include('inc','islandora','includes/utilities');
    module_load_include('inc','islandora_content_stats','includes/utilities');

    $time = time();
    //array of each namespace prefix and sub-inst namespace prefix ie [lsu,lsu-ag];
    $all_prefixes = get_all_prefixes();
    //all available models
    $all_cmodels = array_keys(islandora_get_content_models());
    //all available collections at islandora:root
    $all_colls = get_all_collections();
    //Loop for Totals at root
    foreach ($all_cmodels as $model) {
            //root totals
            $count = total_model_count($model);
            $coll = 'islandora:root';
            queries_db_insert(NULL, $coll, $model, $count, $time);
            //loop for all institutions
    }
    foreach ($all_prefixes as $prefix) {
        $total_policy = [];
        $inst_colls = get_all_collections($prefix);
        //check each inst for constituent members.
        // $children_of_cpds = compound_check_constituency($prefix);
        // queries_db_insert($prefix . '-', 'All', 'children of compound object types',$children_of_cpds['count'],$time);
        // //(check the type of parent of constituents.)
        //loop through each inst_collection, total for collection
         foreach ($inst_colls as $coll) {
        //     $collCpds = compound_check_constituency($prefix, $coll);
        //     queries_db_insert($prefix . '-', $coll, $collCpds['parentModels'], $collCpds['count'], $time);
             $policy = get_collection_policy_array($coll);
             $total_policy = array_merge($policy);
        //     //loop through each model in the collection policy
        //     foreach ($policy as $model) {
        //         //collection_totals
        //         $inst_collection_model_count = total_model_count($model, $prefix, $coll);
        //         queries_db_insert($prefix . '-', $coll, $model, $inst_collection_model_count, $time);
        //     }
         }
             //we have constructed total_policy as an array of cmodels possible in a given inst
             $total_policy = array_unique($total_policy);
             //policies may not be set on build boxes, but we know we always want to check collection.
              if(!in_array('islandora:collectionCModel',$total_policy)){
                $total_policy[] = 'islandora:collectionCModel';
              }
              //loop through all models available in an inst.
              foreach ($total_policy as $model){
              //inst_totals
              $inst_model_count = total_model_count($model, $prefix);
              queries_db_insert($prefix . '-', NULL, $model, $inst_model_count, $time);
             }
        //}
    }
}


/**
 *  execute query returns to mysql.
 */
function queries_db_insert($inst, $coll, $cmodel, $count, $time) {
  if ($count > 0){
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
}


/**
 * @return array $display
 * each query where coll is islandora:root (ints == NULL)
 */
function islandora_root_stats_queries() {
    $last_time = db_query('select max(timestamp) from {islandora_content_stats}');
    $time = $last_time->fetchField();
    $pref = 'select * from {islandora_content_stats} where inst is NULL and timestamp = \'';
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
