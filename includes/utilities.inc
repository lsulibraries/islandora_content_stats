<?php
module_load_include('inc','islandora','/includes/utilities');


function map_islandora_cmodels($query){
    $cmodel_map = islandora_get_content_models();
    $query_pop = explode(' ',$query);
    $front = reset($query_pop);
    $query_pop[0] = map_institutional_prefix_to_title($front);
    $end = end($query_pop);
    $human_label = $cmodel_map[$end];
    array_pop($query_pop);
    array_push($query_pop,$human_label['label']);
    $pre_filtered = implode(' ', $query_pop);
    $pre_plural = str_replace(' Content Model','',str_replace('Islandora ','',str_replace('Large ','',$pre_filtered)));
    $fixend = explode(' ',$pre_plural);
    return $pre_plural.'s';
}

function map_institutional_prefix_to_title($prefix){
  if($prefix == 'islandora:root'){return $prefix;}
  $prefix = rtrim($prefix,'-');
  $query = "select * from islandora_namespace_homepage where prefix = '".$prefix."'";
  $ns_homepage_table = db_query($query);
  if($ns_homepage_table->rowCount()==0){return $prefix;}
  else{
    foreach($ns_homepage_table as $row){
      return $row->title;
    }
  }
}
