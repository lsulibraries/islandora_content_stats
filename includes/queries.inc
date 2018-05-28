<?php
// the RI (ITQL & SPARQL) queries
//for aggregated SPARQL queries always use ->countQuery() from the tuque connection

/**
 * Query repository index for all cmodels that exist, are active and
 * Special thanks to rebecca.s.koeser@princeton.edu for this query
 * https://rlskoeser.github.io/2010/04/06/fedora-risearch-query-get-object-totals-cmodel
 */
function all_cmodels(){
  $tuque = islandora_get_tuque_connection();
  $query = <<<EOL
select \$cmodel
count(select \$item from <#ri>
where \$item <info:fedora/fedora-system:def/model#hasModel> \$cmodel
and \$item <info:fedora/fedora-system:def/model#state> <fedora-model:Active>)
from <#ri>
where \$item <info:fedora/fedora-system:def/model#hasModel> \$cmodel
having \$k0 <http://mulgara.org/mulgara#occursMoreThan> '0.0'^^<http://www.w3.org/2001/XMLSchema#double> ;
EOL;
  $result = $tuque->repository->ri->itqlQuery($query, 'unlimited');
  return $result;
}

/**
* returns an array of options to be used in the data.form.inc dropdown
*/
function cmodel_types_for_form(){
  $model_counts = all_cmodels();
  $return = [];
  foreach($model_counts as $model){
    $model = $model['cmodel']['value'];
    if($model != 'fedora-system:FedoraObject-3.0' && $model != 'fedora-system:ContentModel-3.0'){
      $string = map_cmodels($model);
      $return[$model] = t($string);
    }
  }
  return $return;
}

//return only cmodels no counts. wish I could figure it in sparql
// function cmodel_types(){
//   $tuque = islandora_get_tuque_connection();
//   $members = array();
//   $cmodel_types = <<<EOL
// select \$cmodel from <#ri>
// where \$item <info:fedora/fedora-system:def/model#hasModel> \$cmodel
// EOL;
//
//   $result = $tuque->repository->ri->itqlQuery($cmodel_types, 'unlimited');
//   return $result;
// }


function all_collections(){
  $tuque = islandora_get_tuque_connection();
  $collections = array();
  $all_root_members = <<<EOL
SELECT DISTINCT ?pid
FROM <#ri>
WHERE
{?pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root>}
EOL;
  $result = $tuque->repository->ri->sparqlQuery($all_root_members, 'unlimited');
  foreach($result as $key => $val){
    $value = $val['pid']['value'];
    $collections[] = $value;
  }
  return $collections;
}

/**
 * Query the ri for a count of a collection's content models
 * use with institutions somehow
 */
function collection_cmodel_query($collection) {
  $tuque = islandora_get_tuque_connection();
  $query = <<<EOL
select \$cmodel
count(select \$item from <#ri>
where \$item <info:fedora/fedora-system:def/model#hasModel> \$cmodel
and \$item <info:fedora/fedora-system:def/model#state> <fedora-model:Active>
and \$item <fedora-rels-ext:isMemberOfCollection> <info:fedora/$collection>
)
from <#ri>
where \$item <info:fedora/fedora-system:def/model#hasModel> \$cmodel
having \$k0 <http://mulgara.org/mulgara#occursMoreThan> '0.0'^^<http://www.w3.org/2001/XMLSchema#double> ;
EOL;

  $cmodel_counts = $tuque->repository->ri->itqlQuery($query, 'unlimited');
  $sanitized_array = [];
  foreach($cmodel_counts as $count){
    $cmodel = $count['cmodel']['value'];
    $count = $count['k0']['value'];
    if($cmodel != 'fedora-system:FedoraObject-3.0'){
      $sanitized_array[$cmodel] = $count;
    }
  }
  $result[$collection] = $sanitized_array;
  return $result;
}

/**
* returns array of options for data.form.inc dropdown of colleciton names
*/
function collection_pid_labels_for_form(){
  module_load_include('inc', 'islandora_content_stats','/includes/utilities');
  $collections = all_collections();
  foreach($collections as $coll){
    $coll = $coll;
    $return[$coll] = map_collection($coll);
  }
  $return['All'] = 'All';
  return $return;
}

/**
 * Implements hook_cron().
 * Runs queries at regular intervals
 * IF the current cron time is greater than the interval of time plus the last query runtime
 */
function islandora_content_stats_cron() {
  $interval = strtotime(variable_get('islandora_content_stats_interval')) -time();
  $last_q_run = '';
  $q_time = db_query('select max(timestamp) from ldl.islandora_content_stats');
  $time = $q_time->fetchField();

  $last_q_plus_interval = $interval + $time;
  if(time() >= $last_q_plus_interval){
    islandora_content_stats_run_queries();
  }
}

// save the results to a database table (with a timestamp)
// manually run the queries now.
function islandora_content_stats_run_queries(){
  $time = time();
  $all_models = all_cmodels();
  foreach($all_models as $model){
    db_insert('islandora_content_stats')
      ->fields(array('id', 'coll', 'cmodel', 'count', 'timestamp'))
      ->values(array(
        'id' => NULL,
        'coll' =>  'islandora:root',
        'cmodel' => $model['cmodel']['value'],
        'count' => intval($model['k0']['value']),
        'timestamp' => $time,
      ))
      ->execute();
  }
  $all_colls = all_collections();
  $each_coll_cmodel= array();
  foreach($all_colls as $coll){
    $each_coll_cmodel[$coll] = collection_cmodel_query($coll);
  }
  foreach($each_coll_cmodel as $coll){
    //coll is key value paired with pid -> array()
    foreach($coll as $pid => $modelcount){
      //modelcount is associative array
      foreach($modelcount as $model => $count){
        db_insert('islandora_content_stats')
        ->fields(array('id', 'coll', 'cmodel', 'count', 'timestamp'))
        ->values(array(
          'id' => NULL,
          'coll' => $pid,
          'cmodel' => $model,
          'count' => $count,
          'timestamp' => $time,
        ))
        ->execute();
      }
    }
  }
}

/**
* returns contextual queries for a given collection
* used by the reports-page.form.inc not used by default
*/

function islandora_content_stats_page_cmodels_coll($path){
  $query ="select * from ldl.islandora_content_stats where coll = ";
  $last_query_time_db = db_query('select max(timestamp) from ldl.islandora_content_stats');
  $time = $last_query_time_db->fetchField();
  $query_suf = " and timestamp = ";
  $result = db_query($query .  '\''. $path . '\'' . $query_suf . '\'' .$time . '\'');
  $results = $result->fetchAll();
  $display = [];
  foreach($results as $query_r){
    $test = $query_r->cmodel;
    if(strpos($test, 'fedora-system') === FALSE){
      $display[$query_r->id] = array(
        'coll' => $query_r->coll,
        'cmodel' => $query_r->cmodel,
        'count' => $query_r->count
      );
    }
  }
  return $display;
}

/**
* returns contextual queries for the front page/ all collections page
* used by the reports-page.form.inc not used by default
*/
function islandora_root_stats_queries(){
  $last_query_time_db = db_query('select max(timestamp) from ldl.islandora_content_stats');
  $last_q_time = '';
  $time = $last_query_time_db->fetchField();
  $query ="select * from ldl.islandora_content_stats where timestamp = ";
  $result = db_query($query. '\'' .$time . '\''.' and cmodel not like '.'\''.'fedora-system%'.'\'');
  $results = $result->fetchAll();
  $display = [];
  foreach($results as $query_r){
    $display[$query_r->id] = array(
     'cmodel' => $query_r->cmodel,
     'count' => $query_r->count,
    );
  }
  return $display;
}
