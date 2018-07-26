<?php

/**
 * @file
 * This is the template file for the object page for audio file
 * 
 * @TODO: add documentation about file and available variables
 */
?>

<?php //dpm($variables) ?>

<div>
<div class="page_description"><?php print $variables['lang_desc'];?></div>
<div class="last_run">Last run at: <?php print $variables['last_run'];?></div>


<div class='global_totals'>
<?php foreach ($variables['global_totals'] as $global) :?>
<div class='gloabl'>
    <div class='cmodel'><?php print $global['cmodel'] ?></div>
    <div class='total'><?php print $global['count'] ?></div>
</div>
<?php endforeach; ?>
</div>


<div class='instGroup'>
<?php foreach ($variables['inst_totals'] as $itotal) : ?>
<div class="inst_wrapper <?php print $itotal['inst'] ?>">    
    <div class='cmodel'><?php print $itotal['cmodel'] . ' ' ?></div>
    <div class="inst"><?php print $itotal['inst-label'] ?></div>
    <div class='total'><?php print $itotal['count'] ?></div>

</div>
<?php endforeach; ?>
</div>


<div class='tableStats'>
    <div class='ics_filter'>
        <legend>
            <span class='fieldset-legend'><?php print $variables['lang_filter']?></span>
        </legend>
    </div>
</div>


<!-- <div class=''><?php print $itotal['id'] . ' ' ?></div> -->
<!-- <div class=''><?php print $itotal['coll'] . ' ' ?></div> -->
<!-- <div class=''><?php print $itotal['timestamp'] . ' ' ?></div> -->

</div>