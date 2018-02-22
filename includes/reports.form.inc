<?php
require_once dirname(__FILE__) . '/queries.inc';
/**
 * Builds form out of query results from db.
 * accounts for custom content models
 */
function islandora_content_stats_reports_form($form, &$form_state) {
  $latest_query = db_query("SELECT * FROM ldl.islandora_content_stats  ORDER by timestamp DESC");
  foreach($latest_query as $record){
    $form[$record->id] = array(
             '#type' => 'fieldset',
             '#value' =>"Items: " . $record->count . " Query increment: " . $record->id ,
             '#title' => "Number of items with '" . $record->query . "' content model",
             '#description' => "Date:  ". gmdate("Y-m-d\TH:i:s\Z",$record->timestamp), 
            );
  }
//  $form['table'] = islandora_content_stats_theme_table();
  return $form;
}
  


//function islandora_content_stats_theme_table_form(){
//  $latest_query = db_query("SELECT * FROM ldl.islandora_content_stats  ORDER by timestamp DESC");
//  $headers = ['ID','Query','Count','Timestamp'];
//  $rows = array();
//  foreach($latest_query as $record){
//      $row = array($record->id,$record->query,$record->count,gmdate("Y-m-d\TH:i:s\Z",$record->timestamp));
//      $rows[$record->id] = $row;
//  }
//  $form['table'] = array(
//      '#theme' => 'formtable_form_table',
//      '#header'
//  );
//  return theme('table',$headers,$rows);
//}
