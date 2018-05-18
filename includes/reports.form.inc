<?php
require_once dirname(__FILE__) . '/queries.inc';
require_once dirname(__FILE__) . '/utilities.inc';

/**
 * Builds form out of query results from db with latest timestamp.
 * accounts for custom content models
 */
function islandora_content_stats_recent_form($form, &$form_state){
  $rows = [];
  $headers = array(
    array('data'=> t('ID'),'field' =>'stats.id','sort' => 'desc'),
    array('data'=> t('Instituton/Context'), 'field' =>'stats.inst',),
    array('data' => t('Collection (optional'), 'field' =>'stats.coll',),
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
      if( ($record->cmodel != 'fedora-system:FedoraObject-3.0') && ($record->cmodel !='fedora-system:ContentModel-3.0')){
        $rows[$record->id] = array(
          'id' => $record->id,
          'inst' => ($record->inst == '' ? 'All' : map_institution($record->inst)),
          'coll' => ($record->coll == '' ? 'All' : map_collection($record->coll)),
          'cmodel' =>  map_cmodels($record->cmodel),
          'count' => $record->count,
          'timestamp' => gmdate("Y-m-d\TH:i:s\Z",$record->timestamp),
        );
      }
    }
  }
  $form = [];
  $form['filter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter query results'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['filter']['cmodel'] = array(
    '#type' => 'select',
    '#title' => t('Filter by type of object'),
    '#options' => cmodel_types_for_form(),
    '#description' => t('content model names'),
    '#required' => FALSE,
    '#empty_option' => '--none--',
  );
  $form['filter']['institution'] = array(
    '#type' => 'select',
    '#title' => t('Filter by institution or sub-institution'),
    '#options' => institutions_for_form(),
    '#required' => FALSE,
    '#description' => t('institution or sub-institution prefix (.i.e. "lsu")'),
    '#empty_option' => '--none--',
  );
  $form['filter']['collection'] = array(
    '#type' => 'select',
    '#title' => t('Filter by specific Collection'),
    '#options' => collection_pid_labels_for_form(),
    '#required' => FALSE,
    '#empty_option' => '--none--',
  );
  $form['filter']['all'] = array(
    '#type' => 'checkbox',
    '#title' => t('Show all results (history)')
  );
  $form['filter']['download'] = array(
    '#type' => 'submit',
    '#value' => t('Download as CSV'),
    '#submit' => array('download_data'),
  );
  $form['filter']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
  );
  $form['table'] = array(
    '#theme' => 'table',
    '#header' => $headers,
    '#rows' => $rows,
    '#empty' => t('table is empty')
  );
  $form['pager'] = array('#markup' => theme('pager'));
  $form['show_all'] = array (
    '#type' => 'submit',
    '#value' => t('Show All Queries'),
    '#submit' => array('go_to_all')
  );
  return $form;
}

function apply_query_filters($form_state, $mysqlpart, $timestamp, $headers){
  if(isset($form_state['input']['all'])){
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
        $mysqlpart = $mysqlpart->condition($key,'%'.db_like($val).'%','LIKE');
      }
    }
  }
  return $mysqlpart->orderByHeader($headers)
    ->extend('TableSort')->extend('PagerDefault')->limit(144)
    ->execute();
}

function download_data(&$form,&$form_state){
  $options = array('query' => array('inst' => $form_state['values']['institution'],'cmodel' => $form_state['values']['cmodel'], 'coll' => $form_state['values']['collection']));
  drupal_goto('admin/reports/content_stats/data', $options);
}

function go_to_all(){
  drupal_goto('/admin/reports/content_stats/all');
}

function islandora_content_stats_recent_form_submit($form, &$form_state){
  $form_state['rebuild'] = TRUE;
}
