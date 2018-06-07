<?php
/**
 * Builds form out of query results from db with latest timestamp.
 * accounts for custom content models
 */
function islandora_content_stats_form($form, &$form_state){
  module_load_include('inc', 'islandora_content_stats','/includes/queries');
  module_load_include('inc', 'islandora_content_stats','/includes/utilities');
  $rows = [];
  $headers = array(
    array('data'=> t('ID'),'field' =>'stats.id','sort' => 'desc'),
    array('data' => t('Collection'), 'field' =>'stats.coll',),
    array('data' => t('Content Model'), 'field' =>'stats.cmodel',),
    array('data'=> t('Count'),'field' =>'stats.count',),
    array('data'=> t('Timestamp'),'field' =>'stats.timestamp',),
  );
  $max_timestamp = db_query('select MAX(timestamp) from islandora_content_stats');
  //second argument in db select is an alias for the table (which is the first argument.)
  $query = db_select("islandora_content_stats","stats")
    ->extend('TableSort');
  $query = $query->fields('stats', array('id','coll','cmodel','count','timestamp'));
  $results = apply_query_filters($form_state, $query, $max_timestamp, $headers);
  foreach($results as $record){
    if($record->count != 0){
      if( ($record->cmodel != 'fedora-system:FedoraObject-3.0') && ($record->cmodel !='fedora-system:ContentModel-3.0') && $record->coll != 'islandora:root' ){
        $rows[$record->id] = array(
          'id' => $record->id,
          'coll' => ($record->coll == '' ? 'All' : map_collection($record->coll,'form')),
          'cmodel' => map_cmodels($record->cmodel),
          'count' => $record->count,
          'timestamp' => gmdate("Y-m-d\TH:i:s\Z",$record->timestamp),
        );
      }
    }
  }
  $total_counts = [];
  $timestamp = db_query('select MAX(timestamp) from islandora_content_stats');
  $total_query = db_select("islandora_content_stats","total");
  $totals = $total_query->fields('total', array('id','cmodel','count','timestamp'))
    ->condition('coll', 'islandora:root')
    ->condition('timestamp', $timestamp->fetchField())
    ->execute();
  foreach($totals as $total){
    if($total->count != 0){
      if( ($total->cmodel != 'fedora-system:FedoraObject-3.0') && ($total->cmodel !='fedora-system:ContentModel-3.0')){
        $total_counts[$total->id] = array(
          'cmodel' =>  map_cmodels($total->cmodel),
          'count' => $total->count,
        );
      }
    }
  }
  $form = [];
  $form['totals']['item'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#title' => t('Totals for each content model'),
  );
  foreach($total_counts as $id => $count){
    $form['totals']['item'][$id] = array(
      '#type' => 'item',
      '#title' => t($count['cmodel'].' Total: '.$count['count']),
    );
  }
  $form['filter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter results by type or collection membership'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  $form['filter']['cmodel'] = array(
    '#type' => 'select',
    '#title' => t('Filter by type of object'),
    '#options' => cmodel_types_for_form(),
    '#description' => t('Shows results to that match a single content model.'),
    '#required' => FALSE,
    '#empty_option' => '--none--',
  );
  $form['filter']['collection'] = array(
    '#type' => 'select',
    '#title' => t('Filter by specific collection'),
    '#description' => t('Shows counts of object types for an individual collection.'),
    '#options' => collection_pid_labels_for_form(),
    '#required' => FALSE,
    '#empty_option' => '--none--',
  );
  $form['filter']['all'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show all results (history)')
  );
  $form['filter']['filtertitle'] = array(
    '#type' => 'item',
    '#title' => t('Press to apply filters to page results.'),
  );
  $form['filter']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
  );

  $form['table-collapse'] = array(
    '#type' => 'fieldset',
    '#collapsible' => TRUE,
    '#title' => t('Table of results'),
  );
  $form['table-collapse']['explain'] = array(
    '#type' => 'item',
    '#title' => t('Content from collections, can be filtered by choosing from drop downs above, and pressing the "Filter" button.
(not every choice combination will return results)
Sort by ID, Collection, Content Model, Count, or Timestamp by clicking headers below.'),
  );
  $form['table-collapse']['table'] = array(
    '#theme' => 'table',
    '#header' => $headers,
    '#rows' => $rows,
    '#empty' => t('table is empty')
  );
  $form['table-collapse']['pager'] = array('#markup' => theme('pager'));
  $form['download']['dltitle'] = array(
    '#type' => 'item',
    '#title' => t('Downloads results for use in a spreadsheet. (always applies current filters)'),
  );
  $form['download']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Download as CSV'),
    '#submit' => array('download_data'),
  );
  return $form;
}

/**
 * takes filters from the form and applies them to database conditions
 * inputs $form_state has filter values
 * $mysqlpart is passed to either do nothing or add conditions to the $query
 * $timestamp is applied only if recent queries are requested,
 * $headers are required for ordering of the form
 * returns an executed mysql query
 */
function apply_query_filters($form_state, $mysqlpart, $timestamp, $headers){
  if(!isset($form_state['input']['all'])){
    $mysqlpart = $mysqlpart->condition('timestamp', $timestamp->fetchField());
  }
  if(isset($form_state['input']['collection'])){
    $filters = [];
    $filters['coll'] = $form_state['input']['collection'];
    $filters['cmodel'] = $form_state['input']['cmodel'];
    foreach($filters as $key => $val){
      if($val == ''){
        //do nothing to the sqlpart
      }
      elseif($val == 'All'){
        //'All' is same as NULL in mysql field
        $mysqlpart = $mysqlpart->isNull($key);
      }
      else{
        //apply filter if not '' or 'All'
        $mysqlpart = $mysqlpart->condition($key, $val);
      }
    }
  }
  return $mysqlpart->orderByHeader($headers)
    ->extend('TableSort')->extend('PagerDefault')->limit(144)
    ->execute();
}

/**
 * sends the filter options to the islandora_content_stats_csv_download function
 * sent as url parameters to page data/download
 */
function download_data(&$form,&$form_state){
  $options = array('query' => array('cmodel' => $form_state['values']['cmodel'], 'coll' => $form_state['values']['collection'],'all' => $form_state['values']['all']));
  drupal_goto('data/download', $options);
}

function islandora_content_stats_form_submit($form, &$form_state){
  $form_state['rebuild'] = TRUE;
}
