<?php

function tripal_diamond_user_analysis_page($uid) {
  $content = array(
    'page_title' => array(
      '#type' => 'markup',
      '#markup' => '<h2>Your Uploaded Files</h2>',
    ),
    'page_description' => array(
      '#type' => 'markup',
      '#markup' => '<p>' . t('This is where all your TSeq Jobs will live!') . '</p>',
    )
  );
  return $content;
}