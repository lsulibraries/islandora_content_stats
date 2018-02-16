<?php
require_once dirname(__FILE__) . '/queries.inc';
/**
 * Builds form out of query results from db.
 * accounts for custom content models
 */
function islandora_content_stats_reports_form($form, &$form_state) {
  $cmodels =[]; 
  $ask = all_cmodel_counts();
  foreach($ask as $model){
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

  //$col_counts = collection_cmodel_query('latech-cmprt:collection');
  //dpm($col_counts);

  //$institution_collection_audit = all_institution_cmodel_query();
  //dpm($institution_collection_audit);
  //how to get the name of the collection into the datastructure.
  //same way we got the name of the institution into the query  