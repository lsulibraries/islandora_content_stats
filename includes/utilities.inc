<?php
module_load_include('inc','islandora','/includes/utilities');

function map_islandora_cmodels($query, $csv = FALSE){
    $cmodel_map = islandora_get_content_models();
    $query_array = explode(' ',$query);
    $coll_link = '';
    if(count($query_array) == 3){
      if(strpos($query_array[1],'collection')){
        $pid = $query_array[1];
        $obj = islandora_object_load($query_array[1]);
        $query_array[1] = $obj->label.' |';
        if($csv){
          $coll_link = '=HYPERLINK("http://'.$_SERVER['SERVER_NAME'].':8000/islandora/object/'.$pid; //Not going to be 8000 in future
        }
        else{
          $coll_link = ' <a href = "../../islandora/object/'.$pid.'">Link</a>';
        }
      }
    }
    $inst = reset($query_array);
    $query_array[0] = map_institutional_prefix_to_title($inst);
    $cmodel = end($query_array);
    $human_label = $cmodel_map[$cmodel];
    array_pop($query_array);
    array_push($query_array,$human_label['label']);
    $pre_filtered = implode(' ', $query_array);
    $pre_plural_str = str_replace(' Content Model','',str_replace('Islandora ','',str_replace('Large ','',$pre_filtered)));
    $pre_plural_array = explode(' ',$pre_plural_str);
    $no_link = (end($pre_plural_array) == ' ' ? $pre_plural_str.' |' : $pre_plural_str.'s |');
    ($csv != '' ? $return = $no_link : $return = $no_link.$coll_link );
    return $return;
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




//Block Related functions
function islandora_content_stats_post_collection_stats(){
  if(strpos(current_path(),'collection')){
    $str =current_path();
    $coll = str_replace('islandora/object/','',$str);
    return islandora_content_stats_page_cmodels($coll);
  }
}

function check_path(){
  $ns_prefixes = update_namespace_prefixes_cache();
      foreach($ns_prefixes as $prefix){
      if(preg_match('/'.$prefix.'/', str_replace('/','',request_uri()))){
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
