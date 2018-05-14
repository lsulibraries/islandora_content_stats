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
  if(!empty($form_state['input'])){
    $results = $query->fields('stats', array('id','inst','coll','cmodel','count','timestamp'))
      ->orderByHeader($headers)
      ->condition('timestamp',$max_timestamp->fetchField())
      ->condition('inst', '%'. db_like($form_state['input']['institution']). '%', 'LIKE')
      ->condition('coll', '%'. db_like($form_state['input']['collection']). '%', 'LIKE')
      ->condition('cmodel', '%'. db_like($form_state['input']['cmodel']). '%', 'LIKE')
      ->extend('TableSort')->extend('PagerDefault')->limit(144)
      ->execute();
  }
  else{
    $results = $query->fields('stats', array('id','inst','coll','cmodel','count','timestamp'))
      ->condition('timestamp',$max_timestamp->fetchField())
      ->orderByHeader($headers)
      ->extend('TableSort')->extend('PagerDefault')->limit(144)
      ->execute();
  }
  foreach($results as $record){
    if($record->count != 0){
      if( ($record->cmodel != 'fedora-system:FedoraObject-3.0') && ($record->cmodel !='fedora-system:ContentModel-3.0')){
        $rows[$record->id] = array(
          'id' => $record->id,
          'inst' => ($record->inst == '' ? 'Site Wide Count' : map_institution($record->inst)),
          'coll' => ($record->coll == '' ? 'institution total' : map_collection($record->coll)),
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

function download_data(&$form,&$form_state){
  $options = array('query' => array('inst' => $form_state['values']['institution'],'cmodel' => $form_state['values']['cmodel'], 'coll' => $form_state['values']['collection']));
  drupal_goto('admin/reports/content_stats/data', $options);
}

function go_to_all(){
  drupal_goto('/admin/reports/content_stats/all');
}

function islandora_content_stats_recent_form_submit($form, &$form_state){
  dpm($form_state);
  $form_state['rebuild'] = TRUE;
}
