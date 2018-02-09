<?php
require_once dirname(__FILE__) . '/queries.inc';
/**
 * Builds form out of query results.
 * Change to query database.
 */
function islandora_content_stats_reports_form($form, &$form_state) {
//    $counts = all_cmodel_counts();
//    foreach ($counts as $count){
//        $form[$count['cmodel']['value']] = array(
//         '#type' => 'fieldset',
//         '#value' => $count['k0']['value'],
//         '#title' => $count['cmodel']['value'],
//         '#description' => t('Current active items'),
//        );
//    }
$col_counts = collection_cmodel_query('latech-cmprt:collection');
//dpm($col_counts);
//    foreach ($col_counts[1] as $col_count){
//        $form[$col_counts[0].$col_count['cmodel']['value']] = array(
//         '#type' => 'fieldset',
//         '#value' => $col_count['k0']['value'],
//         '#title' => $col_counts[0].': '.$col_count['cmodel']['value'],
//         '#description' => t('collection active items'),
//        );
//    }
  $institution_collection_audit = all_institution_cmodel_query();
  //dpm($institution_collection_audit);
//  foreach($institution_collection_audit as $inst => $count){
//    if(isset($count)){
//      foreach($count as $key => $val){
//        $form[$inst][$key] = array(
//        '#type' => 'fieldset',
//        '#value'=>$val,
//        '#title'=>preg_replace('/-/',' ',$inst) .' Count of: ' .  $key,
//        //'#description'=> $val,
//        );
//      }
//    }  
//  }
  
  $query_result = db_query('SELECT * FROM ldl.islandora_content_stats ORDER BY id DESC');
  foreach($query_result as $record){


//How do I conditionally return the last run id for a given query
//bc this does not work...      
//      foreach($query_result as $check){
//      if($record->query == $check->query  && intval($record->id ) < intval($check->id)){
//              dpm( gettype($record->id) );
//           $form[$record->id] = array(
//             '#type' => 'fieldset',
//             '#value' =>"Results: " . $record->count . "@ " . $record->id ,
//             '#title' => "Query for number of '" . $record->query . "' content model",
//             '#description' => "Time of query ". gmdate("Y-m-d\TH:i:s\Z",$record->timestamp), 
//            );
//          }      
//        }  
//  }
      
    $form[$record->id] = array(
             '#type' => 'fieldset',
             '#value' =>"Results: " . $record->count . " @ id: " . $record->id ,
             '#title' => "Query for number of '" . $record->query . "' content model",
             '#description' => "Time of query ". gmdate("Y-m-d\TH:i:s\Z",$record->timestamp), 
            );
  }
  
return $form;
}

