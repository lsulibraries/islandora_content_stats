<?php
module_load_include('inc','islandora','/includes/utilities');


/**
 * Maps a machine name to a human readable $label
 * @param string $model
 *  String of model's machine name
 * @return string
 *  String of model's label stripped of repetitous words
 */
function map_cmodels($model){
  if($model == 'fedora-system:ContentModel-3.0' || $model == 'fedora-system:FedoraObject-3.0'){}
  else{
    $cmodel_map = islandora_get_content_models();
    if($model != '' && array_key_exists($model, $cmodel_map)){
      $label = $cmodel_map[$model];
      $label_f = str_replace(' Content Model','',str_replace('Islandora ','',str_replace('Large ','', $label['label'])));
    return $label_f;
    }
  }
}

/**
 * Maps an institution prefix to an intitution's title
 * @param string $inst
 * String of institutional prefix
 * @param string $form
 *  string indicates function is used for form dropdown
 * @return string
 * String of institution's title
 */
function map_institution($inst,$form = ''){
  if($form != ''){}
  if($inst == 'islandora:root'){return $inst;}
  else{
    $inst = rtrim($inst,'-');
    $query = "select * from islandora_namespace_homepage where prefix = '".$inst."'";
    $ns_homepage_table = db_query($query);
    if($ns_homepage_table->rowCount()==0){return $inst;}
    else{
      foreach($ns_homepage_table as $row){
        return $row->title;
      }
    }
  }
}

/**
 * Maps a collection pid to a collection label
 * @param string $pid
 * String of collection pid
 * @param string $context
 *  string indicates function is used for display in a form, or csv download.
 * @return string
 * String of institution's title with link to item
 */
function map_collection($pid, $context = FALSE){
  if($pid != '' ){
    $obj = islandora_object_load($pid);
    if(isset($obj)){
      $label = $obj->label;
      if($context == 'csv'){
        $link = 'http://'.$_SERVER['SERVER_NAME'].'/islandora/object/'.$pid;
      }
      elseif($context == 'form'){
        $link = '<a href = "../../islandora/object/'.$pid.'">'.$label.'</a>';
      }
      else{
        $link = $label;
      }
      return $link;
    }
  }
}

//Block Related functions
function islandora_content_stats_post_collection_stats(){
  if(strpos(request_uri(),'collection')){
    $str = request_uri();
    $coll = str_replace('/islandora/object/','',$str);
    return islandora_content_stats_page_cmodels_coll($coll);

  }
}

function check_path(){
  $ns_prefixes = update_namespace_prefixes_cache();
  $page = str_replace('/','',request_uri());
      foreach($ns_prefixes as $prefix){
        if($prefix == $page){
        return true;
      }

    }
}

function islandora_content_stats_post_institution_stats(){
  if(check_path()){
    return islandora_content_stats_page_cmodels_inst(str_replace('/','',request_uri()));
  }
  else{
    return;
  }
}
