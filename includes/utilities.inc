<?php
module_load_include('inc','islandora','/includes/utilities');

function map_islandora_cmodels($query,$csv = FALSE){
    $cmodel_map = islandora_get_content_models();
    $query_pop = explode(' ',$query);
    $coll_link = '';
    if(count($query_pop) == 3){
      if(strpos($query_pop[1],'collection')){
        $pid = $query_pop[1];
        $obj = islandora_object_load($query_pop[1]);
        $query_pop[1] = $obj->label.' |';
        if($csv){
          $coll_link = '=HYPERLINK("http://'.$_SERVER['SERVER_NAME'].':8000/islandora/object/'.$pid;
        }
        else{
          $coll_link = ' <a href = "../../islandora/object/'.$pid.'">Link</a>';
        }
      }
    }
    $front = reset($query_pop);
    $query_pop[0] = map_institutional_prefix_to_title($front);
    $end = end($query_pop);
    $human_label = $cmodel_map[$end];
    array_pop($query_pop);
    array_push($query_pop,$human_label['label']);
    $pre_filtered = implode(' ', $query_pop);
    $pre_plural = str_replace(' Content Model','',str_replace('Islandora ','',str_replace('Large ','',$pre_filtered)));
    $fixend = explode(' ',$pre_plural);
    $pre_plural = (end($fixend) == ' ' ? $pre_plural.' |' : $pre_plural.'s |');
    if($coll_link != ''){
      ($csv != '' ? $return = $coll_link.'";"'.$pre_plural.'")' : $return = $pre_plural.$coll_link);
      return $return;
    }
    else{
      return $pre_plural;
  }
}

function map_institutional_prefix_to_title($prefix){
  if($prefix == 'islandora:root'){return $prefix.' |';}
  $prefix = rtrim($prefix,'-');
  $query = "select * from islandora_namespace_homepage where prefix = '".$prefix."'";
  $ns_homepage_table = db_query($query);
  if($ns_homepage_table->rowCount()==0){return $prefix.' |';}
  else{
    foreach($ns_homepage_table as $row){
      return $row->title.' |';
    }
  }
}


function islandora_content_stats_post_collection_stats(){
  if(strpos(current_path(),'collection')){
    $str =current_path();
    $coll = str_replace('islandora/object/','',$str);
    return islandora_content_stats_page_cmodels($coll);
  }
}

function check_path(){
  $one_of_these = update_namespace_prefixes_cache();
      foreach($one_of_these as $ns_prefix){
      if(preg_match('/'.$ns_prefix.'/', str_replace('/','',request_uri()))){
        return true;
      }
    }
}

function islandora_content_stats_post_institution_stats(){
  if(!preg_match("/([\/]\w+[\/]\w+)/", request_uri())){
    if(check_path()){
      return islandora_content_stats_page_cmodels(str_replace('/','',request_uri()));
    }
  }
}

function islandora_root_page_stats(){
  return islandora_root_stats_queries();
}