<?php
require_once dirname(__FILE__) . '/queries.inc';
/**
 * Builds form out of query results from db.
 * accounts for custom content models
 */
function islandora_content_stats_reports_form($form, &$form_state) {
  $cmodels =[]; 
  $cmodel_types = cmodel_types();
  //$headers = ['ID','Query','Count','Timestamp'];
  foreach($cmodel_types as $model){
      array_push($cmodels,$model['cmodel']['value']);
  }
  //select and format item counts for all content models
  foreach($cmodels as $each){
    $latest_query = db_query("SELECT * FROM ldl.islandora_content_stats WHERE query = '$each' ORDER by timestamp DESC LIMIT 1");
    foreach($latest_query as $record){
       
    $form[$record->id] = array(
             '#type' => 'fieldset',
             '#value' =>"Items: " . $record->count . " Query increment: " . $record->id ,
             '#title' => "Number of items with'" . $record->query . "' content model",
             '#description' => "Date:  ". gmdate("Y-m-d\TH:i:s\Z",$record->timestamp), 
            );
    }
  }
  
  return $form;
}