<?php

namespace Drupal\tripal_seq\Form;

use Drupal\Core\Form\FormInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal;

class Tripal_SeqAdminForm {

    /**
     * Database entries
     */
    function tripal_seq_db_add() {
        return 0;
    }
    
    function tripal_seq_db_edit($db_id) {
        return 0;
    }

    function tripal_seq_db_delete($db_id) {
        return 0;
    }

    /**
     * Handle import/export of the Diamond and BLAST database list.
     */
    function tripal_seq_db_import() {

    }

    function tripal_seq_db_export() {

    }

    /**
     * Category entries
     */
    function tripal_seq_category_add() {
        return 0;
    }

    function tripal_seq_category_edit($category_id) {
        return 0;
    }

    function tripal_seq_category_delete($category_id) {
        return 0;
    }

    function tripal_seq_categories_import() {
        return 0;
    }

    function tripal_seq_categories_export() {
        return 0;
    }
}