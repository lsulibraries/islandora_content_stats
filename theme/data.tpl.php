<?php

/**
 * @file
 * This is the template file for the object page for audio file
 * 
 * @TODO: add documentation about file and available variables
 */
?>

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
        <?php foreach ($variables['inst_totals'] as $inst => $model_counts) : ?>
        <div class="inst_wrapper <?php print $inst ?>">
            <?php foreach ($model_counts as $itotal) : ?>
                <div class='cmodel_wrapper_inst'>
                    <div class="inst"><?php print $itotal['inst-label'] ?></div>
                    <div class='cmodel'><?php print $itotal['cmodel'] . ' ' ?></div>
                    <div class='total'><?php print $itotal['count'] ?></div>
                </div>
            <?php endforeach; ?>
        </div>
        <?php endforeach; ?>
    </div>
    <div class='tableStats'>
        <div class='ics_filters'>
            <legend>
                <span class='fieldset-legend'><?php print $variables['lang_filter']?></span>
            </legend>
            <div>Form Dropdowns...</div>
        </div>
        <div class='ics_table_collapse form-wrapper' id='edit-table-relsults'>
            <legend>
                <span><?php print $variables['lang_table'] ?></span>
            </legend>
            <div class='ics_table_explain'><?php print $variables['lang_table_desc'] ?></div>
            <div>TABLE GOES HERE
                <div class='column'>
                    <div class='header'>Institution/Sub-institution</div>
                    <?php foreach  ($variables['latest'] as $record) :?>
                    <div class='row'><?php print $record['inst-label'] ?></div>
                    <?php endforeach; ?>
                </div>
                <div class='column'>
                    <div class='header'>Cmodel</div>
                    <?php foreach ($variables['latest'] as $record) :?>
                        <div class='row'><?php print $record['cmodel'] ?></div>
                    <?php endforeach; ?>
                </div>
                <div class='column'>
                    <div class='header'>Count</div>    
                    <?php foreach ($variables['latest'] as $record) :?>
                        <div class='row <?php print $record['cmodel'] ?>'><?php print $record['count'] ?></div>
                    <?php endforeach; ?>
                </div>
            </div>
        </div>
    </div>
</div>    
</div>
</div>