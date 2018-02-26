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

////using a less sophisticated form api.
//function islandora_content_stats_table_form($form, &$form_state){
//  $latest_query = db_query("SELECT * FROM ldl.islandora_content_stats ORDER by id DESC");
//  $headers = ['ID','Query','Count','Timestamp'];
//  $form['header'] =array(
//    '#prefix' => '<thead><tr><th>'.$headers[0].'</th><th>'.$headers[1].'</th><th>'.$headers[2].'</th><th>'.$headers[3],
//      '#suffix' => '</th><tr><thead>',
//      
//  );
//  $rows = array();
//  $form['#prefix'] = '<table cellpadding=3 cellspacing=3 border=1px solid black>';
//  $form['#suffix'] = '</table>';
//  foreach($latest_query as $record){
//      $class = ($record->id % 2 == 0) ? 'odd' :'even';
//
//      $row = array($record->id,$record->query,$record->count,gmdate("Y-m-d\TH:i:s\Z",$record->timestamp));
//      $rows[$record->id] = $row;
//           $form[$record->id] = array(
//         '#prefix' => '<tr class='.$class.'><td>'.$record->id.'</td><td>'.$record->query.'<td>'.$record->count.'</td><td>'.gmdate("Y-m-d\TH:i:s\Z",$record->timestamp).'</td>',
//         '#suffix' => '</td></tr>',
//         );
//  }
//  return $form;
//}


//same result as above but don't use page_arguments, use page_callback & comment out page_arguments.
//function islandora_content_stats_table_theme_form($form, &$form_state){
function islandora_content_stats_table_theme(){
  $variables = [];
  
    $headers = array(
      array('data'=>'ID','field' =>'stats.id','sort' => 'desc'),
      array('data'=>'Query','field' =>'stats.query',),
      array('data'=>'Count','field' =>'stats.count',),
      array('data'=>'Timestamp','field' =>'stats.timestamp',),
    );
    
        //second argument in db select is an alias for the table(which is the first argument.)
        $query = db_select("islandora_content_stats","stats")
                ->extend('TableSort');
        
        //somehow get a variable from a textfield with a submit into the db_like function...
        $results = $query->fields('stats',array('id','query','count','timestamp'))
                ->orderByHeader($headers)
                ->condition('query', '%'. db_like(''). '%', 'LIKE')
                ->execute();
        
  $rows = [];

  foreach($results as $record){
      $rows[$record->id] = array(
          'id' => $record->id,
          'query' => $record->query,
          'count' => $record->count,
          'timestamp' => gmdate("Y-m-d\TH:i:s\Z",$record->timestamp)
          );
              
 }
  $variables = array(
      'header' => $headers,
      'rows' => $rows,
      'attributes' => array(),
      'caption' => '',
      'colgroups' => array(),
      'sticky' => FALSE,
      'empty' => 'No data available'      
  );
    
  //trying to add a user submitted filter from a textfield.
  //haven't had luck turning this into a form function...
//$form['like'] = array(
//  '#type' => 'textfield',
//   
//);
// $form['table'] = theme_table($variables);
// 
// $form['submit'] = array (
//  '#type' => 'submit',
//  '#value' => t('Submit')
// );
//  return $form;  
  $output = theme_table($variables);
  return $output;
  
}

//islandora_content_stats_table_theme();
 //islandora_content_stats_theme_table(islandora_content_stats_table_prep());

//  
// function islandora_content_stats_theme_table($variables){
//     $output = theme_table('islandora_content_stats',array('header'=>$variables['headers'],'rows'=>$variables['rows']));
//     return $output;
// }
 
