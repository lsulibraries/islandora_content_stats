<?php
module_load_include('php','islandora_namespace_homepage','includes/namespaces');
module_load_include('php','islandora_namespace_homepage','includes/db');
module_load_include('php','islandora_namespace_homepage','includes/utilities');
// the RI (ITQL & SPARQL) queries

//for aggregated SPARQL queries always use ->countQuery() from the tuque connection
/**
 * Query repository index for all cmodels and a count for each.
 */
function all_cmodel_counts(){
  $tuque = islandora_get_tuque_connection();
  $members = array();

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
  foreach($cmodel_counts as $cmodel_count){
      $sanitized_array[$cmodel_count['cmodel']['value']] = $cmodel_count['k0']['value'];
  }
  $result[$collection] = [$sanitized_array];
  return $result;
}

/**
 * SPARQLQuery for the content models and counts within an institution
 * used by all_institution_cmodel_query
 */
function institution_cmodel_query($ns_prefix){
  $tuque = islandora_get_tuque_connection();
  //first query gets all the collections that contain the ns_prefix string.
  $ns_prefix = $ns_prefix.'-';
  $query_ns = <<<EOQ
SELECT ?pid
FROM <#ri>
WHERE {
{?pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root>}
FILTER regex(str(?pid), "$ns_prefix")
}
ORDER BY ?pid
EOQ;
  $members = [];
  $result =  $tuque->repository->ri->sparqlQuery($query_ns, 'unlimited');
  $int = 0;
  foreach ($result as $member) {
    $count = count($result);
    $member_pid = preg_replace('/info:fedora\//', '', $member['pid']['value']);
    $members[$ns_prefix.$int] = $member_pid;
    $int = $int + 1;
  }
  foreach($members as $coll_pid){
    //Filters out fedora system object cmodel +1
    $query_cm = <<<EOL
SELECT ?cmodel ?item
FROM <#ri>
WHERE { ?item <info:fedora/fedora-system:def/model#hasModel> ?cmodel .
?item <info:fedora/fedora-system:def/model#state> <fedora-model:Active> .
?item <fedora-rels-ext:isMemberOfCollection> <info:fedora/$coll_pid> .
FILTER (!sameterm(str(?cmodel), "info:fedora/fedora-system:FedoraObject-3.0"))
}
EOL;
    $results_model = $tuque->repository->ri->sparqlQuery($query_cm, 'unlimited');//sparqlQuery($query_cm, 'unlimited');
  }

    $models = [];
    foreach($results_model as $item ){
        $prefix =explode('-',$item['item']['value']);
        $models[$item['item']['value'].' '.$item['cmodel']['value']] = array();
    }
    dpm($models);
    foreach($models as $ctype){
      foreach($results_model as $item ){
        if($item['cmodel']['value'] == $ctype){ //&& explode(':',$item['item']['value'])
          array_push($models[$ctype],$item['item']['value']);
        }
      }
    }
    $cmodel_collection = [];
    foreach($models as $key => $value){

      }
      return $cmodel_collection;
}

/**
 * SPARQL that queries root for all existing collections
 * containing institutional prefix string
 */
function all_institution_cmodel_query_count(){
  $tuque = islandora_get_tuque_connection();
  $institutions = update_namespace_prefixes_cache();
  $institution_coll_counts = [];
  foreach($institutions as $lib){
    $query = <<<EOQ
SELECT ?pid
FROM <#ri>
WHERE {
{?pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root>}
FILTER regex(str(?pid), "$lib")
}
ORDER BY ?pid
EOQ;
    $result =  $tuque->repository->ri->countQuery($query, 'sparql');
    $institution_coll_counts[$lib] = $result;
  }
  $coll_models = [];
  foreach($institution_coll_counts as $key => $val){
    $coll_models[$key] =  institution_cmodel_query($key);
  }
  return $coll_models;
}

/**
 * SPARQL that queries root for all existing collections
 * different from above. not a count query. store all the collections then use later
 * containing institutional prefix string
 * WIP
 */
function all_institution_each_collection_query_count(){
  $tuque = islandora_get_tuque_connection();
  $institution_colls = [];
  $institutions = update_namespace_prefixes_cache();
  foreach($institutions as $lib){
    $query = <<<EOQ
SELECT ?pid
FROM <#ri>
WHERE {
{?pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root>}
FILTER regex(str(?pid), "$lib")
}
ORDER BY ?pid
EOQ;
    $coll_results =  $tuque->repository->ri->sparqlQuery($query, 'sparql');
    foreach($coll_results as $key => $coll){
    $institution_colls[$lib.$key] = $coll['pid']['value'];
    }
  }
  $coll_models = [];
  foreach($institution_colls as $inst_num => $collection){
    $coll_count_results = collection_cmodel_query($collection);
    foreach($coll_count_results as $collection => $array){
        foreach($array as $key => $model_count){
            foreach($model_count as $model => $count){
                $coll_models[preg_replace('/\d/','',$inst_num)." : ".$collection." : ".$model] = $count;
            }
        }

    }
  }
  return $coll_models;
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
  foreach($q_time as $return){
    $last_q_run = (isset ($return) ? intval($return->{'max(timestamp)'}) : 0);
  }
  $last_q_plus_interval = $interval + $last_q_run;
  if(time() >= $last_q_plus_interval){
    islandora_content_stats_test();
  }
}

// save the results to a database table (with a timestamp)
function islandora_content_stats_test(){
    //echo('populating db w/ queries');
    $all_models = all_cmodel_counts();
    foreach($all_models as $model){
        db_insert('islandora_content_stats')
        ->fields(array('id', 'query', 'count', 'timestamp'))
        ->values(array(
          'id' => NULL,
          'query' => $model['cmodel']['value'],
          'count' => intval($model['k0']['value']),
          'timestamp' => time(),
        ))
        ->execute();
    }
    $inst_models = all_institution_cmodel_query_count();
    foreach($inst_models as $key1 => $val1){
        if(isset($val1)){
            foreach($val1 as $key2 => $val2){
                    db_insert('islandora_content_stats')
        ->fields(array('id', 'query', 'count', 'timestamp'))
        ->values(array(
          'id' => NULL,
          'query' => $key1 . " " . $key2 ,
          'count' => $val2,
          'timestamp' => time(),
        ))
        ->execute();

            }
        }
    }
    $inst_coll_cmodels = all_institution_each_collection_query_count();
    foreach($inst_coll_cmodels as $key => $val){
        db_insert('islandora_content_stats')
        ->fields(array('id','query','count','timestamp'))
        ->values(array(
            'id'=> NULL,
            'query'=> $key,
            'count'=> $val,
            'timestamp'=> time(),
        ))
        ->execute();
    }
 }


 function islandora_content_stats_page_cmodels($path){
   $last_query_time_db = db_query('select max(timestamp) from ldl.islandora_content_stats');
   $last_q_time = '';
   foreach($last_query_time_db as $return){
     $last_q_time = (isset ($return) ? $return->{'max(timestamp)'} : 0);
   }
   $query ="select * from ldl.islandora_content_stats where query like ";
   $query_suf = " and timestamp = ";
   $result = db_query($query .  '\''. '%'. $path . '%'. '\'' . $query_suf . '\'' .$last_q_time . '\'');
   $results = $result->fetchAll();
   $display = [];
   foreach($results as $query_r){
     $split_q = explode(' ', $query_r->query);
     if($path == $split_q[0]){
     if(!strpos($query_r->query,'fedora-system')){
       $display[$query_r->query] = $query_r->count;
     }
   }
   }
   //dpm($display);
   return $display;
 }
