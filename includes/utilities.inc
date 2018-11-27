<?php

/**
 * @file
 * place to store utility functions for mapping readable lables
 */

/**
 * Maps a machine name to a human readable $label
 * @param string $model
 *  String of model's machine name
 * @return string
 *  String of model's label stripped of repetitous words
 */
function map_cmodels($model) {
  module_load_include('inc','islandora','includes/utilities');
    if($model == 'children of compound object types'){return $model;}
    module_load_include('inc', 'islandora', 'includes/utilities');
    $not_cmobj = $model != 'fedora-system:ContentModel-3.0';
    $not_fobj = $model != 'fedora-system:FedoraObject-3.0';
    if (($not_fobj) && ($not_cmobj)) {
        $cmodel_map = islandora_content_stats_islandora_get_content_models();
        if ($model != '' && array_key_exists($model, $cmodel_map)) {
            $label = $cmodel_map[$model];
            $replace = $label['label'];
            $l_replace = str_replace('Large ', '', $replace);
            $i_replace = str_replace('Islandora ', '', $l_replace);
            $short_label = str_replace(' Content Model', '', $i_replace);
            return $short_label;
        }
        return $model;
    }
    return $model;
}

/**
 * Maps an institution prefix to an intitution's title
 * @param string $inst
 * String of institutional prefix
 * @param string $form
 *  string indicates function is used for form dropdown
 * @return string
 * String of institution's title
 */
function map_institution($inst) {
  if ($inst == variable_get('islandora_repository_pid', 'islandora:root')) {
    return $inst;
  }
  else if (module_exists('islandora_namespace_homepage')) {
    $qargs = [':inst' => rtrim($inst, '-')];
    $query = 'select * from {islandora_namespace_homepage} where prefix = :inst';
    $ns_homepage_table = db_query($query, $qargs);
    foreach ($ns_homepage_table as $row) {
      if ($row->title == '') {
        return $inst;
      }
      return $row->title;
    }
    // if there are no hits in the db:
    return $inst;
  }
  else {
    return $inst;
  }
}

/**
 * Maps a collection pid to a collection label
 * @param string $pid
 * String of collection pid
 * @param string $context
 *  string indicates function is used for display in a form, or csv download.
 * @return string
 * String of institution's title with link to item
 */
function map_collection($pid, $context = FALSE) {
  //throw new Exception ("This function is not intended to be active ATM!");
    $iobj_str = '/islandora/object/';
    if ($pid != '') {
        $obj = islandora_object_load($pid);
        if (isset($obj) && gettype($obj) == 'object') {

            $label = $obj->label;
            if ($context == 'csv') {
                //add port number after $_SERVER['SERVER_NAME'] if running locally
                $link = 'http://' . $_SERVER['SERVER_NAME'] . $iobj_str . $pid;
            } elseif ($context == 'form') {
                $link = '<a href = "../..' . $iobj_str . $pid . '">' . $label . '</a>';
            } else {
                $link = $label;
            }
            return $link;
        }
        else{
            return $pid;
        }
    }
}

/**
 *  @param string $model
 *  name of a content model
 *  @return bool $return
 */
function cmodel_check($model) {
    $check_against = ['fedora-system:FedoraObject-3.0', 'islandora:collectionCModel'];
    if (!in_array($model, $check_against)) {
        return true;
    }
}

function global_totals() {
    $rows = array();
    $time = latest_run();
    $query = db_select('islandora_content_stats','stats')
            ->fields('stats', array('id','inst','coll','cmodel','count','timestamp'))
            ->condition('timestamp', $time)
            ->condition('inst', NULL)
            ->orderBy('cmodel', 'ASC')
            ->execute();
   $totals = $query->fetchAll();
   foreach ($totals as $record) {
        //does not build row when query's cmodel is fedora-Object or ContentModel
        $not_fobj = $record->cmodel != 'fedora-system:FedoraObject-3.0';
        $not_cmobj = $record->cmodel != 'fedora-system:ContentModel-3.0';
        //does not build row when query's inst is is_null
        if ($record->count != 0) {
            if (($not_fobj) && ($not_cmobj)) {
                $rows[$record->id] = array(
                    'id' => $record->id,
                    'coll' => ($record->coll == '' ? 'All' : map_collection($record->coll, 'form')),
                    'cmodel' => map_cmodels($record->cmodel),
                    'cmodel-id' => $record->cmodel,
                    'count' => $record->count,
                    'timestamp' => gmdate('Y m d \TH:i:s\Z', $record->timestamp),
                );
            }
        }
    }
  return $rows;
}



function inst_totals() {
    module_load_include('inc','islandora_content_stats','includes/queries');
    $prefixes = get_all_prefixes();
    $rows = array();
    $time  = latest_run();
    foreach($prefixes as $inst){
        $query = db_select('islandora_content_stats','stats')
                ->fields('stats', array('id','inst','cmodel','count','timestamp'))
                ->condition('timestamp', $time)
                ->condition('coll', NULL)
                ->condition('inst', $inst.'-')
                ->orderBy('cmodel', 'ASC')
                ->execute();
       $totals = $query->fetchAll();
       foreach ($totals as $record) {
            //does not build row when query's cmodel is fedora-Object or ContentModel
            $not_fobj = $record->cmodel != 'fedora-system:FedoraObject-3.0';
            $not_cmobj = $record->cmodel != 'fedora-system:ContentModel-3.0';
            //does not build row when query's inst is is_null
            if ($record->count != 0) {
                if (($not_fobj) && ($not_cmobj)) {
                    $rows[$inst][] = array(
                        'id' => $record->id,
                        'inst-label' => ($record->inst == '' ? 'All' : map_institution($record->inst)),
                        'inst' => $record->inst,
                        'cmodel' => map_cmodels($record->cmodel),
                        'count' => $record->count,
                        'timestamp' => gmdate('Y m d \TH:i:s\Z', $record->timestamp),
                    );
                }
            }
        }
    }
  return $rows;
}

