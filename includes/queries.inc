<?php  
module_load_include('php','islandora_namespace_homepage','includes/namespaces');
module_load_include('php','islandora_namespace_homepage','includes/db');
module_load_include('php','islandora_namespace_homepage','includes/utilities');
// the RI (ITQL & SPARQL) queries 
// 
// 
  $institutions = ['lsm','lsu-','lsus-','lsuhsc-','lsuhscs-','latech','loyno','mcneese','nicholls','nsu','subr','state','hnoc','tulane','ull','ulm','uno','vville'];
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
  foreach ($result as $member) {
    $member_pid = preg_replace('/info:fedora\//', '', $member['pid']['value']);
    $members[$ns_prefix] = $member_pid;
  }
  foreach($members as $coll_pid){
    //same query as collection_cmodel_query but in sparql
    $query_cm = <<<EOL
SELECT ?cmodel ?item
FROM <#ri>
WHERE { ?item <info:fedora/fedora-system:def/model#hasModel> ?cmodel . 
?item <info:fedora/fedora-system:def/model#state> <fedora-model:Active> .
?item <fedora-rels-ext:isMemberOfCollection> <info:fedora/$coll_pid> .
}
EOL;
    $results = $tuque->repository->ri->sparqlQuery($query_cm, 'unlimited');
  }
  //check that the institution exists
  if(isset($results)){
    $models = [];
    foreach($results as $item ){ 
      if($item['cmodel']['value'] != 'fedora-system:FedoraObject-3.0'){
        $models[$item['cmodel']['value']] = array();
      }
    }
    foreach($models as $ctype => $value){
      foreach($results as $item ){
        if($item['cmodel']['value'] == $ctype){
          array_push($models[$ctype],$item['item']['value']);
        }
      }
    }
    $cmodel_collection = [];
    foreach($models as $key => $value){
      if($value){
        $cmodel_collection[$key] = count($value);  }
      }
      return $cmodel_collection;
  }
}

/**
 * SPARQL that queries root for all existing collections
 * containing institutional prefix string
 */
function all_institution_cmodel_query_count($institutions){
  $tuque = islandora_get_tuque_connection();
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
      dpm($val);
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


/**
 * Implements hook_cron().
 *
 * periodically run a set of queries
 * save the results to a database table (with a timestamp)
 */
function islandora_content_stats_cron() {
    islandora_content_stats_test();
}

function islandora_content_stats_test(){
    echo('populating db w/ queries');
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
                    db_insert('islandora_con,taoeuent_stats')
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