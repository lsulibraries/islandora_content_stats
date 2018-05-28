<?php
//row building function
function getRow($cmodel, $count) {
  if($count != 0){
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
}
function getRow_helper($counts){
  $rows = [];
  if(isset($counts)){
    foreach($counts as $id => $fields){
      $rows[$id] = getRow($fields['cmodel'],$fields['count']);
    }
  }
  return $rows;
}

/**
 * Builds form out of query results from db with latest timestamp.
 * accounts for custom content models
 * to be displayed as a block on homepage, institution page, and collection page
 * not currently implemented
 */
function islandora_content_stats_table_page_form($form, &$form_state){
  module_load_include('inc','islandora','/includes/utilities');
  module_load_include('inc', 'islandora_content_stats', '/includes/queries');
  $cmodel_map = islandora_get_content_models();
  $variables = [];
  //get latest timestamp from run queries
  $q_time = db_query('select MAX(timestamp) from ldl.islandora_content_stats');
  $max_time = $q_time->fetchField();
  $all_inst = all_ns_prefix_incl_subs();
  $uri_split = explode('/', request_uri());
  //find the context of the page: is it front, institution or collection?
  if(strpos(request_uri(), 'collection')){
    $counts = islandora_content_stats_post_collection_stats();
    $rows = getRow_helper($counts);
  }
  elseif( drupal_is_front_page() ){
    $counts = islandora_root_stats_queries();
    $rows = getRow_helper($counts);
  }
  else{
    $counts = islandora_content_stats_post_institution_stats();
    $rows = getRow_helper($counts);
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
