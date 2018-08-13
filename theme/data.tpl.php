<?php

/**
 * @file
 * This is the template file for the object page for audio file
 *
 * @TODO: add documentation about file and available variables
 */
?>

<div class="dataPage">
  <div class="backgroundDiv"></div>
  <div class="dataHeader">
    <div class='headerTitle'><?php print $variables['lang']['pageHeader']; ?></div>
  </div>
  <div class='headerDescription'> <?php print $variables['lang']['pageDesc']; print $variables['lang']['lastRun']; ?></div>
  <div class='glanceStats'>
    <div class='globalContainer'>
      <div class='globalHeader statHeader'><?php print $variables['lang']['globalHeader']?></div>
      <div class='globalHeader_desc statHeader'><?php print $variables['lang']['globalDesc']?></div>
      <div class='globalStats global_totals'>
      <canvas id="globalChart" width="400" height="400"></canvas>
      <?php foreach ($variables['global_totals'] as $global) :?>
        <div class='globalStat'>
          <?php if( $global['cmodel'] == 'Collection') : ?>
          <div class='collections'>
            <div class="cmodel <?php print $global['cmodel'] ?>"><?php print $global['cmodel']; ?></div>
            <div class="total <?php print $global['count'] ?>"><?php print $global['count']; ?></div>
          </div>
           <?php else: ?>
            <div class='cmodel <?php print $global['cmodel'] ?>'><?php print $global['cmodel'] ?></div>
            <div class='total <?php print $global['count'] ?>'><?php print $global['count']; ?></div>
          <?php endif; ?>
        </div>
      <?php endforeach; ?>
      </div>
    </div>
    <div class='instContainer'>
      <div class='instHeader statHeader'><?php print $variables['lang']['instHeader']; ?></div>
      <div class='instHeader_desc statHeader'><?php print $variables['lang']['instDesc']; ?></div>
      <div class='instStats instGroup' data-masonry='{ "columnWidth": 212, "itemSelector": ".inst_wrapper" }'>
          <?php foreach ($variables['inst_totals'] as $inst => $model_counts) : ?>
          <div class="inst_wrapper <?php print $inst ?>">
              <?php foreach ($model_counts as $itotal) : ?>
                  <div class='cmodel_wrapper_inst'>
                      <div class="inst"><?php print $itotal['inst-label']; ?></div>
                      <div class='cmodel'><?php print $itotal['cmodel']; ?></div>
                      <div class='total'><?php print $itotal['count']; ?></div>
                  </div>
              <?php endforeach; ?>
          </div>
          <?php endforeach; ?>
      </div>
    </div>
  </div>
    <div class='tableStats'>
        <div class='ics_table_collapse form-wrapper' id='edit-table-results'>
            <legend>
                <span><?php print $variables['lang']['tableTitle']; ?></span>
            </legend>
            <div class='ics_table_explain'><?php print $variables['lang']['tableDesc']; ?></div>
            <div class='filter download form-wrapper'>
                <form>
                  <?php $form = drupal_get_form('islandora_content_stats_data_filter_form'); print drupal_render($form); ?>
                </form>
            </div>
            <div class='table'>
                <div class='column'>
                    <div class='header'>
                      <?php $insturl = $variables['insturl']?>
                      <?php print "<a href='$insturl'>Institution/Sub-institution</a>"?>
                    </div>
                    <?php foreach  ($variables['latest'] as $record) :?>
                      <div class='row <?php print $record['inst-id']; print $record['cmodel-id'] ?>'><?php print $record['inst-label']; ?></div>
                    <?php endforeach; ?>
                </div>
                <!-- <div class='column'>
                  <div class='header'>Collection</div>
                  <?php //foreach  ($variables['latest'] as $record) :?>
                        <div class='row <?php //print $record['coll']; ?>'><?php //print $record['coll-label']; ?></div>
                        <?php //endforeach; ?>
                </div> -->
                <div class='column'>
                  <div class='header'>
                    <?php $typeurl = $variables['typeurl']?>
                    <?php print "<a href='$typeurl'>Type</a>"?>
                  </div>
                  <?php foreach  ($variables['latest'] as $record) :?>
                        <div class='row <?php print $record['cmodel-id']; ?>'><?php print $record['cmodel-label']; ?></div>
                        <?php endforeach; ?>
                </div>
                <div class='column'>
                    <div class='header'>
                      <?php $counturl = $variables['counturl']?>
                      <?php print "<a href='$counturl'>Count</a>"?>
                      </div>
                    <?php foreach  ($variables['latest'] as $record) :?>
                      <div class='row <?php print $record['cmodel-id']; print $record['inst-id'] ?>'><?php print $record['count-label']; ?></div>
                    <?php endforeach; ?>
                </div>
            </div>
        </div>
    </div>
</div>
