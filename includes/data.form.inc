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
    array('data'=> t('Instituton/Context'), 'field' =>'stats.inst',),
    array('data' => t('Collection (optional)'), 'field' =>'stats.coll',),
    array('data' => t('Content Model'), 'field' =>'stats.cmodel',),
    array('data'=> t('Count'),'field' =>'stats.count',),
    array('data'=> t('Timestamp'),'field' =>'stats.timestamp',),
  );
  $max_timestamp = db_query('select MAX(timestamp) from ldl.islandora_content_stats');
  //second argument in db select is an alias for the table (which is the first argument.)
  $query = db_select("islandora_content_stats","stats")
    ->extend('TableSort');
  $query = $query->fields('stats', array('id','inst','coll','cmodel','count','timestamp'));
  $results = apply_query_filters($form_state, $query, $max_timestamp, $headers);
  foreach($results as $record){
    if($record->count != 0){
      if( ($record->cmodel != 'fedora-system:FedoraObject-3.0') && ($record->cmodel !='fedora-system:ContentModel-3.0') && ($record->inst != NULL) ){
        $rows[$record->id] = array(
          'id' => $record->id,
          'inst' => ($record->inst == '' ? 'All' : map_institution($record->inst)),
          'coll' => ($record->coll == '' ? 'All' : map_collection($record->coll,'form')),
          'cmodel' => map_cmodels($record->cmodel),
          'count' => $record->count,
          'timestamp' => gmdate("Y-m-d\TH:i:s\Z",$record->timestamp),
        );
      }
    }
  }
  //I cannot explain why but I could not re-use $query or $max_timestamp after using them above.
  //Multiple tables on the same page cannot be sorted independently, refer to: https://www.drupal.org/node/94058
  //Putting the totals into prose is better
  $total_counts = [];
  $timestamp = db_query('select MAX(timestamp) from ldl.islandora_content_stats');
  $total_query = db_select("islandora_content_stats","total");
  $totals = $total_query->fields('total', array('id','inst','cmodel','count','timestamp'));
    $totals_cond = $totals->condition('inst', NULL)
    ->condition('timestamp', $timestamp->fetchField())
    ->execute();
  foreach($totals_cond as $total){
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
    '#title' => t('<br>Filter results by type, ownership, or collection name'),
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
  $form['filter']['institution'] = array(
    '#type' => 'select',
    '#title' => t('Filter according to ownership by institution or sub-institution'),
    '#options' => institutions_for_form(),
    '#required' => FALSE,
    '#description' => t('Shows results for any items owned by chosen institution or sub-institution.'),
    '#empty_option' => '--none--',
  );
  $form['filter']['collection'] = array(
    '#type' => 'select',
    '#title' => t('Filter by specific Collection'),
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
    '#title' => t('<br>Table of results'),
  );
  $form['table-collapse']['explain'] = array(
    '#type' => 'item',
    '#title' => t('<br>Content from institutions and collections, can be filtered by choosing from drop downs above, and pressing the "Filter" button.<br>
(not every choice combination will return results)<br>
Sort by ID, Institution/Context, Collection, Content Model, Count, or Timestamp by clicking headers below.'),
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
    '#title' => t('<br>Downloads results for use in a spreadsheet. (always applies current filters)'),
  );
  $form['download']['button'] = array(
    '#type' => 'submit',
    '#value' => t('Download as CSV'),
    '#submit' => array('download_data'),
  );


  return $form;
}

function apply_query_filters($form_state, $mysqlpart, $timestamp, $headers){
  if(!isset($form_state['input']['all'])){
    $mysqlpart = $mysqlpart->condition('timestamp', $timestamp->fetchField());
  }
  if(isset($form_state['input']['institution'])){
    $filters = [];
    $filters['inst'] = $form_state['input']['institution'];
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

function download_data(&$form,&$form_state){
  $options = array('query' => array('inst' => $form_state['values']['institution'],'cmodel' => $form_state['values']['cmodel'], 'coll' => $form_state['values']['collection'],'all' => $form_state['values']['all']));
  drupal_goto('data/download', $options);
}

function islandora_content_stats_form_submit($form, &$form_state){
  $form_state['rebuild'] = TRUE;
}
