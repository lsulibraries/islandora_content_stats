<?php  
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

//list of content models as variables (unused...)
$audio = "<info:fedora/islandora:sp-audioCModel>";
$large_image = "<info:fedora/islandora:sp_large_image_cmodel>";
$compound = "<info:fedora/islandora:compoundCModel>";
$books = "<info:fedora/islandora:bookCModel>";
$video = "<info:fedora/islandora:sp_videoCModel>";
$collection = "<info:fedora/islandora:collectionCModel>";
$pdf = "<info:fedora/islandora:sp_pdf>";
$newspaper = "<info:fedora/islandora:newspaperCModel>";
$newspage = "<info:fedora/islandora:newspaperPageModel>";
$newsissue = "<info:fedora/islandora:newspaperIssueCModel>";
$oralhist = "<info:fedora/islandora:sp-ohCModel>";
$remote = "<info:fedora/islandora:sp_remote_resource>";

/**
 * Query the RI for a count of a single individual content models
 */
function cmodel_query($model) {
  $tuque = islandora_get_tuque_connection();
  $members = array();
  $model_key = preg_replace('/>/','', preg_replace('/</', '',$model));
  
  $query = <<<EOL
select count(select \$item from <#ri>
where \$item <info:fedora/fedora-system:def/model#hasModel>;
$model and \$item <info:fedora/fedora-system:def/model#state> <fedora-model:Active>)
from <#ri>
where \$item <info:fedora/fedora-system:def/model#hasModel> $model
having \$k0 <http://mulgara.org/mulgara#occursMoreThan> '0.0'^^<http://www.w3.org/2001/XMLSchema#double> ;";
EOL;
  
  $result = $tuque->repository->ri->itqlQuery($query, 'unlimited');
  foreach ($result as $member) {
    $count =  [$model_key, $member['k0']['value']];
    //dpm($count);
  }
  return $count;
}

/**
 * Query the ri for a count of a collection's content models
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
  $result = [];
  $result[0] = $collection;
  $result[1] = $tuque->repository->ri->itqlQuery($query, 'unlimited');
  return $result;
}

/**
 * SPARQLQuery the ri for the content models and counts within an institution
 * used by all_institution_cmodel_query(=
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
function all_institution_cmodel_query(){
  $institutions = ['lsm','lsu-','lsus-','lsuhsc-','lsuhscs-','latech','loyno','mcneese','nicholls','nsu','subr','state','hnoc','tulane','ull','ulm','uno','vville'];
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
    $coll_models[$key] =  institution_cmodel_query($key);
  }
  return $coll_models;
}