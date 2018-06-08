<?php

/**
 * @file
 * the RI (ITQL & SPARQL) queries
 * for aggregated SPARQL queries use ->countQuery() on the tuque connection
 */

module_load_include('php', 'islandora_namespace_homepage', 'includes/namespaces');
module_load_include('php', 'islandora_namespace_homepage', 'includes/db');
module_load_include('php', 'islandora_namespace_homepage', 'includes/utilities');


/**
 * Query repository index for all cmodels that exist, are active and
 * Special thanks to rebecca.s.koeser@princeton.edu for this query
 * @link https://rlskoeser.github.io/2010/04/06/fedora-risearch-query-get-object-totals-cmodel query. @endlink
 */

function all_cmodels() {
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

/**
 * @return array $return
 * array of mapped cmodel labels
 * puts into dropdown for filter on data.form.inc
 */

function cmodel_types_for_form() {
  module_load_include('inc', 'islandora_content_stats', 'includes/utilities');
  $model_counts = all_cmodels();
  $return = array();
  foreach($model_counts as $model) {
    $model = $model['cmodel']['value'];
    //when cmodel is FedoraObject or ContentModel, or collectionCModel don't map
    $not_fobj = $model != 'fedora-system:FedoraObject-3.0';
    $not_cmobj = $model != 'fedora-system:ContentModel-3.0';
    if ( ($not_fobj) && ($not_cmobj) ) {
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

function all_ns_prefix_incl_subs() {
  $tuque = islandora_get_tuque_connection();
  $prefixes = array();
  $all_root_members = <<<EOL
SELECT DISTINCT ?pid
FROM <#ri>
WHERE
{?pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root>}
EOL;
  $result = $tuque->repository->ri->sparqlQuery($all_root_members, 'unlimited');
  foreach($result as $key => $val) {
    $value = $val['pid']['value'];
    if ( (isset($value)) && (strpos($value, '-')) ) {
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
  $prefixes = all_ns_prefix_incl_subs();
  $return = array();
  foreach($prefixes as $inst) {
    $return[$inst.'-'] = map_institution($inst, 'form');
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
  $sanitized_array = array();
  foreach($cmodel_counts as $count) {
    $cmodel = $count['cmodel']['value'];
    $count = $count['k0']['value'];
    if ($cmodel != 'fedora-system:FedoraObject-3.0') {
      $sanitized_array[$cmodel] = $count;
    }
  }
  $result[$collection] = $sanitized_array;
  return $result;
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
  foreach($collections as $coll) {
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
  $institutions = all_ns_prefix_incl_subs();
  foreach($institutions as $lib) {
    $lib = check_prefix($lib);
    $query = <<<EOQ
SELECT ?pid
FROM <#ri>
WHERE {
{?pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root>}
FILTER regex(str(?pid), '$lib')
}
ORDER BY ?pid
EOQ;
    $coll_results =  $tuque->repository->ri->sparqlQuery($query, 'unlimited');
    foreach($coll_results as $num => $coll) {
    $institution_colls[$lib.$num] = $coll['pid']['value'];
    }
  }
  foreach($institution_colls as $inst_num => $collection) {
    $coll_count_results = collection_cmodel_query($collection);
    foreach($coll_count_results as $collection => $array) {
      foreach($array as $model => $count) {
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
 * @param string $ns_prefix
 *  prefix of institution or sub-institution
 * @return array
 *  array of an institution's total of items with a particular content model.
 */

function institution_cmodel_totals($ns_prefix) {
  $inst_cmodel_counts = array();
  $cmodels = all_cmodels();
  foreach($cmodels as $model) {
    $model = $model['cmodel']['value'];
    $not_cmobj = $model != 'fedora-system:ContentModel-3.0';
    $not_coll = $model != 'islandora:collectionCModel';
    if ( ($not_cmobj) || ($not_coll) ) {
      $inst_cmodel_counts[$model] = 0;
    }
  }
  $tuque = islandora_get_tuque_connection();
  //first query gets all the collections that contain the ns_prefix string.
  $ns_prefix = check_prefix($ns_prefix);
  $ns_member_q = <<<EOQ
  SELECT ?pid
  FROM <#ri>
  WHERE {
  {?pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root>}
  FILTER regex(str(?pid), '$ns_prefix')
  }
  ORDER BY ?pid
EOQ;

  $member_colls= $tuque->repository->ri->sparqlQuery($ns_member_q, 'unlimited');
  $inst_cmodel_counts['islandora:collectionCModel'] = $tuque->repository->ri->countQuery($ns_member_q, 'sparql');
  foreach($member_colls as $coll) {
    $coll = $coll['pid']['value'];
    //for each cmodel, count members of each collection
    foreach($cmodels as $model) {
      $model = $model['cmodel']['value'];
      $not_coll = $model != 'islandora:collectionCModel';
      if ($not_coll) {
        $query_cm = <<<EOL
      SELECT ?cmodel ?item
      FROM <#ri>
      WHERE {{ ?item <info:fedora/fedora-system:def/model#hasModel> ?cmodel .
      ?item <info:fedora/fedora-system:def/model#state> <fedora-model:Active> .
      ?item <fedora-rels-ext:isMemberOfCollection> <info:fedora/$coll> .}
      FILTER (!sameterm(str(?cmodel), '$model'))}
EOL;
        $coll_cmodel_items =  $tuque->repository->ri->sparqlQuery($query_cm, 'unlimited');
      }
        foreach ($coll_cmodel_items as $item_model) {
          $not_coll = $item_model['cmodel']['value'] != 'islandora:collectionCModel';
          if ($not_coll) {
            $item_model = $item_model['cmodel']['value'];
            $inst_cmodel_counts[$item_model] = $inst_cmodel_counts[$item_model] + 1;
          }
        }
      }
    }
  return $inst_cmodel_counts;
}

/**
 * saves the results to a database table (with a timestamp)
 * manually runs the queries now.
*/

function islandora_content_stats_run_queries() {
  variable_set('islandora_content_stats_button', 'cron');
    $time = time();
    $all_cmodels = all_cmodels();
    foreach($all_cmodels as $model) {
        db_insert('islandora_content_stats')
        ->fields(array('id', 'inst', 'coll', 'cmodel', 'count', 'timestamp'))
        ->values(array(
          'id' => NULL,
          'inst' => NULL,
          'coll' =>  'islandora:root',
          'cmodel' => $model['cmodel']['value'],
          'count' => $model['k0']['value'],
          'timestamp' => $time,
        ))
        ->execute();
    }
    $ns_prefixes = all_ns_prefix_incl_subs();
    foreach($ns_prefixes as $ns) {
    $inst_model_counts = institution_cmodel_totals($ns);
      foreach($inst_model_counts as $model => $count) {
        if (strpos($model, 'islandora:collectionCModel')) {
          db_insert('islandora_content_stats')
            ->fields(array('id', 'inst', 'coll', 'cmodel', 'count', 'timestamp'))
            ->values(array(
              'id' => NULL,
              'inst' => $ns.'-',
              'coll' => NULL,
              'model' => $model,
              'count' => $count,
              'timestamp' => $time,
            ))
            ->execute();
        }
        else {
          db_insert('islandora_content_stats')
            ->fields(array('id', 'inst', 'coll', 'cmodel', 'count', 'timestamp'))
            ->values(array(
              'id' => NULL,
              'inst' => $ns.'-',
              'coll' => NULL,
              'cmodel' =>$model,
              'count' => $count,
              'timestamp' => $time,
            ))
            ->execute();
        }
    }
  }
    $inst_coll_counts = all_institution_each_collection_query_count();
    foreach($inst_coll_counts as $inst) {
      foreach($inst['models'] as $model => $count) {
            db_insert('islandora_content_stats')
              ->fields(array('id', 'inst', 'coll', 'cmodel', 'count', 'timestamp'))
              ->values(array(
                'id'=> NULL,
                'inst'=> $inst['inst'],
                'coll' => $inst['coll'],
                'cmodel' => $model,
                'count'=> $count,
                'timestamp'=> $time,
              ))
            ->execute();
    }
  }
}

/**
 * makes sure queries for ns are distinct
 * otherwise lsu matches lsu-sub
 * @param string $ns_prefixe
 * institution string to check for '-'
*/

function check_prefix($ns_prefix) {
  if (strpos($ns_prefix, '-') === FALSE) {
    $ns_prefix = $ns_prefix.'-';
  }
  return $ns_prefix;
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
  foreach($results as $result) {
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
  $query = db_query($pref . $path . '\'' . $suff . $time . $end );
  $results = $query->fetchAll();
  $display = array();
  foreach($results as $result) {
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
  $ns_prefixes = all_ns_prefix_incl_subs();
  $last_time = db_query('select max(timestamp) from ldl.islandora_content_stats');
  $time = $last_time->fetchField();
  $pref = 'select * from ldl.islandora_content_stats where inst is NULL and timestamp = \'';
  $suff = '\' and cmodel not like \'fedora-system%\'';
  $query = db_query($pref . $time . $suff);
  $results = $query->fetchAll();
  $display = array();
  foreach($results as $result) {
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