function conditionals_for_table($inst = FALSE, $coll = FALSE, $cmodel = FALSE, $time, $order = FALSE, $sort = FALSE){
  $query = db_select('islandora_content_stats','stats')
    ->fields('stats',array('id','inst','coll','cmodel','count','timestamp'))
    ->condition('timestamp', $time);
  if($inst) {
    $query->condition('inst',$inst.'-');
  }
  if($coll) {
    $query->condition('coll',$coll);
  }
  if($cmodel) {
    $query->condition('cmodel',$cmodel);
  }
  if($order && $sort){
    $query->orderBy($sort, $order);
  }
  else {
    $query->orderBy('inst', 'asc');
    $query->orderBy('cmodel', 'asc');
  }
  $totals = $query->execute();
  return $totals;
}


function latest_counts_for_table($pageParams) {
  $rows = [];
  $order = $pageParams['order'];
  $sort = $pageParams['sort'];
  $inst = isset($pageParams['inst']) ? $pageParams['inst']: FALSE;
  $cmodel = isset($pageParams['cmodel']) ? $pageParams['cmodel'] : FALSE;
  $coll = FALSE;
  module_load_include('inc','islandora_content_stats','includes/data.form');
  module_load_include('inc','islandora_content_stats','includes/queries');
  $time  = latest_run();
  $totals = conditionals_for_table($inst, $coll, $cmodel, $time, $order, $sort);
  foreach ($totals as $record) {
    //does not build row when query's cmodel is fedora-Object or ContentModel
    $not_fobj = $record->cmodel != 'fedora-system:FedoraObject-3.0';
    $not_cmobj = $record->cmodel != 'fedora-system:ContentModel-3.0';
    $not_global = $record->inst != NULL;
    //does not build row when query's inst is is_null
    if ($record->count != 0) {
      if (($not_fobj) && ($not_cmobj) && ($not_global)) {
        $inst = rtrim($record->inst, '-');
          $rows[] = array(
            'id' => $record->id,
            'inst-id' => $inst,
            'inst-label' => ($inst == '' ? 'All' : map_institution($inst)),
            'coll-label' => ($record->coll == '' ? 'All' : map_collection($record->coll)),
            'coll' => $record->coll,
            'cmodel-id' => $record->cmodel,
            'cmodel-label' => map_cmodels($record->cmodel),
            'count-label' => $record->count,
            'timestamp' => gmdate('Y m d \TH:i:s\Z', $record->timestamp),
          );
      }
    }
  }
  return tablesort($rows,$sort,$order);
}


function tablesort($rows, $sort, $order){
  $sortField = "$sort-label";
  $rows2Lower = [];
  foreach ($rows as $row) {
    $row['inst-label'] = strtolower($row['inst-label']);
    $rows2Lower[] = $row;
  }

  if (!$sort) {
    array_multisort(
        array_column($rows2Lower, 'inst-label'), SORT_ASC, SORT_STRING,
        array_column($rows2Lower, 'cmodel-label'), SORT_ASC, SORT_STRING,
        $rows
      );
    return $rows;
  }
  $orderMethod = $order === 'asc' ? SORT_ASC : SORT_DESC;
  $sortMethod  = $sortField === 'count-label' ? SORT_NUMERIC : SORT_STRING;
  array_multisort(array_column($rows2Lower, $sortField), $orderMethod, $sortMethod, $rows);
  return $rows;
}

function latest_run() {
    $maxtime = db_query('select max(timestamp) from {islandora_content_stats}');
    $time = $maxtime->fetchField();
    return $time;
}

/**
 * @return array $return
 * array of mapped cmodel labels
 * puts into dropdown for filter on data.form.inc
 */

function cmodel_types_for_form() {
  module_load_include('inc','islandora','includes/utilities');
    $model_counts = array_keys(islandora_content_stats_islandora_get_content_models());
    $options = array();
    foreach ($model_counts as $model) {
        //when cmodel is FedoraObject or ContentModel, don't map
        $not_fobj = $model != 'fedora-system:FedoraObject-3.0';
        $not_cmobj = $model != 'fedora-system:ContentModel-3.0';
        if (($not_fobj) && ($not_cmobj)) {
            $string = map_cmodels($model);
            $options[$model] = t($string);
        }
    }
    asort($options);
    return $options;
}

/**
 * @return array $return
 * mapped names of institutions as array
 * puts into dropdown for filter on data.form.inc
 */
function institutions_for_form() {
    $prefixes = get_all_prefixes();
    $options = array();
    foreach ($prefixes as $inst) {
        $options[$inst] = map_institution($inst, 'form');
    }
    asort($options);
    return $options;
}


/**
 * @return array $return
 * options for data.form.inc dropdown of colleciton names
 */
function collection_pid_labels_for_form() {
  throw new Exception ("This fn is not in use ATM!");
    $return = array();
    $tuque = islandora_get_tuque_connection();
    $query = <<<EOL
SELECT ?pid
FROM <#RI>
WHERE {
?pid <fedora-rels-ext:isMemberOfCollection> <info:fedora/islandora:root> }
EOL;
    $collections = $tuque->repository->ri->sparqlQuery($query, 'unlimited');
    foreach ($collections as $coll) {
        $coll = $coll['pid']['value'];
       $return[$coll] = map_collection($coll);
    }
    $return['All'] = 'All';
    return $return;
}
