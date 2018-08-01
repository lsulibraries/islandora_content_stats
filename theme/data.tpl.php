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
        <div class='download form-wrapper'>
            <form>
                <?php print drupal_render_children($variables['dlform']); ?>
                <?php echo drupal_render($variables['dlform']['form_build_id']);?>
                <?php echo drupal_render($variables['dlform']['form_id']);?>
                <?php echo drupal_render($variables['dlform']['actions']);?>
                <?php echo drupal_render($variables['dlform']['form_tokens']);?>
            </form>
        </div>
        <div class='filter form-wrapper'>
            <form>
                <?php print drupal_render_children($variables['filter_form']); ?>
                <?php echo drupal_render($variables['filter_form']['form_build_id']);?>
                <?php echo drupal_render($variables['filter_form']['form_id']);?>
                <?php echo drupal_render($variables['filter_form']['actions']);?>
                <?php echo drupal_render($variables['filter_form']['form_tokens']);?>
            </form>
        </div>
        <div class='ics_table_collapse form-wrapper' id='edit-table-results'>
            <legend>
                <span><?php print $variables['lang_table'] ?></span>
            </legend>
            <div class='ics_table_explain'><?php print $variables['lang_table_desc'] ?></div>
            <div>TABLE GOES HERE
                <div class='column'>
                    <div class='header'>Institution/Sub-institution</div>
                    <?php foreach  ($variables['latest'] as $record) :?>
                    <?php if (isset($variabels['gets']['inst'])) : ?>
                        <?php if ( $variabels['gets']['inst'] == $record['inst']) : ?>
                        <div class='row <?php print $record['inst'] ?>'><?php print $record['inst-label'] ?></div>
                        <?php endif; ?>
                    <?php else : ?>
                        <div class='row <?php print $record['inst'] ?>'><?php print $record['inst-label'] ?></div>
                    <?php endif; ?>
                    <?php endforeach; ?>
                </div>
                <div class='column'>
                    <div class='header'>Cmodel</div>
                    <?php foreach ($variables['latest'] as $record) :?>
                    <?php if (isset($variables['gets']['cmodel'])) : ?> 
                        <?php  if($variables['gets']['cmodel'] == $record['cmodel-id']) : ?>
                            <div class='row'><?php print $record['cmodel-label'] ?></div>
                        <?php endif; ?>
                    <?php else : ?>
                            <div class='row'><?php print $record['cmodel-label'] ?></div>
                    <?php endif; ?>
                    <?php endforeach; ?>
                </div>
                <div class='column'>
                    <div class='header'>Count</div>    
                    <?php foreach ($variables['latest'] as $record) :?>
                        <?php if (isset($variables['gets']['inst']) && !isset($variables['gets']['cmodel'])) : ?>
                            <?php if($variables['gets']['inst'] == $record['inst']) : ?>
                                <div class='row <?php print $record['cmodel-label'] ?>'><?php print $record['count'] ?></div>
                            <?php endif; ?>
                        <?php elseif( isset($variables['gets']['cmodel']) && !isset($variables['gets']['inst'])) : ?>
                            <?php if ($variables['gets']['cmodel'] == $record['cmodel-id']) : ?>
                                <div class='row <?php print $record['cmodel-label'] ?>'><?php print $record['count'] ?></div>
                            <?php endif; ?>
                        <?php else :?>
                            <div class='row <?php print $record['cmodel-label'] ?>'><?php print $record['count'] ?></div>
                        <?php endif; ?>
                    <?php endforeach; ?>
                </div>
            </div>
        </div>
    </div>
</div>    
</div>
</div>