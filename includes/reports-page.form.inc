<?php
require_once dirname(__FILE__) . '/queries.inc';
require_once dirname(__FILE__) . '/../islandora_content_stats.module';
module_load_include('inc','islandora','/includes/utilities');

//row building function
function getRow($cmodel, $count) {
    return array(
       'model' => array(
        'data' => map_cmodels($cmodel),
        'class' => array('content_stats_model'),
       ),
       'count' => array(
        'data' => $count,
        'class' => array('content_stats_count'),
       ),
      );
}
/**
 * Builds form out of query results from db with latest timestamp.
 * accounts for custom content models
 * to be displayed as a block on homepage, institution page, and collection page
 */
function islandora_content_stats_table_page_form($form, &$form_state){
  $cmodel_map = islandora_get_content_models();
  $variables = [];
  $rows = [];
  //get latest timestamp from run queries
  $max_time = '';
  $q_time = db_query('select MAX(timestamp) from ldl.islandora_content_stats');
  foreach($q_time as $return){
    foreach($return as $key => $value){
        $max_time = $value;
    }
  }
  $all_inst = all_ns_prefix_incl_subs();
  $uri_split = explode('/', request_uri());
  //find the context of the page: is it front, institution or collection?
  if(strpos(request_uri(), 'collection')){
    $counts = islandora_content_stats_post_collection_stats();
    if(isset($counts)){
      foreach($counts as $id => $fields){
        if($fields['count'] != 0){
          $rows[$id] = getRow($fields['cmodel'],$fields['count']);
        }
      }
    }
  }
  elseif( drupal_is_front_page() ){
    $counts = islandora_root_page_stats();
    if(isset($counts)){
        foreach($counts as $id => $fields){
          if($fields['count'] != 0){
              $rows[$id] =  getRow($fields['cmodel'], $fields['count']);
          }
        }
      }
    }
  else{
    $counts = islandora_content_stats_post_institution_stats();
    if(isset($counts)){
      foreach($counts as $id => $fields){
        if($fields['count'] != 0){
              $rows[$id] = getRow($fields['cmodel'], $fields['count']);
            }
          }
        }
      }
  //build up the form
  $form = [];
    $headers =  array(
      array('data'=> t('Cmodel'),'field' =>'stats.id','class' => array('islandora_content_stats_trow_model'),),
      array('data'=> t('Count'),'field' =>'stats.query','class' => array('islandora_content_stats_trow_count'),),
    );

    $form['table'] = array(
      '#theme' => 'table',
      '#header' => $headers,
      '#rows' => $rows,
      '#empty' => t('table is empty'),
      '#attributes' => array('class' => array('islandora_content_stats_page_table')),
      );
  return $form;
}

function islandora_content_stats_table_page_form_submit($form, &$form_state){
  $form_state['rebuild'] = TRUE;
}
