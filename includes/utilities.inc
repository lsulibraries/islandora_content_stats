<?php
/**
 * Maps a machine name to a human readable $label
 * @param string $model
 *  String of model's machine name
 * @return string
 *  String of model's label stripped of repetitous words
 */
function map_cmodels($model){
  module_load_include('inc','islandora','includes/utilities');
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
 * Maps a collection pid to a collection label
 * @param string $pid
 * String of collection pid
 * @param string $context
 *  string indicates function is used for display in a form, or csv download.
 * @return string
 * String of collection title with link to item
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


//Block Related function tied to report-page.form.inc
//applies a path check to determin if on a 'collection page'
//unused by default
function islandora_content_stats_post_collection_stats(){
  if(strpos(request_uri(),'collection')){
    $str = request_uri();
    $coll = str_replace('/islandora/object/','',$str);
    return islandora_content_stats_page_cmodels_coll($coll);
  }
}
