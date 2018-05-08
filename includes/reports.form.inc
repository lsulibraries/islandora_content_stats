<?php
require_once dirname(__FILE__) . '/queries.inc';
require_once dirname(__FILE__) . '/utilities.inc';


/**
 * Builds form out of query results from db with latest timestamp.
 * accounts for custom content models
 */
function islandora_content_stats_table_form($form, &$form_state){
  $variables = [];
  $rows = [];
  $headers = array(
    array('data'=> t('ID'),'field' =>'stats.id', 'sort' => 'desc',),
    array('data'=> t('Query'),'field' => 'stats.query',),
    array('data'=> t('Count'),'field' => 'stats.count',),
    array('data'=> t('Timestamp'),'field' => 'stats.timestamp',),
  );
  //get latest timestamp from run queries
  $max_time = '';
  $q_time = db_query('select MAX(timestamp) from ldl.islandora_content_stats');
  foreach($q_time as $return){
    foreach($return as $key => $value){
        $max_time = $value;
    }
  }
  //second argument in db select is an alias for the table(which is the first argument.)
  $query = db_select("islandora_content_stats","stats")
    ->extend('TableSort');
  if(!empty($form_state['input']) && $form_state['input']['op'] != 'filter'){

    //somehow get a variable from a textfield with a submit into the db_like function...
    $results = $query->fields('stats',array('id','query','count','timestamp'))
      ->orderByHeader($headers)
      ->condition('timestamp',$max_time)
      ->condition('query', '%'. db_like($form_state['input']['institution']). '%', 'LIKE')
      ->condition('query', '%'. db_like($form_state['input']['cmodel']). '%', 'LIKE')
      ->condition('query', '%'. db_like($form_state['input']['collection']). '%', 'LIKE')
      ->extend('TableSort')->extend('PagerDefault')->limit(144)
      ->execute();
  }
  else{
    $results = $query->fields('stats',array('id','query','count','timestamp'))
      ->condition('timestamp',$max_time)
      ->orderByHeader($headers)
      ->extend('TableSort')->extend('PagerDefault')->limit(144)
      ->execute();
  }
  foreach($results as $record){
    if($record->count != 0){
      if(strpos($record->query,'fedora-system:FedoraObject-3.0') || strpos($record->query,'fedora-system:ContentModel-3.0')){}
      else{
        $label = map_islandora_cmodels($record->query);
        $rows[$record->id] = array(
          'id' => $record->id,
          'query' => $label,
          'count' => $record->count,
          'timestamp' => gmdate("Y-m-d\TH:i:s\Z",$record->timestamp)
        );
      }
    }
  }
  $form = [];
  $form['filter'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter Query Results'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['filter']['institution'] = array(
    '#type' => 'textfield',
    '#title' => t('Institution'),
    '#size' => 15,
    '#description' => t('institution or sub-institution prefix (.i.e. "lsu")')
  );
  $form['filter']['cmodel'] = array(
    '#type' => 'textfield',
    '#title' => t('Content Model'),
    '#size' => 15,
    '#description' => t('content model string/sub-string (.i.e. "large_image" or "FedoraObject")')
  );
  $form['filter']['collection'] = array(
    '#type' => 'textfield',
    '#title' => t('Collection PID'),
    '#size' => 15,
    '#description' => t('PID string/sub-string (.i.e. "dolls:collection" or "lsu-hpl")')
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
  drupal_goto('/admin/reports/content_stats/download', $options);
}

function go_to_all(){
  drupal_goto('/admin/reports/content_stats/all');
}
