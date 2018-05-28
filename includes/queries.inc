<?php
module_load_include('php','islandora_namespace_homepage','includes/namespaces');
module_load_include('php','islandora_namespace_homepage','includes/db');
module_load_include('php','islandora_namespace_homepage','includes/utilities');
// the RI (ITQL & SPARQL) queries
//for aggregated SPARQL queries always use ->countQuery() from the tuque connection
/**
 * Query repository index for all cmodels and a count for each.
 * Special thanks to rebecca.s.koeser@princeton.edu for this query
 * https://rlskoeser.github.io/2010/04/06/fedora-risearch-query-get-object-totals-cmodel
 */
function all_cmodels(){
  $tuque = islandora_get_tuque_connection();
  $all_cmodels_query = <<<EOL
select \$cmodel
count(select \$item from <#ri>
where \$item <info:fedora/fedora-system:def/model#hasModel> \$cmodel
and \$item <info:fedora/fedora-system:def/model#state> <fedora-model:Active>)
from <#ri>
where \$item <info:fedora/fedora-system:def/model#hasModel> \$cmodel
having \$k0 <http://mulgara.org/mulgara#occursMoreThan> '0.0'^^<http://www.w3.org/2001/XMLSchema#double> ;
EOL;
  $result = $tuque->repository->ri->itqlQuery($all_cmodels_query, 'unlimited');
  return $result;
}

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

function all_collections(){
  $tuque = islandora_get_tuque_connection();
  $collections = array();
  $all_root_members = <<<EOL
SELECT ?pid
FROM <#ri>
WHERE
{?pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root>}
EOL;
  $result = $tuque->repository->ri->sparqlQuery($all_root_members, 'unlimited');
  foreach($result as $key => $val){
    $value = $val['pid']['value'];
    $collections[] = $value;
  }
  return array_unique($collections);
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

function collection_pid_labels_for_form(){
  $return = [];
  $tuque = islandora_get_tuque_connection();
  $query = <<<EOL
SELECT ?pid
FROM <#RI>
{?pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root>}
EOL;

  $collections = $tuque->repository->ri->sparqlQuery($query,'unlimited');
  foreach($collections as $coll){
    $coll = $coll['pid']['value'];
    $return[$coll] = map_collection($coll);
  }
  $return['All'] = 'All';
  return $return;
}

//return only cmodels no counts. wish I could figure it in sparql
function cmodel_types(){
  $tuque = islandora_get_tuque_connection();
  $members = array();
  $cmodel_types = <<<EOL
select \$cmodel from <#ri>
where \$item <info:fedora/fedora-system:def/model#hasModel> \$cmodel
EOL;

  $result = $tuque->repository->ri->itqlQuery($cmodel_types, 'unlimited');
  return $result;
}

 //if an empty texfield is submitted it defaults to a resonable interval.
 if(!variable_get('islandora_content_stats_interval')){
     variable_set('islandora_content_stats_interval', '3 days');
 }
/**
 * Implements hook_cron().
 * Runs our queries
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
    foreach($coll as $key => $modelcount){
      foreach($modelcount as $model => $count){
        db_insert('islandora_content_stats')
        ->fields(array('id', 'coll', 'cmodel', 'count', 'timestamp'))
        ->values(array(
          'id' => NULL,
          'coll' => $key,
          'cmodel' => $model,
          'count' => $count,
          'timestamp' => $time,
        ))
        ->execute();
      }
    }
  }
}

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
