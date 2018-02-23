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
  


function islandora_content_stats_table_form($form, &$form_state){
  $latest_query = db_query("SELECT * FROM ldl.islandora_content_stats ORDER by id DESC");
  $headers = ['ID','Query','Count','Timestamp'];
  $form['header'] =array(
    '#prefix' => '<thead><tr><th>'.$headers[0].'</th><th>'.$headers[1].'</th><th>'.$headers[2].'</th><th>'.$headers[3],
      '#suffix' => '</th><tr><thead>',
      
  );
  $rows = array();
  $form['#prefix'] = '<table cellpadding=3 cellspacing=3 border=1px solid black>';
  $form['#suffix'] = '</table>';
  foreach($latest_query as $record){
      $class = ($record->id % 2 == 0) ? 'odd' :'even';

      $row = array($record->id,$record->query,$record->count,gmdate("Y-m-d\TH:i:s\Z",$record->timestamp));
      $rows[$record->id] = $row;
           $form[$record->id] = array(
         '#prefix' => '<tr class='.$class.'><td>'.$record->id.'</td><td>'.$record->query.'<td>'.$record->count.'</td><td>'.gmdate("Y-m-d\TH:i:s\Z",$record->timestamp).'</td>',
         '#suffix' => '</td></tr>',
         );
  }


 

  return $form;
}
